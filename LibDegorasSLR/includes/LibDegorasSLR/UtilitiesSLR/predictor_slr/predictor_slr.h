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
 * @file predictor_slr.h
 * @author Degoras Project Team
 * @brief This file contains the definition of the PredictorSLR class.
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/FormatsILRS/cpf/cpf.h"
#include "LibDegorasSLR/Geophysics/types/geodetic_point.h"
#include "LibDegorasSLR/Geophysics/types/geocentric_point.h"
#include "LibDegorasSLR/Mathematics/types/matrix.h"
#include "LibDegorasSLR/Mathematics/types/vector3d.h"
#include "LibDegorasSLR/Statistics/types/statistics_types.h"
#include "LibDegorasSLR/Geophysics/meteo.h"
#include "LibDegorasSLR/Timing/types/time_types.h"
#include "LibDegorasSLR/Timing/time_utils.h"
#include "LibDegorasSLR/UtilitiesSLR/predictor_slr/prediction_data_slr.h"
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
using geo::meteo::WtrVapPressModel;
using timing::MJDate;
using timing::SoD;
using timing::MJDateTime;
// ---------------------------------------------------------------------------------------------------------------------

// CONSTANTS
// =====================================================================================================================
constexpr long double kVelTDelta = 0.5L;  ///< Time delta (+-) for calculating the velocity vectors (seconds).
constexpr long double kTMargin = 1L;      ///< Margin to apply to the instant sod to check the CPF data interval.
constexpr unsigned kPolLagDeg9 = 9U;      ///< Fixed degree for the 9th polynomial lagrange interpolator.
constexpr unsigned kPolLagDeg16 = 16U;    ///< Fixed degree for the 16th polynomial lagrange interpolator.
// =====================================================================================================================

/**
 * @brief This class implements an interpolator for CPF positions.
 *
 *
 * Para la interpolacion de las velocidades no se utilizan las del CPF, porque no es habitual que estén disponibles,
 *  se utiliza el la posición -0.5 y +0.5 en tiempo.
 *
 * @warning The results of the interpolations made by this class can contain any corrections (such as the refraction
 * correction or for the center of mass correction). It is important to keep good track of where each of the fixes
 * are applied at the development level.
 *
 * @todo Predictor from TLE data instead CPF using SGP4 propagator. It would be useful for space debris, although we
 * currently create CPFs from TLE for these cases, so development of this functionality is not a priority right now.
 * For the ILRS trackings, always use the CPF-based predictor (for its greater precision).
 */
class LIBDPSLR_EXPORT PredictorSLR
{
public:

    /**
     * @brief Array of error strings used for the errors that the predictor can throw.
     *
     * This static constant contains an array of strings that represent the error messages for predictor errors.
     */
    static const std::array<std::string, 10> PredictorErrorStr;

    /** @enum PredictionError
     *  @brief This enum represents the different errors that can happen at interpolation.
     */
    enum class PredictionError
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
     * @enum PredictionMode
     *
     * @brief This enum represents the different prediction modes.
     *
     * This enumeration represents the different modes to perform the range and position predictions. For more
     * information, read the documentation of the class (PredictorSLR).
     *
     * @warning The mode selection is very important, because it will affect the accuracy of the results and the
     *          execution time of the predictor, in such a way that it will affect both the calculation algorithm and
     *          real-time systems that make use of this class.
     */
    enum class PredictionMode
    {
        ONLY_INSTANT_RANGE,
        INSTANT_VECTOR,
        OUTBOUND_VECTOR,   ///< Uses the outbound vector to calculate an average distance at bounce time.
        INBOUND_VECTOR
    };

    /** @enum InterpolFunction
     *  @brief This enum represents the different interpolators that can be used.
     *  @todo Hermite interpolation function.
     */
    enum class InterpolFunction
    {
        LAGRANGE_16,
        LAGRANGE_9,
        HERMITE
    };

    /** @enum TroposphericModel
     *  @brief This enum represents the different tropospheric models that can be used.
     *  @todo Mendes-Pavlis tropospheric model (IERS Conventions 2010, IERS Technical Note No. 36).
     */
    enum class TroposphericModel
    {
        MARINI_MURRAY,
        MENDES_PAVLIS
    };


    /**
     * This container has all the data returned by the predictor. The InstantRange always will be disponible. The
     * rest of the data will be available or not depending on the selected computing mode. The azimuth and elevation
     * difference between receive and transmit direction at instant time parameters will only be available in
     * the PredictionMode::OUTBOUND_VECTOR and PredictionMode::INBOUND_VECTOR modes. You can check the corrections
     * applied by accessing the corresponding parameters.
     *
     * @see PredictorSLR::PredictionMode
     */
    struct LIBDPSLR_EXPORT SLRPrediction
    {
        // Constructors.
        SLRPrediction() = default;
        SLRPrediction(const SLRPrediction&) = default;
        SLRPrediction(SLRPrediction&&) = default;

        // Operators.
        SLRPrediction& operator=(const SLRPrediction&) = default;
        SLRPrediction& operator=(SLRPrediction&&) = default;

        // Result containers for the different modes.
        InstantRange instant_range;           ///< Result range for the instant time in the ONLY_INSTANT_RANGE mode.
        Optional<InstantData> instant_data;   ///< Result data for the instant time (instant vectors).
        Optional<OutboundData> outbound_data; ///< Result data for the bounce time (outbound vectors).
        Optional<InboundData> inbound_data;   ///< Result data for the arrival time (inbound vector).

        // Difference between receive and transmit direction at instant time.
        Optional<Degrees> diff_az;   ///< Azimuth difference between outbound and instant vectors (4 decimals).
        Optional<Degrees> diff_el;   ///< Elevation difference between outbound and instant vectors (4 decimals).

        // Corrections applied.
        Optional<Picoseconds> cali_del_corr; ///< Station calibration delay correction (picoseconds, 2 way).
        Optional<Meters> objc_ecc_corr;      ///< Eccentricity correction at the object (meters, 1 way, usually CoM).
        Optional<Meters> grnd_ecc_corr;      ///< Eccentricity correction at the ground (meters, usually not used).
        Optional<Meters> corr_tropo;         ///< Tropospheric path delay correction (meters, 1 way).
        Optional<Meters> syst_rnd_corr;      ///< Other systematic and random error corrections (meters, 1 way).

        // Error code.
        PredictionError error;  ///< Error that may have occurred.

        /**
         * @brief Represents all the prediction result as a JSON formated string.
         */
        std::string toJsonStr() const;
    };

    /// Alias for SLRPrediction vector.
    using SLRPredictions = std::vector<SLRPrediction>;

    /**
     * @brief Constructs the interpolator by getting CPF and the station location. CPF must be correctly opened.
     * @param cpf CPF object for getting position records.
     * @param geod Geodetic ECEF position of the station (meters with mm preccision).
     * @param geoc Geocentric position of the station (radians, N > 0 and E > 0, altitude in m, 8 decimals for ~1 mm).
     */
    PredictorSLR(const CPF& cpf, const GeodeticPoint<long double>& geod, const GeocentricPoint& geoc);

    PredictorSLR(const GeodeticPoint<long double>& geod, const GeocentricPoint& geoc);

    bool setCPF(const CPF& cpf);

    /**
     * @brief Get the station location of this cpf interpolator as a GeodeticPoint.
     */
    const GeodeticPoint<long double>& getGeodeticLocation() const;

    /**
     * @brief Get the station location of this cpf interpolator as a GeocentricPoint.
     */
    GeocentricPoint getGeocentricLocation() const;

    const CPF& getCPF() const;

    /**
     * @brief Enables or disables all the corrections application.
     *
     * Enabling or disabling the corrections is independent of whether or not they can be applied at a certain time.
     * For example, even if the system is enabled, if the tropospheric parameters are not configured, the tropospheric
     * correction cannot be applied. To disable specific corrections, simply use the value 0 for those corrections or,
     * in the case of tropospheric, all the function PredictorSLR::unsetTropoCorrParams
     *
     * @param enable A boolean value indicating whether to enable or disable the corrections.
     */
    void enableCorrections(bool enable);

    void setPredictionMode(PredictionMode mode);

    void setTropoModel(TroposphericModel model);

    void setInterpolFunction(InterpolFunction func);


    /// If this function is not called, the class will store the CoM correction of the CPF, if any.
    void setObjEccentricityCorr(Meters correction);

    void setCaliDelayCorr(Picoseconds correction);

    void setSystematicCorr(Meters correction);

    /**
     * @brief Sets the parameters for the tropospheric path delay correction.
     *
     * This function sets the parameters required for calculating tropospheric correction. The parameters include
     * pressure, temperature, relative humidity and wavelength. Additionally, a water vapor pressure model is
     * specified to determine the method for calculating the water vapor pressure.
     *
     * @param press The atmospheric pressure in millibars (mbar).
     * @param temp The temperature in Kelvin (K).
     * @param rh The relative humidity as a percentage (%).
     * @param wl The wavelength of the laser signal (micrometres).
     * @param wvpm The water vapor pressure model to use for calculations.
     *
     * @note If this parameters are not setted, the refraction correction will not be applied.
     * @note To disable the current refraction correction application, call to CPFPredictor::unsetTropoCorrParams().
     */
    void setTropoCorrParams(long double press, long double temp, long double rh, long double wl,
                            WtrVapPressModel wvpm = WtrVapPressModel::GIACOMO_DAVIS);

    /**
     * @brief Unsets the parameters for the tropospheric path delay correction.
     **/
    void unsetTropoCorrParams();

    /**
     * @brief Checks if interpolator is ready. An interpolator is ready if it has positions for interpolating.
     * @return true if interpolator is ready, false otherwise.
     */
    bool isReady() const;

    bool isInsideTimeWindow(MJDate mjd_start, SoD sod_start, MJDate mjd_end, SoD sod_end) const
    {
        // Check for valid time.
        if(!(mjd_start < mjd_end || (mjd_start == mjd_end && sod_start < sod_end)))
            return false;

        // Auxiliar.
        MJDate predict_mjd_start, predict_mjd_end;
        SoD predict_sod_start, predict_sod_end;

        // Get the time window.
        this->getTimeWindow(predict_mjd_start, predict_sod_start, predict_mjd_end, predict_sod_end);

        // Check if start time is inside predictor available time window
        return (timing::mjdInsideTimeWindow(mjd_start, sod_start, predict_mjd_start, predict_sod_start,
                                            predict_mjd_end, predict_sod_end));
    }

    /**
     * @brief Interpolates position at requested instant.
     * @param mjd, the modified julian date (in days) of the instant to be interpolated.
     * @param second the, second of day, with decimals, of the instant to be interpolated.
     * @param[out] result The result of the prediction.
     * @return The error code generated by the predictor.
     */
    PredictionError predict(MJDate mjd, SoD second, SLRPrediction& result) const;

    // En este caso se debe comprobar los resultados mirando el vector. Estará vacío en caso de error critico.
    // Explicar la computacion paralela.
    SLRPredictions predict(MJDate mjd_start, SoD sod_start, MJDate mjd_end, SoD sod_end, unsigned step_ms) const;



    /**
     * @brief If predictor is ready, returns the time window in which the predictor can be used.
     * @param mjd_start, Modified Julian Date in days of time window start.
     * @param sod_start, Second of day in seconds of time window start.
     * @param mjd_end, Modified Julian Date in days of time window end.
     * @param sod_end, Second of day in seconds of time window end.
     */
    void getTimeWindow(MJDate &mjd_start, SoD &sod_start, MJDate &mjd_end, SoD &sod_end) const;



private:
    
    Meters applyCorrections(Meters& range, SLRPrediction& result, bool cali = false, Degrees el = 0) const;

    PredictionError callToInterpol(const Seconds& x, Vector3D<Meters>& y, SLRPrediction& result) const;

    static PredictionError convertLagInterpError(stats::types::LagrangeError error);

    // Configuration variables.
    InterpolFunction interpol_function_;
    TroposphericModel tropo_model_;
    PredictionMode prediction_mode_;

    // Correction related parameters.
    Meters objc_ecc_corr_;            // Eccentricity correction at the satellite in meters (usually center of mass).
    Meters grnd_ecc_corr_;            // Eccentricity correction at the ground in meters (usually not used).
    Meters syst_rnd_corr_;            // Other systematic and random error corrections (in meters).
    Picoseconds cali_del_corr_;       // Station calibration delay correction (in picoseconds).
    bool apply_corr_;                 // Flag for apply the corrections.

    // Tropospheric parameters.
    long double press_;
    long double temp_;
    long double rel_hum_;
    long double wl_;
    WtrVapPressModel wtrvap_model_;
    bool tropo_ready_;

    // Station position data.
    // Station latitude in radians (north > 0). 8 decimals preccision (1.1mm).
    // Station longitude in radians (east > 0). 8 decimals preccision (1.1mm).
    // Station altitude in metres
    GeodeticPoint<long double> stat_geodetic_;

    // Station geocentric in metres
    GeocentricPoint stat_geocentric_;


    // Topocentric local rotation matrix.
    Matrix<long double> rotm_topo_local_;

    std::vector<Seconds> pos_times_;               // Position data used at interpolation.

    Matrix<Meters> pos_data_;

    CPF cpf_;
};

}} // END NAMESPACES
// =====================================================================================================================
