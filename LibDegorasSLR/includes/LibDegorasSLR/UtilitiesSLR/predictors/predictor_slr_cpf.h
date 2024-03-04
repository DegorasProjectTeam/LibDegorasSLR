/***********************************************************************************************************************
 *   LibDegorasSLR (Degoras Project SLR Library).                                                                      *
 *                                                                                                                     *
 *   A modern and efficient C++ base library for Satellite Laser Ranging (SLR) software and real-time hardware         *
 *   related developments. Developed as a free software under the context of Degoras Project for the Spanish Navy      *
 *   Observatory SLR station (SFEL) in San Fernando and, of course, for any other station that wants to use it!        *
 *                                                                                                                     *
 *   Copyright (C) 2024 Degoras Project Team                                                                           *
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >                             *
 *                      < Jesús Relinque Madroñal >                                                                    *
 *                                                                                                                     *
 *   This file is part of LibDegorasSLR.                                                                               *
 *                                                                                                                     *
 *   Licensed under the European Union Public License (EUPL), Version 1.2 or subsequent versions of the EUPL license   *
 *   as soon they will be approved by the European Commission (IDABC).                                                 *
 *                                                                                                                     *
 *   This project is free software: you can redistribute it and/or modify it under the terms of the EUPL license as    *
 *   published by the IDABC, either Version 1.2 or, at your option, any later version.                                 *
 *                                                                                                                     *
 *   This project is distributed in the hope that it will be useful. Unless required by applicable law or agreed to in *
 *   writing, it is distributed on an "AS IS" basis, WITHOUT ANY WARRANTY OR CONDITIONS OF ANY KIND; without even the  *
 *   implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the EUPL license to check specific   *
 *   language governing permissions and limitations and more details.                                                  *
 *                                                                                                                     *
 *   You should use this project in compliance with the EUPL license. You should have received a copy of the license   *
 *   along with this project. If not, see the license at < https://eupl.eu/ >.                                         *
 **********************************************************************************************************************/

/** ********************************************************************************************************************
 * @file predictor_cpf.h
 * @author Degoras Project Team
 * @brief This file contains the definition of the PredictorCPF class.
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/UtilitiesSLR/predictors/predictor_slr_base.h"
#include "LibDegorasSLR/FormatsILRS/cpf/cpf.h"
#include "LibDegorasSLR/Geophysics/types/geodetic_point.h"
#include "LibDegorasSLR/Geophysics/types/geocentric_point.h"
#include "LibDegorasSLR/Mathematics/types/matrix.h"
#include "LibDegorasSLR/Mathematics/types/vector3d.h"
#include "LibDegorasSLR/Statistics/types/statistics_types.h"
#include "LibDegorasSLR/Geophysics/meteo.h"
#include "LibDegorasSLR/UtilitiesSLR/predictors/prediction_data_slr.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace utils{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using geo::types::GeocentricPoint;
using geo::types::GeodeticPoint;
using ilrs::cpf::CPF;
using math::types::Matrix;
using math::types::Vector3D;
using math::units::Degrees;
using math::units::Picoseconds;
using math::units::Milliseconds;
using geo::meteo::WtrVapPressModel;
using timing::MJDateTime;
using timing::types::MJDateTimeV;
// ---------------------------------------------------------------------------------------------------------------------

// CONSTANTS
// =====================================================================================================================
constexpr long double kVelTDelta = 0.5L;  ///< Time delta (+-) for calculating the velocity vectors (seconds).
constexpr long double kTMargin = 1L;      ///< Margin to apply to the instant sod to check the CPF data interval.
constexpr unsigned kPolLagDeg9 = 9U;      ///< Fixed degree for the 9th polynomial lagrange interpolator.
constexpr unsigned kPolLagDeg16 = 16U;    ///< Fixed degree for the 16th polynomial lagrange interpolator.
// =====================================================================================================================

/**
 * @brief This class implements PredictorCPF class, a PredictorSLR that interpolates CPF positions.
 *
 * @note CPF velocities are not used for interpolation, since they are usually not available. This class will use
 * the velocity calculated the position at -kVelTDelta s and the position at +kVelTDelta s from a given position.
 *
 * @warning The results of the interpolations made by this class can contain any corrections (such as the refraction
 * correction or for the center of mass correction). It is important to keep good track of where each of the fixes
 * are applied at the development level.
 *
 * @todo Predictor from TLE data instead CPF using SGP4 propagator. It would be useful for space debris, although we
 * currently create CPFs from TLE for these cases, so development of this functionality is not a priority right now.
 * For the ILRS trackings, always use the CPF-based predictor (for its greater precision).
 */
class LIBDPSLR_EXPORT PredictorCPF : public PredictorSLR
{
public:

    /**
     * @brief Array of error strings used for the errors that the predictor can throw.
     *
     * This static constant contains an array of strings that represent the error messages for predictor errors.
     */
    static const std::array<std::string, 10> PredictorErrorStr;

    /**
     *  @enum PredictionError
     *  @brief This enum represents the different errors that can happen at interpolation.
     */
    enum class PredictionError : int
    {
        NO_ERROR,
        CPF_NOT_FOUND,
        CPF_HEADER_LOAD_FAILED,
        CPF_DATA_LOAD_FAILED,
        INTERPOLATION_NOT_IN_THE_MIDDLE,
        X_INTERPOLATED_OUT_OF_BOUNDS,
        INTERPOLATION_DATA_SIZE_MISMATCH,
        UNKNOWN_INTERPOLATOR,
        UNKNOWN_TROPO_MODEL,
        NO_POS_RECORDS,
        INVALID_INTERVAL,
        OTHER_ERROR
    };

    /**
     *  @enum InterpFunction
     *  @brief This enum represents the different interpolators that can be used.
     *  @todo Hermite interpolation function. Other future functions.
     */
    enum class InterpFunction
    {
        LAGRANGE_16,
        LAGRANGE_9,
        HERMITE
    };

    /**
     * @brief Constructs the interpolator with a opened CPF (all data) and the station location. CPF must be correctly opened with
     * all datas.
     * @param cpf CPF object for getting position records.
     * @param geod Geodetic ECEF position of the station (meters with mm preccision).
     * @param geoc Geocentric position of the station (radians, N > 0 and E > 0, altitude in m, 8 decimals for ~1 mm).
     */
    PredictorCPF(const CPF& cpf, const GeodeticPoint<Degrees>& geod, const GeocentricPoint& geoc);

    PredictorCPF(const GeodeticPoint<Degrees>& geod, const GeocentricPoint& geoc);

    PredictorCPF(const PredictorCPF&) = default;
    PredictorCPF(PredictorCPF&&) = default;
    PredictorCPF& operator=(const PredictorCPF&) = default;
    PredictorCPF& operator=(PredictorCPF&&) = default;

    /**
     * @brief Sets the CPF to use for predictions.
     * @param cpf, the CPF used for prediction,s
     * @return true if CPF is correct for predictions, false otherwise.
     */
    bool setCPF(const CPF& cpf);

    /**
     * @brief Gets the CPF used for predictions.
     * @return the CPF used for predictions. You should check if whether it is valid or not.
     */
    const CPF& getCPF() const;

    /**
     * @brief Set the interpolation function to use for predictions.
     * @param intp_funct, the interpolation function to use for predictions.
     */
    void setInterpFunction(InterpFunction intp_funct);

    /**
     * @brief Gets the current interpolation function used for predictions.
     * @return the current interpolation function used for predictions.
     */
    InterpFunction getInterpFunction() const;


    /**
     * @brief Checks if interpolator is ready. An interpolator is ready if it has positions for interpolating.
     * @return true if interpolator is ready, false otherwise.
     */
    bool isReady() const override;

    /**
     * @brief Interpolates position at requested instant.
     * @param mjdt, the modified julian datetime of the instant to be interpolated.
     * @param result, the result of the prediction.
     * @return The error code generated by the predictor.
     */
    int predict(const MJDateTime& mjdt, SLRPrediction& result) const override;

    /**
     * @brief Interpolates positions at requested time window.
     * @param mjdt_start, the start MJ datetime of the time window.
     * @param mjdt_end, the end MJ datetime of the time window.
     * @param step_ms, the step in ms from one interpolation to the next one.
     * @return A vector with all the results, or empty if it was impossible. To check errors produced at individual
     *         interpolations, check each result's error code.
     */
    SLRPredictionV predict(const MJDateTime& mjdt_start, const MJDateTime& mjdt_end, Milliseconds step) const override;

    /**
     * @brief If predictor is ready, returns the time window in which the predictor can be used. Otherwise,
     *        return time 0 at start and end.
     * @param start, MJ datetime of time window start.
     * @param end, MJ datetime of time window end.
     */
    void getTimeWindow(MJDateTime &start, MJDateTime &end) const override;

    /**
     * @brief Gets the asociated error message for a given error_code
     * @param error_code, the code whose error message will be returned.
     * @return the error message associated with error_code if it exists. Otherwise, an empty string.
     */
    std::string getErrorMsg(int error_code) const override;


private:
    
    void callInterp(const Seconds& x, Vector3D<Meters>& y, SLRPrediction& result) const;

    static PredictionError convertLagInterpError(stats::types::LagrangeError error);


    Matrix<long double> rotm_topo_local_;          // Topocentric local rotation matrix.
    std::vector<Seconds> pos_times_;               // Position times used at interpolation.
    Matrix<Meters> pos_data_;                      // Positions used at interpolation as matrix.
    InterpFunction interp_funct_;                  // The interpolator function used.
    CPF cpf_;                                      // The CPF ephemerids file used for interpolation.
};

}} // END NAMESPACES
// =====================================================================================================================
