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

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Helpers/common_aliases_macros.h"
#include "LibDegorasSLR/Timing/types/datetime_types.h"
#include "LibDegorasSLR/Astronomical/types/astro_types.h"
#include "LibDegorasSLR/Astronomical/predictors/predictor_sun_base.h"
#include "LibDegorasSLR/UtilitiesSLR/predictors/predictor_slr_types.h"
#include "LibDegorasSLR/UtilitiesSLR/predictors/predictor_slr_base.h"
// =====================================================================================================================

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
// =====================================================================================================================


/// Alias for mount positions vector.
using MountPositionV = std::vector<MountPosition>;

struct TrackingPrediction
{
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE(TrackingPrediction)

    timing::types::MJDateTime mjdt;
    MountPosition pos;
    astro::PredictionSun sun_pred;

    // Status.
    PositionStatus status;  ///< The current postion status.

};

/// Alias for tracking predictions vector.
using TrackingPredictionV = std::vector<TrackingPrediction>;


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
    timing::types::MJDateTime mjdt;         ///< Modified Julian DateTime.

    // Result members.
    Optional<slr::PredictionSLR> slr_pred;  ///< Optional SLR prediction with the object pass position.
    Optional<astro::PredictionSun> sun_pred;  ///< Optional Sun position container.
    Optional<MountPosition> mount_pos; ///< Optional tracking mount position container.

    // Status.
    PositionStatus status;  ///< The current postion status.
};

/// Alias for mount slr predictions vector.
using MountPredictionSLRV = std::vector<MountPredictionSLR>;




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
struct MountPredictionMove
{
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE_DTOR_DEF(MountPredictionMove)

    // Datetime members.
    timing::types::HRTimePointStd tp;       ///< Timepoint of positions.
    timing::types::MJDateTime mjdt;         ///< Modified Julian DateTime.

    // Result members.
    Optional<astro::PredictionSun> sun_pred;  ///< Optional Sun position container.
    Optional<MountPosition> mount_pos; ///< Optional tracking mount position container.

    // Status.
    PositionStatus status;             ///< The current postion status.
};

/// Alias for mount slr predictions vector.
using MountPredictionMoveV = std::vector<MountPredictionMove>;

/**
 * @brief The MountTrackingMove struct contanis all tracking data for a movement tracking.
 */
struct MountTrackingMove
{
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE(MountTrackingMove)

    // Tracking data
    TrackingAnalyzerConfig config;                 ///< Contains the tracking user configuration.
    TrackingInfo track_info;                       ///< Contains the analyzed tracking information.
    MountPredictionMoveV predictions;

    // Begin and end iterators.
    MountPredictionMoveV::iterator tracking_begin_; ///< Iterator to tracking begining
    MountPredictionMoveV::iterator tracking_end_;   ///< Iterator to tracking end

    // Predictors.
    astro::PredictorSunPtr predictor_sun;   ///< Internal Sun predictor.
};



/**
 * @brief The MountTrackingSLR struct contains all the tracking data and the predictors used for a SLR tracking.
 */
struct MountTrackingSLR
{

    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE(MountTrackingSLR)

    // TODO THIS MUST BE STORED IN PASSINFO class.
    timing::types::MJDateTime pass_mjdt_start;
    timing::types::MJDateTime pass_mjdt_end;

    // Tracking data
    TrackingAnalyzerConfig config;           ///< Contains the tracking user configuration.
    TrackingInfo track_info;                 ///< Contains the analyzed tracking information.
    MountPredictionSLRV predictions;         ///< Predicted data for the required time interval.

    // Predictors.
    slr::PredictorSlrPtr predictor_slr;   ///< Internal PredictorSLR predictor.
    astro::PredictorSunPtr predictor_sun;   ///< Internal Sun predictor.
};



}} // END NAMESPACES
// =====================================================================================================================
