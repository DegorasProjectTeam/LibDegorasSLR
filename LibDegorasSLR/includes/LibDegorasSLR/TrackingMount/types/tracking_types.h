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
 * @file tracking_types.h
 * @author Degoras Project Team.
 * @brief This file contains the definition of types related with trackings.
 * @copyright EUPL License
 * @version
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Helpers/common_aliases_macros.h"
#include "LibDegorasSLR/Timing/types/base_time_types.h"
#include "LibDegorasSLR/Timing/types/datetime_types.h"
#include "LibDegorasSLR/Astronomical/types/astro_types.h"
#include "LibDegorasSLR/Astronomical/predictors/predictor_sun_base.h"
#include "LibDegorasSLR/UtilitiesSLR/predictors/prediction_data_slr.h"
#include "LibDegorasSLR/UtilitiesSLR/predictors/predictor_slr_base.h"
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using dpslr::timing::types::HRTimePointStd;
using dpslr::timing::types::MJDateTime;
using dpslr::timing::types::MJDate;
using dpslr::timing::types::SoD;
using dpslr::math::units::DegreesU;
using dpslr::math::units::Degrees;
using dpslr::math::units::MillisecondsU;
using dpslr::math::units::Meters;
using dpslr::astro::types::AltAzPos;
using dpslr::astro::types::AltAzPosV;
using dpslr::utils::PredictionSLR;
using dpslr::utils::PredictionSLRV;
using dpslr::astro::PredictionSun;
using dpslr::astro::PredictionSunV;
using dpslr::astro::PredictorSunPtr;
using dpslr::utils::PredictorSlrPtr;
// ---------------------------------------------------------------------------------------------------------------------

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
// =====================================================================================================================

/**
 * @brief The PredictorMountSLRConfig struct contains the configuration parameters associated with a tracking. These
 * parameters will define the tracking requirements.
 *
 * @todo Max speeds.
 */
struct TrackingAnalyzerConfig
{    
    TrackingAnalyzerConfig(const MillisecondsU& time_delta, const DegreesU& sun_avoid_angle,
                           const DegreesU& min_elev, const DegreesU& max_elev, bool sun_avoid) :
        time_delta(time_delta),
        sun_avoid_angle(sun_avoid_angle),
        min_elev(min_elev),
        max_elev(max_elev),
        sun_avoid(sun_avoid)
    {}

    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE(TrackingAnalyzerConfig)

    MillisecondsU time_delta;  ///< Time delta fo calculations in milliseconds.
    DegreesU sun_avoid_angle;  ///< Avoid angle for Sun collisions in degrees.
    DegreesU min_elev;         ///< Configured minimum elevation (degrees).
    DegreesU max_elev;         ///< Configured maximum elevation (degrees).
    bool sun_avoid;            ///< Flag indicating if the track is configured for avoid the Sun.
};

/**
 * @brief Enumerates the possible status codes for a tracking position.
 *
 * This enumeration defines the status of a tracking position with respect to the Sun's position and the predictor.
 * It is used to quickly identify the tracking scenario and take appropriate action based on the status.
 */
enum class PositionStatus
{
    OUTSIDE_SUN,        ///< The final mount position is outside the sun.
    INSIDE_SUN,         ///< The final mount position is in the Sun and is configured for not avoiding.
    AVOIDING_SUN,       ///< The final mount position is avoiding sun security sector.
    CANT_AVOID_SUN,     ///< Final mount position can't be calculated, since it cannot avoid sun security sector.
    OUT_OF_TRACK,       ///< The time provided for prediction is outside of tracking.
    PREDICTION_ERROR    ///< The object position can't be calculated, there was a prediction error.
};

/**
 * @brief Represents the azimuth and elevation position of a tracking at a specific instant, as well as its status.
 *
 * This structure holds the calculated azimuth and elevation angles for the mount at a specific instant.
 * It also includes the differences between the real predicted position and the
 * track position. The necessity to deviate from the predicted path to avoid direct line-of-sight with the Sun or
 * other obstructions can result in these differences.
 *
 */
struct MountPosition
{
    MountPosition(const AltAzPos& pos) :
        altaz_coord(pos),
        diff_az(0.0L),
        diff_el(0.0L)
    {}

    MountPosition() :
        altaz_coord(AltAzPos()),
        diff_az(0.0L),
        diff_el(0.0L)
    {}

    M_DEFINE_CTOR_COPY_MOVE_OP_COPY_MOVE(MountPosition)

    AltAzPos altaz_coord;  ///< Altazimuth coordinate for the tracking mount in degrees.
    Degrees diff_az;       ///< Azimuth difference between space object prediction position and tracking position.
    Degrees diff_el;       ///< Elevation difference between space object prediction position and tracking position.

};

/// Alias for mount positions vector.
using MountPositionV = std::vector<MountPosition>;

struct TrackingPrediction
{
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE(TrackingPrediction)

    MJDateTime mjdt;
    MountPosition pos;
    PredictionSun sun_pred;

    // Status.
    PositionStatus status;  ///< The current postion status.

};

/// Alias for tracking predictions vector.
using TrackingPredictionV = std::vector<TrackingPrediction>;

/**
 * @brief Represents the result of a tracking prediction operation, including azimuth and elevation position that
 * the tracking mount must use at a specific time of a tracking.
 * If status is either OUTSIDE_SUN, INSIDE_SUN OR AVOIDING_SUN, all result members are available.
 * This structure is designed to encapsulate the outcome of a tracking prediction operation.
 * It includes timing information, tracking position, prediction results, and sun
 * position, depending on the position `status` information. See the **Note** section for more details.
 *

 *
 * @note The presence of optional members is contingent upon the PositionStatus `status` member:
 * - If `status` is `OUT_OF_TRACK`, the optional members (`tracking_position`, `sun_pos`)
 *   are not populated.
 * - If `status` is `PREDICTION_ERROR` or `CANT_AVOID_SUN`, `sun_pos` and `mount_pos` provided to
 *   detail the prediction outcome and solar interference, respectively.
 */
struct MountMovePrediction
{
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE(MountMovePrediction)

    // Datetime members.
    MJDateTime mjdt;         ///< Modified Julian DateTime.

    // Result members.
    Optional<PredictionSun> sun_pred;  ///< Optional Sun position container.
    Optional<MountPosition> mount_pos; ///< Optional tracking mount position container.

    // Status.
    PositionStatus status;  ///< The current postion status.
};

/// Alias for mount slr predictions vector.
using MountMovePredictionV = std::vector<MountMovePrediction>;

/**
 * @brief Represents the result of a tracking prediction operation for a SLR tracking,
 * including azimuth and elevation position that the tracking mount must use at a specific time.
 * If status is either OUTSIDE_SUN, INSIDE_SUN OR AVOIDING_SUN, all result members are available.
 * This structure is designed to encapsulate the outcome of a tracking prediction operation.
 * It includes timing information, tracking position, prediction results, and sun
 * position, depending on the position `status` information. See the **Note** section for more details.
 *
 * @note The presence of optional members is contingent upon the PositionStatus `status` member:
 * - If `status` is `OUT_OF_TRACK`, the optional members (`prediction_result`, `tracking_position`, `sun_pos`)
 *   are not populated.
 * - If `status` is `PREDICTION_ERROR` or `CANT_AVOID_SUN`, both `prediction_result` and `sun_pos` are provided to
 *   detail the prediction outcome and solar interference, respectively.
 */
struct MountPredictionSLR
{
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE(MountPredictionSLR)

    // Datetime members.
    MJDateTime mjdt;         ///< Modified Julian DateTime.

    // Result members.
    Optional<PredictionSLR> slr_pred;  ///< Optional SLR prediction with the object pass position.
    Optional<PredictionSun> sun_pred;  ///< Optional Sun position container.
    Optional<MountPosition> mount_pos; ///< Optional tracking mount position container.

    // Status.
    PositionStatus status;  ///< The current postion status.
};

/// Alias for mount slr predictions vector.
using MountPredictionSLRV = std::vector<MountPredictionSLR>;

/**
 * @brief The SunCollisionSector struct contains data of a sector where the space object real pass
 * crosses a Sun security sector.
 *
 * @brief Add sun positions. Update rotation and everything when max elevation limit is applied.
 */
struct SunCollisionSector
{
    /**
     * @brief Enumerates the possible rotation direction during a maneuver.
     */
    enum class RotationDirection
    {
        CLOCKWISE,            ///< Clockwise rotation maneuver.
        COUNTERCLOCKWISE      ///< Counterclockwise rotation maneuver
    };

    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE(SunCollisionSector)

    AltAzPosV altaz_sun_coords;  ///< Altazimuth coordinates of the Sun during the collision time in degrees.
    AltAzPos altaz_entry;        ///< Sun sector altazimuth entry point coordinate in degrees.
    AltAzPos altaz_exit;         ///< Sun sector altazimuth exit point coordinate in degrees.
    MJDateTime mjdt_entry;       ///< MJ datetime of sun sector entry point.
    MJDateTime mjdt_exit;        ///< MJ datetime of sun sector exit point.
    RotationDirection cw;        ///< Rotation direction of the avoidance manoeuvre.
};

/// Alias for vector of SunCollisionSector.
using SunCollisionSectorV = std::vector<SunCollisionSector>;

/**
 * @brief The TrackingInfo struct contains the information obtained from the tracking analysis.
 */
struct TrackingInfo
{
    TrackingInfo() :
        sun_deviation(false),
        sun_collision(false),
        sun_collision_at_middle(false),
        sun_collision_at_start(false),
        sun_collision_at_end(false),
        trim_at_start(false),
        trim_at_end(false),
        el_deviation(false),
        valid_pass(false)
    {}

    M_DEFINE_CTOR_COPY_MOVE_OP_COPY_MOVE(TrackingInfo)

    // Time data.
    MJDateTime mjdt_start;     ///< Tracking start Modified Julian Datetime.
    MJDateTime mjdt_end;       ///< Tracking end Modified Julian Datetime.

    // Position data.
    AltAzPos start_coord;         ///< Track start altazimuth coordinates.
    AltAzPos end_coord;           ///< Track end altazimuth  coordinates.
    Degrees max_el;               ///< Track maximum elevation in degrees.

    SunCollisionSectorV sun_sectors;  ///< Data for sun collision sectors

    // Tracking alterations.
    bool sun_deviation;           ///< Flag indicating if the track was deviated from pass due to Sun.
    bool sun_collision;           ///< Flag indicating if the pass has a collision with the Sun.
    bool sun_collision_at_middle; ///< Flag indicating if the pass has a collision at middle with the Sun.
    bool sun_collision_at_start;  ///< Flag indicating if the pass has a collision at start with the Sun.
    bool sun_collision_at_end;    ///< Flag indicating if the pass has a collision at end with the Sun.
    bool sun_collision_high;      ///< Flag indicating if the pass has a collision with a high sun sector.
    bool trim_at_start;           ///< Flag indicating if the pass was trimmed due to elevation or Sun at start.
    bool trim_at_end;             ///< Flag indicating if the pass was trimmed due to elevation or Sun at end.
    bool el_deviation;            ///< Flag indicating if the track was deviated from pass due to max elevation.

    // Validation flags.
    bool valid_pass;              ///< Flag indicating if the pass is valid.
};

/**
 * @brief The MountTrackingMove struct contanis all tracking data for a movement tracking.
 */
struct MountTrackingMove
{
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE(MountTrackingMove)

    // Tracking data
    TrackingAnalyzerConfig config;                 ///< Contains the tracking user configuration.
    TrackingInfo track_info;                       ///< Contains the analyzed tracking information.
    MountMovePredictionV predictions;

    // Begin and end iterators.
    MountMovePredictionV::iterator tracking_begin_; ///< Iterator to tracking begining
    MountMovePredictionV::iterator tracking_end_;   ///< Iterator to tracking end

    // Predictors.
    PredictorSunPtr predictor_sun;   ///< Internal Sun predictor.
};

/**
 * @brief The MountTrackingSLR struct contains all the tracking data and the predictors used for a SLR tracking.
 */
struct MountTrackingSLR
{

    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE(MountTrackingSLR)

    // TODO THIS MUST BE STORED IN PASSINFO class.
    MJDateTime pass_mjdt_start;
    MJDateTime pass_mjdt_end;

    // Tracking data
    TrackingAnalyzerConfig config;                         ///< Contains the tracking user configuration.
    TrackingInfo track_info;                       ///< Contains the analyzed tracking information.
    MountPredictionSLRV predictions;               ///< Predicted data for the required time interval.


    // Begin and end iterators.
    MountPredictionSLRV::iterator tracking_begin_; ///< Iterator to tracking begining
    MountPredictionSLRV::iterator tracking_end_;   ///< Iterator to tracking end

    // Predictors.
    PredictorSlrPtr predictor_slr;   ///< Internal PredictorSLR predictor.
    PredictorSunPtr predictor_sun;   ///< Internal Sun predictor.
};



}} // END NAMESPACES
// =====================================================================================================================
