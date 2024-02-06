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
        MJDateTime mjdt_entry;        ///< MJ datetime of sun sector entry point
        long double az_exit;        ///< Azimuth of sun sector exit point
        long double el_exit;        ///< Elevation of sun sector exit point
        MJDateTime mjdt_exit;       ///< MJ datetime of sun sector exit point
        bool cw;                    ///< Rotation direction of the sun avoidance manoeuvre
    };

    /**
     * @brief The PositionStatus enum represents the possible situations of calling getPosition function.
     */
    enum PositionStatus
    {
        OUTSIDE_SUN,        ///< The final tracking position is outside the sun.
        INSIDE_SUN,         ///< The final tracking position is in the Sun and is configured for not avoiding.
        AVOIDING_SUN,       ///< The final tracking position is avoiding sun security sector.
        CANT_AVOID_SUN,     ///< Final position can't be calculated, since it cannot avoid sun security sector.
        OUT_OF_TRACK,       ///< The time provided for prediction is outside of tracking.
        PREDICTION_ERROR    ///< The object position can't be calculated, there was a SLR prediction error.
    };

    // TODO Explicar el por que puede ser diferente la real que la
    struct TrackingPosition
    {
        long double az;      ///< Azimuth of the space object in degrees.
        long double el;      ///< Elevation of the space object in degrees.
        long double diff_az; ///< Azimuth difference between the real prediction and tracking position.
        long double diff_el; ///< Elevation difference between the real prediction and tracking position.
    };

    /**
     * @brief This struct contanis the azimuth and elevation postion for a given time of the tracking.
     * TODO Explican more
     * Si OUT_OF_TRACK los optional no estan.
     * Si PREDICTION_ERROR el prediction result y el sun position están.
     * Si CANT_AVOID_SUN el prediction result y el sun position están.
     *
     */
    struct TrackingResult
    {
        // Datetime members.
        MJDate mjd;                   ///< Modified julian date in days.
        SoD sod;                   ///< Second of day in that MJD.
        MJDateTime mjdt;                 ///< Modified julian datetime (day & fraction).

        // Result members.
        Optional<PredictorSLR::PredictionResult> prediction_result; ///< SLR prediction result.
        Optional<TrackingPosition> tracking_position;               ///< Tracking position.
        Optional<astro::SunPosition<long double>> sun_pos;          ///< Sun position.

        // Status.
        PositionStatus status;  ///< The postion status.
    };

    /**
     * @brief TrackingSLR constructor. Receives the necessary parameters for looking for a SLR tracking.
     * @param min_elev, the minimum elevation in degrees at which the tracking starts.
     * @param mjd_start, the MJ date in days to start looking for a tracking.
     * @param sod_start, the second of day to start looking for a tracking.
     * @param predictor, the predictor used for calculating the tracked object positions at a given time.
     * @param time_delta, the time delta used to analyze the tracking. The smallest, the more precise.
     * @param avoid_sun (optional), true if you want the sun avoidance to be applied, false otherwise.
     * @param sun_avoid_angle (optional), if sun avoidance is applied, the radius of the sun security sector in degrees.
     */
    TrackingSLR(long double min_elev, MJDate mjd_start, SoD sod_start,
                PredictorSLR&& predictor, long double time_delta = 1.L,
                bool avoid_sun = true, long double sun_avoid_angle = 15.L);

    /**
     * @brief TrackingSLR constructor. Receives the necessary parameters for looking for a SLR tracking.
     * @param min_elev, the minimum elevation in degrees at which the tracking starts.
     * @param tp_start, the time point datetime to start looking for a tracking.
     * @param predictor, the predictor used for calculating the tracked object positions at a given time.
     * @param time_delta, the time delta used to analyze the tracking. The smallest, the more precise.
     * @param avoid_sun (optional), true if you want the sun avoidance to be applied, false otherwise.
     * @param sun_avoid_angle (optional), if sun avoidance is applied, the radius of the sun security sector in degrees.
     */
    TrackingSLR(long double min_elev, const timing::HRTimePointStd& tp_start, PredictorSLR&& predictor,
                long double time_delta = 1.L, bool avoid_sun = true, long double sun_avoid_angle = 15.L);

    /**
     * @brief This function checks if there is a valid SLR tracking. You MUST check this, before requesting positions.
     * @return true if there is a valid tracking, false otherwise.
     */
    bool isValid() const;
    /**
     * @brief This function returns the minimum elevation of this tracking in degrees.
     * @return the minimum elevation of the tracking in degrees.
     */
    long double getMinElev() const;
    /**
     * @brief If this traking is valid, you can get the tracking start with this function.
     * @param mjd, the MJ date in days for the tracking start.
     * @param sod, the second of day for the tracking start.
     */
    void getTrackingStart(MJDate &mjd, SoD& sod) const;
    /**
     * @brief If this tracking is valid, you can get the tracking end with this function.
     * @param mjd, the MJ date in days for the tracking end.
     * @param sod, the second of day for the tracking end.
     */
    void getTrackingEnd(MJDate &mjd, SoD& sod) const;
    /**
     * @brief This function returns if sun avoidance is applied.
     * @return true if sun avoidance is applied, false otherwise.
     */
    bool getSunAvoidApplied() const;

    /**
     * @brief This function returns if there is sun overlapping in this tracking.
     * @return true if sun avoid is applied and there is an overlapping with the sun, false otherwise.
     */
    bool getSunOverlapping() const;

    /**
     * @brief This function returns if the start time of the tracking was modified due to a sun collision at the begining.
     * @return if sun avoid is applied, it returns true if tracking start time was modified due to sun, false otherwise.
     */
    bool getSunAtStart() const;

    /**
     * @brief This function returns if the end time of the tracking was modified due to a sun collision at the end.
     * @return if sun avoid is applied, it returns true if tracking end time was modified due to sun, false otherwise.
     */
    bool getSunAtEnd() const;

    /**
     * @brief This function returns the radius of the sun security sector applied to sun avoidance manouvre.
     *        This function should not be called if sun avoidance is not applied.
     * @return the radius of the sun security sector
     */
    long double getSunAvoidAngle() const;

    /**
     * @brief This function returns the object's position at a given time.
     * @param tp_time The time point datetime.
     * @param tracking_result, the returned TrackingResult struct.
     * @return the result of the operation. Must be checked to ensure the position is valid.
     */
    PositionStatus predictTrackingPosition(const timing::HRTimePointStd& tp_time, TrackingResult &tracking_result);

    /**
     * @brief This function returns the object's position at a given time.
     * @param mjd, the MJD in days.
     * @param sod, the second of day in seconds.
     * @param tracking_result, the returned TrackingResult struct.
     * @return the result of the operation. Must be checked to ensure the position is valid.
     */
    PositionStatus PredictTrackingPosition(MJDate mjd, SoD sod, TrackingResult &tracking_result);

private:

    void analyzeTracking(MJDate mjd_start, SoD sod_start);
    bool findTrackingStart(MJDate mjd_start, SoD sod_start);
    bool findTrackingEnd();

    bool insideSunSector(const PredictorSLR::InstantData& pos,
                         const dpslr::astro::SunPosition<long double>& sun_pos) const;
    void setSunSectorRotationDirection(
        SunSector &sector, const std::vector<dpslr::astro::SunPosition<long double>> &sun_positions);

    long double min_elev_;
    long double time_delta_;

    MJDate mjd_start_;
    SoD sod_start_;
    MJDate mjd_end_;
    SoD sod_end_;

    bool valid_pass_;
    bool avoid_sun_;
    long double sun_avoid_angle_;
    bool sun_at_start_;
    bool sun_at_end_;
    std::vector<SunSector> sun_sectors_;

    dpslr::algoslr::utils::PredictorSLR predictor_;
    dpslr::astro::PredictorSun<long double> sun_predictor_;


};

}}} // END NAMESPACES
// =====================================================================================================================
