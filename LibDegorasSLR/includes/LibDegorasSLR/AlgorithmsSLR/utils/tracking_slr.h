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
 * This class uses a @a PredictorSLR to look for a SLR tracking within the given time window, i.e., the object is
 * always above the minimum elevation.
 *
 * This class also offers a sun avoidance algorithm. This algorithm changes the tracking trajectory whenever it passes
 * through a circular sector around the sun (the sun security sector). There are other checkings performed to the
 * tracking by this class. Before using the tracking you should check whether the tracking is valid or not.
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
        MJDateTime mjdt_entry;      ///< MJ datetime of sun sector entry point
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

    /**
     * @brief The TrackingPosition struct represents the pointing position of a tracking at a given time. It includes
     * a diff_az and diff_el members, which are used when the tracking is avoiding a sun security sector. In that case,
     * these members hold the difference between the real azimuth and elevation and the azimuth and elevation used for
     * avoiding the sun sector.
     */
    struct TrackingPosition
    {
        long double az;      ///< Azimuth of the space object in degrees.
        long double el;      ///< Elevation of the space object in degrees.
        long double diff_az; ///< Azimuth difference between the real prediction and tracking position.
        long double diff_el; ///< Elevation difference between the real prediction and tracking position.
    };

    /**
     * @brief This struct contanis the azimuth and elevation postion for a given time of the tracking.
     * Depending on the status returned, the available data may vary.
     * If status is OUT_OF_TRACK no result member is available, only the datetime
     * If status is PREDICTION_ERROR, prediction_result and sun_pos are available, but no tracking_position.
     * If status is CANT_AVOID_SUN, prediction_result and sun_pos are available, but no tracking_position.
     * If status is either OUTSIDE_SUN, INSIDE_SUN OR AVOIDING_SUN, all result members are available.
     * Care must be taken if status is INSIDE_SUN, since the class is configured to not avoid sun and the position
     * returned is within a sun security sector.
     */
    struct TrackingResult
    {
        // Datetime members.
        MJDate mjd;                                                 ///< Modified julian date in days.
        SoD sod;                                                    ///< Second of day in that MJD.
        MJDateTime mjdt;                                            ///< Modified julian datetime (day & fraction).

        // Result members.
        Optional<PredictorSLR::PredictionResult> prediction_result; ///< SLR prediction result.
        Optional<TrackingPosition> tracking_position;               ///< Tracking position.
        Optional<astro::SunPosition<long double>> sun_pos;          ///< Sun position.

        // Status.
        PositionStatus status;                                      ///< The postion status.
    };

    struct TrackSLR
    {
        // TODO: velocities
        MJDate mjd_start;
        SoD sod_start;
        MJDate mjd_end;
        SoD sod_end;
        MJDate mjd_max_elev;
        SoD sod_max_elev;

        double start_elev;
        double end_elev;
        double max_elev;

        bool valid_pass;
        bool avoid_sun;

        double min_elev;           ///< Degrees.
        double time_delta;         ///< Seconds.
        double sun_avoid_angle;    ///< Degrees.

        bool sun_collision_at_start;
        bool sun_collision_at_end;

        std::vector<SunSector> sun_sectors;

        std::vector<TrackingResult> positions;
    };

    /**
     * @brief TrackingSLR constructor. Receives the necessary parameters for looking for a SLR tracking.
     * @param min_elev, the minimum elevation in degrees at which the tracking starts.
     * @param mjd_search, the search Modified Julian Date in days to start looking for a tracking.
     * @param sod_search, the search Second of Day inside the previous `mjd_search` to start looking for a tracking.
     * @param predictor, the predictor used for calculating the tracked object positions at a given time.
     * @param time_delta, the time delta in milliseconds used to analyze the tracking. The smallest, the more precise.
     * @param avoid_sun, true if you want the sun avoidance to be applied, false otherwise.
     * @param sun_avoid_angle, if sun avoidance is applied, the radius of the sun security sector in degrees.
     */
    TrackingSLR(unsigned min_elev, MJDate mjd_start, SoD sod_start, MJDate mjd_end, SoD sod_end,
                PredictorSLR&& predictor, unsigned time_delta_ms = 1000,
                bool avoid_sun = true, unsigned sun_avoid_angle = 15);

    /**
     * @brief TrackingSLR constructor. Receives the necessary parameters for looking for a SLR tracking.
     * @param min_elev, the minimum elevation in degrees at which the tracking starts.
     * @param tp_start, the time point datetime to start looking for a tracking.
     * @param predictor, the predictor used for calculating the tracked object positions at a given time.
     * @param time_delta, the time delta in milliseconds used to analyze the tracking. The smallest, the more precise.
     * @param avoid_sun, true if you want the sun avoidance to be applied, false otherwise.
     * @param sun_avoid_angle, if sun avoidance is applied, the radius of the sun security sector in degrees.
     */
    TrackingSLR(unsigned min_elev, const timing::HRTimePointStd& tp_start, const timing::HRTimePointStd& tp_end,
                PredictorSLR&& predictor, unsigned time_delta_ms = 1000,
                bool avoid_sun = true, unsigned sun_avoid_angle = 15);

    /**
     * @brief This function checks if there is a valid SLR tracking. You should check this, before requesting positions.
     * @return true if there is a valid tracking, false otherwise.
     */
    bool isValid() const;

    /**
     * @brief This function returns the tracking info available.
     * @return the tracking info.
     */
    const TrackSLR& getTrackingInfo() const;

    /**
     * @brief This function returns the minimum elevation of this tracking in degrees.
     * @return the minimum elevation of the tracking in degrees.
     */
    unsigned getMinElev() const;

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
    bool isSunOverlapping() const;

    /**
     * @brief This function returns if the start time of the tracking was modified due to a sun collision
     * at the begining.
     * @return if sun avoid is applied, it returns true if tracking start time was modified due to sun, false otherwise.
     */
    bool isSunAtStart() const;

    /**
     * @brief This function returns if the end time of the tracking was modified due to a sun collision at the end.
     * @return if sun avoid is applied, it returns true if tracking end time was modified due to sun, false otherwise.
     */
    bool isSunAtEnd() const;

    /**
     * @brief This function returns the radius of the sun security sector applied to sun avoidance manouvre.
     *        This function should not be called if sun avoidance is not applied.
     * @return the radius of the sun security sector
     */
    unsigned getSunAvoidAngle() const;

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
    PositionStatus predictTrackingPosition(MJDate mjd, SoD sod, TrackingResult &tracking_result);

private:

    void analyzeTracking();
    bool checkTrackingStart();
    bool checkTrackingEnd();
    bool checkTracking();
    bool insideSunSector(const PredictorSLR::InstantData& pos,
                         const dpslr::astro::SunPosition<long double>& sun_pos) const;
    void setSunSectorRotationDirection(
        SunSector &sector, const std::vector<dpslr::astro::SunPosition<long double>> &sun_positions);


    dpslr::algoslr::utils::PredictorSLR predictor_;
    dpslr::astro::PredictorSun<long double> sun_predictor_;

    TrackSLR track_info_;
};

}}} // END NAMESPACES
// =====================================================================================================================
