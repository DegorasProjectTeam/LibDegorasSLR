/***********************************************************************************************************************
 *   LibDPSLR (Degoras Project SLR Library): A libre base library for SLR related developments.                        *
 *                                                                                                                     *
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
 * @file tracking_slr.h
 * @author Degoras Project Team.
 * @brief This file contains the definition of the TrackingSLR class.
 * @copyright EUPL License
 * @version 2306.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdpslr_global.h"
#include "LibDegorasSLR/AlgorithmsSLR/utils/predictor_slr.h"
#include "LibDegorasSLR/Astronomical/predictor_sun.h"
// =====================================================================================================================


// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace algoslr{
namespace utils{
// =====================================================================================================================

/**
 * @brief The TrackingSLR class implements an abstraction for a SLR tracking.
 *
 * This class uses a @a PredictorSLR to look for a SLR tracking next to a given datetime. This class also offers a sun
 * avoidance algorithm. This algorithm changes the tracking trajectory whenever it passes through a circular sector
 * around the sun (the sun security sector).
 *
 */
class LIBDPSLR_EXPORT TrackingSLR
{
public:

    /**
     * @brief The SunSector struct contains data of a sector where this tracking passes through the sun security sector.
     */
    struct SunSector
    {
        long double az_entry;       ///< Azimuth of sun sector entry point
        long double el_entry;       ///< Elevation of sun sector entry point
        long double mjdt_entry;     ///< MJ datetime of sun sector entry point
        long double az_exit;        ///< Azimuth of sun sector exit point
        long double el_exit;        ///< Elevation of sun sector exit point
        long double mjdt_exit;      ///< MJ datetime of sun sector exit point
        bool cw;                    ///< Rotation direction of the sun avoidance manoeuvre
    };

    /**
     * @brief This struct contanis the azimuth and elevation for a given time of the tracking.
     */
    struct Position
    {
        long double az;             ///< Azimuth of the position in degrees.
        long double el;             ///< Elevation of the position in degrees.
        long double mjdt;           ///< MJ datetime of the position in days.
    };

    /**
     * @brief The PositionResult enum represents the possible result of calling @a getPosition function.
     */
    enum PositionResult
    {
        NOT_ERROR,                  ///< Position is correct, there was no error.
        AVOIDING_SUN,               ///< Position is correct, but is avoiding sun security sector.
        CANT_AVOID_SUN,             ///< Position is NOT correct, since it cannot avoid sun security sector.
        PREDICTION_ERROR            ///< Position is NOT correct, there was a prediction error.
    };

    /**
     * @brief TrackingSLR constructor. Receives the necessary parameters for looking for a SLR tracking.
     * @param min_elev, the minimum elevation in degrees at which the tracking starts.
     * @param mjd_start, the MJ date in days to start looking for a tracking.
     * @param sod_start, the second of day to start looking for a tracking.
     * @param predictor, the predictor used for calculating the tracked object positions at a given time.
     * @param avoid_sun (optional), true if you want the sun avoidance to be applied, false otherwise.
     * @param sun_avoid_angle (optional), if sun avoidance is applied, the radius of the sun security sector in degrees.
     */
    LIBDPSLR_EXPORT TrackingSLR(long double min_elev, unsigned int mjd_start, long double sod_start, PredictorSLR&& predictor,
                                 bool avoid_sun = true, long double sun_avoid_angle = 15.L);

    /**
     * @brief This function checks if there is a valid SLR tracking. You MUST check this, before requesting positions.
     * @return true if there is a valid tracking, false otherwise.
     */
    LIBDPSLR_EXPORT bool isValid() const;
    /**
     * @brief This function returns the minimum elevation of this tracking in degrees.
     * @return the minimum elevation of the tracking in degrees.
     */
    LIBDPSLR_EXPORT long double getMinElev() const;
    /**
     * @brief If this traking is valid, you can get the tracking start with this function.
     * @param mjd, the MJ date in days for the tracking start.
     * @param sod, the second of day for the tracking start.
     */
    LIBDPSLR_EXPORT void getTrackingStart(unsigned int &mjd, long double& sod) const;
    /**
     * @brief If this tracking is valid, you can get the tracking end with this function.
     * @param mjd, the MJ date in days for the tracking end.
     * @param sod, the second of day for the tracking end.
     */
    LIBDPSLR_EXPORT void getTrackingEnd(unsigned int &mjd, long double& sod) const;
    /**
     * @brief This function returns if sun avoidance is applied.
     * @return true if sun avoidance is applied, false otherwise.
     */
    LIBDPSLR_EXPORT bool getSunAvoidApplied() const;

    /**
     * @brief This function returns if there is sun overlapping in this tracking.
     * @return true if sun avoid is applied and there is an overlapping with the sun, false otherwise.
     */
    LIBDPSLR_EXPORT bool getSunOverlapping() const;

    /**
     * @brief This function returns the radius of the sun security sector applied to sun avoidance manouvre.
     *        This function should not be called if sun avoidance is not applied.
     * @return the radius of the sun security sector
     */
    LIBDPSLR_EXPORT long double getSunAvoidAngle() const;

    /**
     * @brief This function returns the object's position at a given time.
     * @param mjd, the MJD in days.
     * @param sod, the second of day in seconds.
     * @param pos, the returned position data.
     * @return the result of the operation. Must be checked to ensure the position is valid.
     */
    LIBDPSLR_EXPORT PositionResult getPosition(unsigned int mjd, long double sod, Position &pos);

private:

    void analyzeTracking(unsigned int mjd_start, long double sod_start);
    bool findTrackingStart(unsigned int mjd_start, long double sod_start);
    bool findTrackingEnd();

    bool insideSunSector(const PredictorSLR::InstantData& pos,
                         const dpslr::astro::SunPosition<long double>& sun_pos) const;
    void setSunSectorRotationDirection(
        SunSector &sector, const std::vector<dpslr::astro::SunPosition<long double>> &sun_positions);

    long double min_elev_;

    unsigned int mjd_start_;
    long double sod_start_;
    unsigned int mjd_end_;
    long double sod_end_;

    bool valid_pass_;
    bool avoid_sun_;
    long double sun_avoid_angle_;
    std::vector<SunSector> sun_sectors_;

    dpslr::algoslr::utils::PredictorSLR predictor_;
    dpslr::astro::PredictorSun<long double> sun_predictor_;


};

}}} // END NAMESPACES
// =====================================================================================================================
