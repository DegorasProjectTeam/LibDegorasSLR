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
 * @file predictor_mount_move.cpp
 * @author Degoras Project Team.
 * @brief This file contains the implementation of the class PredictorMountMovement.
 * @copyright EUPL License
 2306.1
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/TrackingMount/types/mount_position.h"
#include "LibDegorasSLR/libdegorasslr_init.h"
#include "LibDegorasSLR/TrackingMount/predictors/predictor_mount_movement.h"
// =====================================================================================================================

// LIBDPBASE INCLUDES
// =====================================================================================================================
#include "LibDegorasBase/Timing/utils/time_utils.h"
#include "LibDegorasBase/Timing/dates/datetime_types.h"
// =====================================================================================================================

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
namespace predictors{
// =====================================================================================================================

PredictorMountMovement::PredictorMountMovement(types::MountPositionV positions,
                                               astro::predictors::PredictorSunPtr pred_sun,
                                               const utils::MovementAnalyzerConfig& config) :
    mv_analyzer_(config),
    positions_(std::move(positions))
{
    // Check Degoras initialization.
    DegorasInitGuard();


    // Check positions
    if (!this->checkPositions(this->positions_))
    {
        throw std::invalid_argument("[LibDegorasSLR,TrackingMount,PredictorMountMovement] No positions found for movement.");
    }

    // Check Sun predictor
    if (!pred_sun)
    {
        throw std::invalid_argument("[LibDegorasSLR,TrackingMount,PredictorMountMovement] Invalid Sun predictor.");
    }

    // Check configured elevations.
    if(config.min_elev >= config.max_elev || config.min_elev > 90 ||
       config.max_elev > 90 || config.sun_avoid_angle > 90)
    {
        throw std::invalid_argument("[LibDegorasSLR,TrackingMount,PredictorMountMovement] Invalid angles configuration.");
    }

    // Check too high values for the sun avoid angle, so the algorithm can fail.
    if((config.sun_avoid_angle*2) + config.min_elev >= 90 || (config.sun_avoid_angle*2)+(90 - config.max_elev) >= 90)
    {
        throw std::invalid_argument("[LibDegorasSLR,TrackingMount,PredictorMountMovement] Sun avoid angle too high for the "
                                    "configured minimum and maximum elevations.");
    }

    this->mount_track_.predictor_sun = pred_sun;
    this->mount_track_.config = config;

    // Analyze the tracking.
    this->analyzeTracking();
}


bool PredictorMountMovement::isReady() const
{
    return this->mount_track_.track_info.valid_movement;
}

const MountTrackingMovement& PredictorMountMovement::getMountTrackingMovement() const
{
    return this->mount_track_;
}


PredictionMountMovement PredictorMountMovement::predict(const dpbase::timing::types::HRTimePointStd &tp_time) const
{


    auto mjdt = dpbase::timing::timePointToModifiedJulianDateTime(tp_time);

    // If requested position is outside of tracking, then return out of track prediction.
    if (mjdt < this->positions_.front().mjdt || mjdt > this->positions_.back().mjdt)
    {
        return {};
    }

    // Calculates the Sun position.
    dpbase::timing::dates::J2000DateTime j2000 = dpbase::timing::timePointToJ2000DateTime(tp_time);
    astro::predictors::PredictionSun sun_pos = this->mount_track_.predictor_sun->predict(j2000, false);

    astro::types::AltAzPos interp_pos = this->interpPos(tp_time);
    types::MountPosition mount_pos;

    // Store the info for analyzing
    mount_pos.mjdt = mjdt;
    mount_pos.altaz_coord = interp_pos;

    return PredictionMountMovement(this->mv_analyzer_.analyzePosition(this->mount_track_.track_info, mount_pos, sun_pos));
}

void PredictorMountMovement::analyzeTracking()
{
    // Results container and auxiliar.
    astro::types::LocalSunPositionV results_sun(this->positions_.size());

    // Calculate sun position for each mount position
    std::transform(this->positions_.begin(), this->positions_.end(), results_sun.begin(), [this](const auto& pos)
    {
        auto j2000_start = dpbase::timing::modifiedJulianDateToJ2000DateTime(pos.mjdt);
        return this->mount_track_.predictor_sun->predict(j2000_start, false);
    });

    // Analyze tracking
    this->mount_track_.track_info = this->mv_analyzer_.analyzeMovement(this->positions_, results_sun);


}

bool PredictorMountMovement::checkPositions(const types::MountPositionV &positions) const
{
    // If positions vector is empty, then it is not valid.
    if (positions.empty())
        return false;

    // Now we will check if time is strictly increasing.
    bool valid = true;

    auto it = positions.cbegin() + 1;

    while (it != positions.cend() && valid)
    {
        valid &= it->mjdt > (it - 1)->mjdt;
        it++;
    }

    // TODO: ensure positions are normalized 0-360 azimuth

    return valid;
}

astro::types::AltAzPos PredictorMountMovement::interpPos(const dpbase::timing::types::HRTimePointStd &tp) const
{

    const dpbase::timing::dates::MJDateTime mjdt = dpbase::timing::timePointToModifiedJulianDateTime(tp);

    auto it_upper = this->positions_.cbegin() + 1;

    while (mjdt > it_upper->mjdt) it_upper++;

    auto it_lower = it_upper - 1;

    auto time_fract = (mjdt - it_lower->mjdt) /
                      (it_upper->mjdt - it_lower->mjdt);

    const auto &pos_lower = it_lower->altaz_coord;
    const auto &pos_upper = it_upper->altaz_coord;

    dpbase::math::units::Degrees diff_az = pos_upper.az - pos_lower.az;

    if (diff_az > 180.L)
    {
        diff_az = diff_az - 360.L;
    }
    else if (diff_az < -180.L)
    {
        diff_az = diff_az + 360.L;
    }

    astro::types::AltAzPos intp_pos;

    intp_pos.az = pos_lower.az + (diff_az) * time_fract;

    if (intp_pos.az > 360.L)
        intp_pos.az -= 360.L;
    else if (intp_pos.az < 0.L)
        intp_pos.az += 360.L;

    // TODO: check normalization for elevation
    intp_pos.el = pos_lower.el + (pos_upper.el - pos_lower.el) * time_fract;

    return intp_pos;
}


}}} // END NAMESPACES
// =====================================================================================================================
