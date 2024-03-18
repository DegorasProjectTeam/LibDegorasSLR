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
 * @file predictor_slr_base.h
 * @author Degoras Project Team
 * @brief This file contains the definition of the PredictorSLR class.
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include <memory>
#include <cstdint>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Geophysics/types/geodetic_point.h"
#include "LibDegorasSLR/Geophysics/types/geocentric_point.h"
#include "LibDegorasSLR/Geophysics/meteo.h"
#include "LibDegorasSLR/UtilitiesSLR/predictors/data/prediction_slr.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace slr{
namespace predictors{
// =====================================================================================================================

/**
 * @brief This class implements an interpolator for CPF positions.
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
class LIBDPSLR_EXPORT PredictorSlrBase
{
public:

    /**
     * @enum PredictionMode
     *
     * @brief This enum represents the different prediction modes.
     *
     * @warning The mode selection is very important, because it will affect the accuracy of the results and the
     *          execution time of the predictor, in such a way that it will affect both the calculation algorithm and
     *          real-time systems that make use of this class.
     */
    enum class PredictionMode
    {
        ONLY_INSTANT_RANGE,
        INSTANT_VECTOR,
        OUTBOUND_VECTOR,        ///< Uses the outbound vector to calculate an average distance at bounce time.
        INBOUND_VECTOR
    };

    /**
     *  @enum PredictionError
     *  @brief This enum represents the different errors that can happen at interpolation.
     */
    enum class PredictionSLRError : std::uint32_t
    {
        NO_ERROR = 0,
        INVALID_INTERVAL,
        OTHER_ERROR,
        END_BASE_ERRORS   = 10  ///< Sentinel value indicating the end of the base errors (invalid error).
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

    // Default copy and movement constructor and operators.
    M_DEFINE_CTOR_COPY_MOVE_OP_COPY_MOVE(PredictorSlrBase)
    /**
     * @brief Constructs the interpolator by getting the station location.
     * @param geod Geodetic position of the station (N and E > 0, 8 decimals for ~1 mm).
     * @param geoc Geocentric ECEF position of the station (with mm preccision).
     */
    PredictorSlrBase(const geo::types::GeodeticPointDeg& geod, const geo::types::GeocentricPoint& geoc);


    template <typename T, typename... Args>
    static std::shared_ptr<PredictorSlrBase> factory(Args&&... args)
    {
        return std::static_pointer_cast<PredictorSlrBase>(std::make_shared<T>(std::forward<Args>(args)...));
    }

    template <typename T>
    static std::shared_ptr<T> specialization(std::shared_ptr<PredictorSlrBase> base)
    {
        return std::static_pointer_cast<T>(base);
    }



    /**
     * @brief Get the station location of this cpf interpolator as a GeodeticPoint.
     */
    template <typename T>
    geo::types::GeodeticPoint<T> getGeodeticLocation() const
    {
        return this->stat_geodetic_.convertAngles<T>();
    }

    /**
     * @brief Get the station location of this cpf interpolator as a GeocentricPoint.
     */
    const geo::types::GeocentricPoint& getGeocentricLocation() const;

    /**
     * @brief Set the prediction mode that will be used
     * @param mode the prediction mode that will be used.
     */
    void setPredictionMode(PredictionMode mode);

    /**
     * @brief Gets the current prediction mode.
     * @return the current prediction mode.
     */
    PredictionMode getPredictionMode() const;

    /**
     * @brief Enables or disables all the corrections application.
     *
     * Enabling or disabling the corrections is independent of whether or not they can be applied at a certain time.
     * For example, even if the system is enabled, if the tropospheric parameters are not configured, the tropospheric
     * correction cannot be applied. To disable specific corrections, simply use the value 0 for those corrections or,
     * in the case of tropospheric, all the function PredictorCPF::unsetTropoCorrParams
     *
     * @param enable A boolean value indicating whether to enable or disable the corrections.
     */
    void enableCorrections(bool enable);

    /**
     * @brief Set the troposheric model used for troposheric correction calculation.
     * @param model, the troposheric model used for troposheric correction calculated.
     */
    void setTropoModel(TroposphericModel model);

    /**
     * @brief Set object eccentricity correction in meters.
     * @param correction in meters.
     */
    void setObjEccentricityCorr(math::units::Meters correction);

    /**
     * @brief Set calibration delay correction in picoseconds.
     * @param correction in picoseconds.
     */
    void setCaliDelayCorr(math::units::Picoseconds correction);

    /**
     * @brief Set systematic correction in meters.
     * @param correction in meters
     */
    void setSystematicCorr(math::units::Meters correction);

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
                            geo::meteo::WtrVapPressModel wvpm = geo::meteo::WtrVapPressModel::GIACOMO_DAVIS);

    /**
     * @brief Unsets the parameters for the tropospheric path delay correction.
     **/
    void unsetTropoCorrParams();

    /**
     * @brief Checks if interpolator is ready. An interpolator is ready if it has positions for interpolating.
     * @return true if interpolator is ready, false otherwise.
     */
    virtual bool isReady() const = 0;

    /**
     * @brief Checks if given time window is inside the valid prediction range.
     * @param start, the start MJ datetime of the window to be checked.
     * @param end, the end MJ datetime of the window to be checked.
     * @return true if the window is inside prediction range, false otherwise.
     */
    bool isInsideTimeWindow(const timing::types::MJDateTime& start, const timing::types::MJDateTime& end) const;

    /**
     * @brief Interpolates position at requested instant.
     * @param mjdt, the modified julian datetime of the instant to be interpolated.
     * @param result, the result of the prediction.
     * @return The error code generated by the predictor.
     */
    virtual int predict(const timing::types::MJDateTime& mjdt, PredictionSLR& result) const = 0;

    /**
     * @brief Interpolates positions at requested time window.
     * @param mjdt_start The start Modified Julian Datetime of the time window.
     * @param mjdt_end The end Modified Julian Datetime of the time window.
     * @param step The step in milliseconds from one interpolation to the next one.
     * @return A vector of `PredictionSLR` with all the prediction results, or empty if it was impossible. To
     * check errors produced at individual interpolations, check each independent result error code.
     */
    virtual PredictionSLRV predict(const timing::types::MJDateTime& mjdt_start,
                                   const timing::types::MJDateTime& mjdt_end,
                                   const math::units::Milliseconds& step) const;

    /**
     * @brief If predictor is ready, returns the time window in which the predictor can be used. Otherwise,
     *        return time 0 at start and end.
     * @param start, MJ datetime of time window start.
     * @param end, MJ datetime of time window end.
     */
    virtual void getTimeWindow(timing::types::MJDateTime& start, timing::types::MJDateTime& end) const = 0;

    /**
     * @brief Gets the asociated error message for a given error_code
     * @param error_code, the code whose error message will be returned.
     * @return the error message associated with error_code if it exists. Otherwise, an empty string.
     */
    virtual std::string getErrorMsg(int error_code) const = 0;

    virtual ~PredictorSlrBase();

protected:
    
    virtual math::units::Meters applyCorrections(math::units::Meters& range, PredictionSLR& result,
                                                 bool cali = false, math::units::Degrees el = 0) const;

    // Configuration variables.
    TroposphericModel tropo_model_;

    // Correction related parameters.
    math::units::Meters objc_ecc_corr_;       ///< Eccentricity correction at the satellite in meters (center of mass).
    math::units::Meters grnd_ecc_corr_;       ///< Eccentricity correction at the ground in meters (usually not used).
    math::units::Meters syst_rnd_corr_;       ///< Other systematic and random error corrections (in meters).
    math::units::Picoseconds cali_del_corr_;  ///< Station calibration delay correction (in picoseconds).
    bool apply_corr_;                         ///< Flag for apply the corrections.

    // Tropospheric parameters.
    // TODO Strong units
    long double press_;
    long double temp_;
    long double rel_hum_;
    long double wl_;
    geo::meteo::WtrVapPressModel wtrvap_model_;
    bool tropo_ready_;

private:

    // The prediction mode used.
    PredictionMode prediction_mode_;

    // Station position data.
    // Station latitude in radians (north > 0). 8 decimals preccision (1.1mm).
    // Station longitude in radians (east > 0). 8 decimals preccision (1.1mm).
    // Station altitude in metres
    geo::types::GeodeticPointRad stat_geodetic_;

    // Station geocentric point in metres.
    geo::types::GeocentricPoint stat_geocentric_;

};

/// Alias for PredictorSlrBase shared smart pointer.
using PredictorSlrPtr = std::shared_ptr<PredictorSlrBase>;

}}} // END NAMESPACES
// =====================================================================================================================
