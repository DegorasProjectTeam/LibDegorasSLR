/***********************************************************************************************************************
 *   LibDPSLR (Degoras Project SLR Library): A libre base library for SLR related developments.                        *                                      *
 *                                                                                                                     *
 *   Copyright (C) 2023 Degoras Project Team                                                                           *
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >                             *
 *                      < Jesús Relinque Madroñal >                                                                    *
 *                                                                                                                     *
 *   This file is part of LibDPSLR.                                                                                    *
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
 * @author Degoras Project Team.
 * @brief This file contains the definition of the PredictorSLR class.
 * @copyright EUPL License
 * @version 2306.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/FormatsILRS/cpf/cpf.h"
#include "LibDegorasSLR/Geo/common/geo_types.h"
#include "LibDegorasSLR/Mathematics/containers/matrix.h"
#include "LibDegorasSLR/Mathematics/containers/vector3d.h"
#include "LibDegorasSLR/Statistics/common/statistics_types.h"
#include "LibDegorasSLR/Geo/meteo.h"
#include "LibDegorasSLR/Timing/common/time_types.h"
#include "LibDegorasSLR/Timing/time_utils.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace algoslr{
namespace utils{
// =====================================================================================================================

// =====================================================================================================================
using dpslr::geo::common::GeocentricPoint;
using dpslr::geo::common::GeodeticPoint;
using dpslr::ilrs::cpf::CPF;
using dpslr::math::Matrix;
using dpslr::math::Vector3D;
using dpslr::geo::meteo::WtrVapPressModel;
using dpslr::timing::MJDate;
using dpslr::timing::SoD;
using dpslr::timing::MJDateTime;
using dpslr::timing::J2000;
// =====================================================================================================================

// CONSTANTS
// =====================================================================================================================
constexpr long double kVelTDelta = 0.5L;  /// Time delta (+-) for calculating the velocity vectors (seconds).
constexpr long double kTMargin = 1L;      /// Margin to apply to the instant sod to check the CPF data interval.
constexpr unsigned kPolLagDeg9 = 9U;      /// Fixed degree for the 9th polynomial lagrange interpolator.
constexpr unsigned kPolLagDeg16 = 16U;    /// Fixed degree for the 16th polynomial lagrange interpolator.
// =====================================================================================================================

/**
 * @brief This class implements an interpolator for CPF positions.
 *
 *
 * Para la interpolacion de las velocidades no se utilizan las del CPF, porque no es habitual que estén disponibles,
 *  se utiliza el la posición -0.5 y +0.5 en tiempo.
 *
 * @warning The results of the interpolations made by this class can contain any corrections (such as the refraction
 * correction or for the center of mass correction). it is important to keep good track of where each of the fixes
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
     * This struct contains the computed data when applying the PredictionMode::ONLY_INSTANT_RANGE mode. The distance
     * and flight time values may include corrections such as the eccentricity correction at the object and at the
     * ground, the signal delay (station calibration), and the systematic and random observation errors. If the
     * corrections are not included, the corresponding optional parameters will not be accessible in the higher level
     * structure (PredictorSLR::PredictionResult).
     *
     * @warning In this case, the tropospheric correction is never included.
     *
     * @see PredictionMode::ONLY_INSTANT_RANGE
     * @see PredictorSLR::PredictionResult
     */
    struct LIBDPSLR_EXPORT InstantRange
    {
        /**
         * @brief Default constructor.
         */
        InstantRange() = default;

        /**
         * @brief Default copy constructor.
         */
        InstantRange(const InstantRange&) = default;

        /**
         * @brief Default movement constructor.
         */
        InstantRange(InstantRange&&) = default;

        /**
         * @brief Default copy assingment operator.
         * @return Reference to itself.
         */
        InstantRange& operator=(const InstantRange&) = default;

        /**
         * @brief Default move assignment operator.
         * @return Reference to itself.
         */
        InstantRange& operator=(InstantRange&&) = default;

        // Datetime members.
        MJDate mjd;                  ///< Modified julian date in days.
        SoD sod;               ///< Second of day in that MJD (ps precission -> 12 decimals).
        MJDateTime mjdt;              ///< Modified julian datetime (day & fraction -> 12 decimals).

        // Range (1 way) and time of flight (2 way).
        long double range_1w;          ///< One way range in meters (mm precission -> 3 decimals).
        long double tof_2w;            ///< Two way flight time in seconds (ps precission -> 12 decimals).

        // Associated object geocentric vectors.
        Vector3D<long double> geo_pos; ///< Object geocentric interpolated positions in meters.

        /**
         * @brief Represents the InstantRange struct as a JSON-formatted string.
         * @return The JSON-formatted string representation of InstantRange.
         */
        std::string toJsonStr() const;
    };

    /**
     * This struct contains the computed data when applying the PredictionMode::INSTANT_VECTOR mode. The distance
     * and flight time values may include all types of corrections. If they are not included, the corresponding
     * optional parameters will not be accessible in the higher level structure (PredictorSLR::PredictionResult).
     *
     * @warning In this case, all the correction could be included.
     *
     * @see PredictionMode::INSTANT_VECTOR
     * @see PredictorSLR::PredictionResult
     */
    struct LIBDPSLR_EXPORT InstantData : public InstantRange
    {
        /**
         * @brief Default constructor.
         */
        InstantData() = default;

        /**
         * @brief Constructor.
         */
        InstantData(InstantRange);

        /**
         * @brief Default copy constructor.
         */
        InstantData(const InstantData&) = default;

        /**
         * @brief Default movement constructor.
         */
        InstantData(InstantData&&) = default;

        /**
         * @brief Default copy assingment operator.
         * @return Reference to itself.
         */
        InstantData& operator=(const InstantData&) = default;

        /**
         * @brief Default move assignment operator.
         * @return Reference to itself.
         */
        InstantData& operator=(InstantData&&) = default;

        // Associated object geocentric vectors.
        Vector3D<long double> geo_vel;   ///< Geocentric interpolated velocity in meters/second.

        // Azimuth and elevation for the instant vector.
        long double az;                       ///< Local computed azimuth in degrees (4 decimals).
        long double el;                       ///< Local computed elevation in degrees (4 decimals).

        /**
         * @brief Represents the InstantData struct as a JSON-formatted string.
         * @return The JSON-formatted string representation of InstantData.
         */
        std::string toJsonStr() const;
    };

    /**
     * This struct contains the computed data when applying the PredictionMode::OUTBOUND_VECTOR mode. The distance
     * and flight time values may include all types of corrections. If they are not included, the corresponding
     * optional parameters will not be accessible in the higher level structure (PredictorSLR::PredictionResult).
     *
     * @warning In this case, all the correction could be included.
     *
     * @see PredictionMode::OUTBOUND_VECTOR
     * @see PredictorSLR::PredictionResult
     */
    struct LIBDPSLR_EXPORT OutboundData : public InstantData
    {
        /**
         * @brief Default constructor.
         */
        OutboundData() = default;
    };

    /**
     * This struct contains the computed data when applying the PredictionMode::INBOUND_VECTOR mode. The distance
     * and flight time values may include all types of corrections. If they are not included, the corresponding
     * optional parameters will not be accessible in the higher level structure (PredictorSLR::PredictionResult).
     *
     * @warning In this case, all the correction could be included.
     *
     * @see PredictionMode::INBOUND_VECTOR
     * @see PredictorSLR::PredictionResult
     */
    struct LIBDPSLR_EXPORT InboundData
    {
        /**
         * @brief Default constructor.
         */
        InboundData() = default;

        /**
         * @brief Default copy constructor.
         */
        InboundData(const InboundData&) = default;

        /**
         * @brief Default movement constructor.
         */
        InboundData(InboundData&&) = default;

        /**
         * @brief Default copy assingment operator.
         * @return Reference to itself.
         */
        InboundData& operator=(const InboundData&) = default;

        /**
         * @brief Default move assignment operator.
         * @return Reference to itself.
         */
        InboundData& operator=(InboundData&&) = default;

        // Datetime members.
        MJDate mjd;              ///< Modified julian date in days.
        SoD sod;                 ///< Second of day in that MJD (ps precission -> 12 decimals).
        MJDateTime mjdt;         ///< Modified julian datetime (day & fraction -> 12 decimals).

        // Range (1 way) and time of flight (2 way).
        long double range_1w;          ///< One way range in meters (mm precission -> 3 decimals).
        long double tof_2w;            ///< Two way flight time in seconds (ps precission -> 12 decimals).

        /**
         * @brief Represents the InboundData struct as a JSON-formatted string.
         * @return The JSON-formatted string representation of InboundData.
         */
        std::string toJsonStr() const;
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
        Optional<double> diff_az;     ///< Azimuth difference between the outbound and instant vectors (4 decimals).
        Optional<double> diff_el;     ///< Elevation difference between the outbound and instant vectors (4 decimals).

        // Corrections applied.
        Optional<double> objc_ecc_corr;    ///< Eccentricity correction at the satellite in meters (usually CoM).
        Optional<double> grnd_ecc_corr;    ///< Eccentricity correction at the ground in meters (usually not used).
        Optional<double> cali_del_corr;    ///< Station calibration delay correction in picoseconds (2 way).
        Optional<double> corr_tropo;       ///< Tropospheric path delay correction in meters (1 way).
        Optional<double> syst_rnd_corr;    ///< Other systematic and random error corrections.

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
    PredictorSLR(const CPF& cpf, const GeodeticPoint<long double>& geod, const GeocentricPoint<long double>& geoc);

    PredictorSLR(const GeodeticPoint<long double>& geod, const GeocentricPoint<long double>& geoc);

    bool setCPF(const CPF& cpf);

    /**
     * @brief Get the station location of this cpf interpolator as a GeodeticPoint.
     */
    const GeodeticPoint<long double>& getGeodeticLocation() const;

    /**
     * @brief Get the station location of this cpf interpolator as a GeocentricPoint.
     */
    GeocentricPoint<long double> getGeocentricLocation() const;

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
    void setObjEccentricityCorr(long double correction);

    void setCaliDelayCorr(long double correction);

    void setSystematicCorr(long double correction);

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

    /**
     * @brief Interpolates position at requested instant.
     * @param mjdt, the modified julian datetime in seconds, with decimals, of the instant to be interpolated.
     * @param[out] result the result of the predictor.
     * @return The error code generated by the predictor.
     */
    PredictionError predict(MJDateTime mjdt, SLRPrediction& result) const;


    // En este caso se debe comprobar los resultados mirando el vector. Estará vacío en caso de error critico.
    // Explicar la computacion paralela.
    SLRPredictions predict(MJDate mjd_start, SoD sod_start,
                              MJDate mjd_end, SoD sod_end,
                              unsigned step_ms) const
    {
        // Container and auxiliar.
        std::vector<std::pair<MJDate, SoD>> interp_times;
        MJDate mjd_current = mjd_start;
        SoD sod_current = sod_start;
        long double step_sec = step_ms/1000.0L;

        // Check interval.
        if(!this->isReady() || !isInsideTimeWindow(mjd_start, sod_start, mjd_end, sod_end))
            return PredictorSLR::SLRPredictions();

        // Calculates all the interpolation time.
        // Parallel calculation of all times.


        // Calculates all the interpolation times.
        while(mjd_current < mjd_end ||sod_current <= sod_end)
        {
            interp_times.push_back({mjd_current, sod_current});
            sod_current += step_sec;
            if (sod_current >= 86400.0L)
            {
                mjd_current++;
                sod_current -= 86400.0L;
            }
        }

        // Results container.
        PredictorSLR::SLRPredictions results(interp_times.size());

        // TODO QUITAR
        // Configure OMP.
        omp_set_num_threads(omp_get_max_threads());

        // Parallel calculation.
        #pragma omp parallel for
        for(size_t i = 0; i<interp_times.size(); i++)
        {
            this->predict(interp_times[i].first, interp_times[i].second, results[i]);
        }

        // Return the container.
        return results;
    }



    /**
     * @brief If predictor is ready, returns the time window in which the predictor can be used.
     * @param mjd_start, Modified Julian Date in days of time window start.
     * @param sod_start, Second of day in seconds of time window start.
     * @param mjd_end, Modified Julian Date in days of time window end.
     * @param sod_end, Second of day in seconds of time window end.
     */
    void getTimeWindow(MJDate &mjd_start, SoD &sod_start, MJDate &mjd_end, SoD &sod_end) const;





private:
    
    long double applyCorrections(long double& range, SLRPrediction& result, bool cali = false, long double el = 0) const;

    PredictionError callToInterpol(long double x, Vector3D<long double>& y, SLRPrediction& result) const;

    static PredictionError convertLagInterpError(stats::common::LagrangeError error);

    // Configuration variables.
    InterpolFunction interpol_function_;
    TroposphericModel tropo_model_;
    PredictionMode prediction_mode_;

    // Correction related parameters.
    long double objc_ecc_corr_;       ///< Eccentricity correction at the satellite in meters (usually center of mass).
    long double grnd_ecc_corr_;       ///< Eccentricity correction at the ground in meters (usually not used).
    long double cali_del_corr_;       ///< Station calibration delay correction (in picoseconds).
    long double syst_rnd_corr_;       ///< Other systematic and random error corrections (in meters).
    bool apply_corr_;                 ///< Flag for apply the corrections.

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
    Vector3D<long double> stat_geocentric_;


    // Topocentric local rotation matrix.
    math::Matrix<long double> rotm_topo_local_;

    std::vector<long double> pos_times_;               // Position data used at interpolation.

    Matrix<long double> pos_data_;


    CPF cpf_;
};

}}} // END NAMESPACES
// =====================================================================================================================
