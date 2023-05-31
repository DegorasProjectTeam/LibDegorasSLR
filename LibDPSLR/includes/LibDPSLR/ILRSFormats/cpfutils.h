/***********************************************************************************************************************
 * Copyright 2023 Degoras Project Team
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they will be approved by the
 * European Commission - subsequent versions of the EUPL (the "Licence");
 *
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the Licence is distributed on
 * an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the Licence for the
 * specific language governing permissions and limitations under the Licence.
 **********************************************************************************************************************/

/** ********************************************************************************************************************
 * @file cpfutils.h
 *
 * @brief This file contains utilities for CPF.
 *
 * @author    Degoras Project Team.
 * @version   2202.1
 * @date      03-02-2022
 * @copyright EUPL License.
 *
 **********************************************************************************************************************/

#pragma once

// ========== DPSLR INCLUDES ===========================================================================================
#include "libdpslr_global.h"
#include "class_cpf.h"
#include "geo.h"
// =====================================================================================================================


// ========== DPSLR NAMESPACES =========================================================================================
namespace dpslr{
namespace cpfutils{
// =====================================================================================================================

/**
 * @brief This class implements an interpolator for CPF positions.
 */
class LIBDPSLR_EXPORT CPFInterpolator
{
public:


    /// @enum ErrorEnum
    /// This enum represents the different errors that can happen at interpolation.
    enum InterpolationError
    {
        NOT_ERROR,
        CPF_NOT_FOUND,
        CPF_HEADER_LOAD_FAILED,
        CPF_DATA_LOAD_FAILED,
        INTERPOLATION_NOT_IN_THE_MIDDLE,
        X_INTERPOLATED_OUT_OF_BOUNDS,
        INTERPOLATION_DATA_SIZE_MISMATCH,
        UNKNOWN_INTERPOLATOR,
        NO_POS_RECORDS,
        OTHER_ERROR
    };

    /// @enum InterpolationMode
    /// This enum represents the interpolation modes.
    enum InterpolationMode
    {
        INSTANT_VECTOR = 0,
        AVERAGE_DISTANCE = 1
    };

    /// @enum InterpolationFunction
    /// This enum represents the different interpolators that can be used.
    enum InterpolationFunction
    {
        LAGRANGE_9 = 0,
    };

    // Map for getting error strings.
    static const std::array<std::string, 10> ErrorEnumStrings;

    /**
     * @brief The InterpolationResult struct has the data returned by the interpolation.
     */
    struct LIBDPSLR_EXPORT InterpolationResult
    {
        // TODO: Cambiar el geocentric interpolated a GeocentricPoint.
        int mjd;                 ///< Interpolation modified julian date in days.
        long double sec_of_day;  ///< Interpolation second of day in that MJD (ps precission -> 12 decimals).
        long double mjdt;        ///< Interpolation modified julian datetime (day and fraction -> 12 decimals).
        long double range;       ///< One way range in meters (mm precission -> 3 decimals).
        long double tof_2w;      ///< Two way flight time in seconds (ps precission -> 12 decimals).
        double azimuth;          ///< Azimuth in degrees (4 decimals).
        double elevation;                 ///< Elevation in degrees (4 decimals).
        double diff_azimuth;              ///< Receive-transmit azi at transmit time in degrees (4 decimals).
        double diff_elevation;            ///< Receive-transmit ele at transmit time in degrees (4 decimals).
        std::array<double, 3> geocentric; ///< Geocentric interpolated positions in meters.
        InterpolationError error;             ///< Error that may have occurred.

        std::string toJson() const;
    };

    using InterpolationVector = std::vector<InterpolationResult>;

    /**
     * @brief CPFInterpolator constructs the interpolator by getting the data from CPF and the station location and
     * leaving it ready for interpolating positions. CPF must be correctly opened and contain position records.
     * @param cpf for getting position records and center of mass correction.
     * @param stat_geodetic the geodetic position of the station.
     * @param stat_geocentric the geocentric position of the station.
     */
    CPFInterpolator(const CPF& cpf, const dpslr::geo::frames::GeodeticPoint<long double> &stat_geodetic,
                    const dpslr::geo::frames::GeocentricPoint<long double> &stat_geocentric);

    // Interpolate methods.
    /**
     * @brief Interpolates position at requested instant.
     * @param mjd, the modified julian date (in days) of the instant to be interpolated.
     * @param second the, second of day, with decimals, of the instant to be interpolated.
     * @param interp_res, the result of the interpolation.
     * @param mode, the selected interpolation mode.
     * @param function, the selected interpolator.
     * @return the error code generated by the interpolation.
     */
    InterpolationError interpolate(int mjd, long double second, InterpolationResult &interp_res,
                          InterpolationMode mode = InterpolationMode::AVERAGE_DISTANCE,
                          InterpolationFunction function = InterpolationFunction::LAGRANGE_9) const;

    /**
     * @brief Interpolates position at requested instant.
     * @param mjt, the modified julian datetime in seconds, with decimals, of the instant to be interpolated.
     * @param interp_res, the result of the interpolation.
     * @param mode, the selected interpolation mode.
     * @param function, the selected interpolator.
     * @return the error code generated by the interpolation.
     */
    InterpolationError interpolate(long double mjt, InterpolationResult &interp_res,
                          InterpolationMode mode = InterpolationMode::AVERAGE_DISTANCE,
                          InterpolationFunction function = InterpolationFunction::LAGRANGE_9) const;

    /**
     * @brief Get the station location of this cpf interpolator.
     * @param geodetic, the geodetic position of the station.
     * @param geocentric, the geocentric position of the station.
     */
    void getStationLocation(dpslr::geo::frames::GeodeticPoint<long double>& geodetic,
                            dpslr::geo::frames::GeocentricPoint<long double>& geocentric) const;

    /**
     * @brief Checks if interpolator is empty. An interpolator is empty if it does not have positions for interpolating.
     * @return true if interpolator is empty, false otherwise.
     */
    bool empty() const;

    /**
     * @brief Checks if interpolator is ready. An interpolator is ready if it has positions for interpolating.
     * @return true if interpolator is ready, false otherwise.
     */
    bool ready() const;

    /**
     * @brief Gets the available time window for interpolation. If interpolator is empty, all values are set to 0.
     * @param mjd_start the modified julian date of interval start.
     * @param fract_start the fraction of day of interval start.
     * @param mjd_end the modified julian date of interval end.
     * @param fract_end the fraction of day of interval end.
     */
    void getAvailableTimeInterval(int &mjd_start, long double &fract_start, int &mjd_end, long double &fract_end) const;

private:

    InterpolationError convertInterpError(dpslr::math::LagrangeResult error) const;

    // Station position data.
    // Station latitude in radians (north > 0). 8 decimals preccision (1.1mm).
    // Station longitude in radians (east > 0). 8 decimals preccision (1.1mm).
    // Station altitude in metres
    dpslr::geo::frames::GeodeticPoint<long double> stat_geodetic;
    // Station geocentric in metres
    dpslr::geo::frames::GeocentricPoint<long double> stat_geocentric;
    // Rotation matrix.
    dpslr::math::Matrix<long double> rotation_matrix;
    // Position data used at interpolation. Loaded when data is loaded.
    std::vector<long double> position_times;
    dpslr::math::Matrix<long double> position_data;

    dpslr::common::optional<double> com_offset;

    int mjd_orig;
    long double sod_orig;
    int mjd_end;
    long double sod_end;
};

/**
 * @brief The Pass struct contains data about a space object pass
 */
struct LIBDPSLR_EXPORT Pass
{
    struct LIBDPSLR_EXPORT Step
    {
        int mjd;                 ///< Modified julian date of step.
        long double fract_day;   ///< Fraction of day in seconds.
        double azim;             ///< Azimuth of the step in degrees.
        double elev;             ///< Elevation of the step in degrees.
        double azim_rate;        ///< Azimuth rate of step in deg/s
        double elev_rate;        ///< Elevation rate of step in deg/s
    };

    long double interval;        ///< Interval between two steps in seconds
    unsigned int min_elev;       ///< Minimum elevation for pass.
    std::vector<Step> steps;     ///< Steps of the pass
};

/**
 * @brief This class implements a wrapper that calculates passes from a CPF
 */
class LIBDPSLR_EXPORT PassCalculator
{
public:

    enum ResultCodes
    {
        NOT_ERROR,
        CPF_NOT_VALID,
        INTERVAL_OUTSIDE_OF_CPF,
        OTHER_ERROR
    };

    /**
     * @brief PassCalculator constructs the pass calculator by getting the data from CPF and the station location and
     * leaving it ready for calculating the passes of the CPF. CPF must be correctly opened and contain position records.
     * @param cpf for getting position records and center of mass correction.
     * @param stat_geodetic the geodetic position of the station.
     * @param stat_geocentric the geocentric position of the station.
     * @param min_elev minimum elevation of the pass in degrees. By default is 0, i.e., above horizon.
     * @param interval interval between two steps of the pass in seconds. By default is 1 second.
     */
    PassCalculator(const CPF& cpf, const dpslr::geo::frames::GeodeticPoint<long double> &stat_geodetic,
                   const dpslr::geo::frames::GeocentricPoint<long double> &stat_geocentric, unsigned int min_elev = 0,
                   long double interval = 1.L);

    /**
     * @brief PassCalculator constructs the pass calculator by getting the data from CPF and the station location and
     * leaving it ready for calculating the passes of the CPF. The cpf_path must point to a valid CPF file.
     * @param cpf_path of the cpf file for getting position records and center of mass correction.
     * @param stat_geodetic the geodetic position of the station.
     * @param stat_geocentric the geocentric position of the station.
     * @param min_elev minimum elevation of the pass in degrees. By default is 0, i.e., above horizon.
     * @param interval interval between two steps of the pass in seconds. By default is 1 second.
     */
    PassCalculator(const std::string& cpf_path, const dpslr::geo::frames::GeodeticPoint<long double> &stat_geodetic,
                   const dpslr::geo::frames::GeocentricPoint<long double> &stat_geocentric, unsigned int min_elev = 0,
                   long double interval = 1.L);

    /**
     * @brief Setter for minimum elevation.
     * @param min_elev the minimum elevation in degrees.
     */
    void setMinElev(unsigned int min_elev);
    /**
     * @brief Getter for minimum elevation.
     * @return the minimum elevation in degrees.
     */
    unsigned int minElev() const;
    /**
     * @brief Setter for interval.
     * @param interval the interval for interpolation in seconds.
     */
    void setInterval(long double interval);
    /**
     * @brief Getter for interval.
     * @return the interval for interpolation in seconds.
     */
    long double interval() const;

    /**
     * @brief Get passes within the given interval of time.
     * @param mjd_start the modified julian date of interval start.
     * @param fract_day_start the fraction of day in seconds of interval start.
     * @param mjd_end the modified julian date of interval end.
     * @param fract_day_end the fraction of day in seconds of interval end.
     * @param passes the returned passes.
     * @return The result of the operation.
     */
    ResultCodes getPasses(int mjd_start, long double fract_day_start,
                          int mjd_end, long double fract_day_end, std::vector<Pass> &passes) const;


private:
    unsigned int min_elev_;
    long double interval_;
    CPFInterpolator interpolator_;
};

}
} // END NAMESPACES
// =====================================================================================================================
