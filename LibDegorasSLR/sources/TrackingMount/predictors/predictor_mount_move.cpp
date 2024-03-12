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
 * @brief This file contains the implementation of the class PredictorMountMove.
 * @copyright EUPL License
 * @version 2306.1
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_init.h"
#include "LibDegorasSLR/Timing/time_utils.h"
#include "LibDegorasSLR/TrackingMount/predictors/predictor_mount_move.h"
// =====================================================================================================================

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace timing::types;
using namespace astro::types;
using namespace math::types;
using namespace math::units;
using namespace astro;
// ---------------------------------------------------------------------------------------------------------------------

PredictorMountMove::PredictorMountMove(MovePositionV positions,
                                       PredictorSunPtr pred_sun,
                                       const TrackingAnalyzerConfig& config) :
    positions_(std::move(positions)),
    tr_analyzer_(config)
{
    // Check Degoras initialization.
    DegorasInit::checkMandatoryInit();


    // Check positions
    if (!this->checkPositions(this->positions_))
    {
        throw std::invalid_argument("[LibDegorasSLR,TrackingMount,PredictorMountSLR] No positions found for movement.");
    }

    // Check Sun predictor
    if (!pred_sun)
    {
        throw std::invalid_argument("[LibDegorasSLR,TrackingMount,PredictorMountSLR] Invalid Sun predictor.");
    }

    // Check configured elevations.
    if(config.min_elev >= config.max_elev || config.min_elev > 90 ||
       config.max_elev > 90 || config.sun_avoid_angle > 90)
    {
        throw std::invalid_argument("[LibDegorasSLR,TrackingMount,PredictorMountMove] Invalid angles configuration.");
    }

    // Check too high values for the sun avoid angle, so the algorithm can fail.
    if((config.sun_avoid_angle*2) + config.min_elev >= 90 || (config.sun_avoid_angle*2)+(90 - config.max_elev) >= 90)
    {
        throw std::invalid_argument("[LibDegorasSLR,TrackingMount,PredictorMountMove] Sun avoid angle too high for the "
                                    "configured minimum and maximum elevations.");
    }

    this->mount_track_.predictor_sun = pred_sun;
    this->mount_track_.config = config;

    // Analyze the tracking.
    this->analyzeTracking();
}


bool PredictorMountMove::isReady() const
{
    return this->mount_track_.track_info.valid_pass;
}

const MountTrackingMove& PredictorMountMove::getMountTrackingMove() const
{
    return this->mount_track_;
}


PositionStatus PredictorMountMove::predict(const HRTimePointStd &tp_time, MountPredictionMove &tracking_result) const
{

    // If requested position is outside of tracking, then return.
    if (tp_time < this->positions_.front().tp || tp_time > this->positions_.back().tp)
    {
        tracking_result.tp = tp_time;
        tracking_result.status = PositionStatus::OUT_OF_TRACK;
        return PositionStatus::OUT_OF_TRACK;
    }

    // Calculates the Sun position.
    J2000DateTime j2000 = timing::timePointToJ2000DateTime(tp_time);
    PredictionSun sun_pos = this->mount_track_.predictor_sun->predict(j2000, false);

    AltAzPos interp_pos = this->interpPos(tp_time);

    // Store the info at result.
    tracking_result.tp = tp_time;
    tracking_result.sun_pred = sun_pos;

    // Store the info at TrackingPrediction for analyzing
    TrackingPrediction tp;
    tp.mjdt = timing::timePointToModifiedJulianDateTime(tp_time);
    tp.pos = interp_pos;
    tp.sun_pred = sun_pos;

    this->tr_analyzer_.analyzePrediction(tp);

    tracking_result.status = tp.status;
    tracking_result.mount_pos = tp.pos;

    if (tracking_result.status == PositionStatus::OUT_OF_TRACK)
    {
        tracking_result.sun_pred.reset();
        tracking_result.mount_pos.reset();
    }
    else if (tracking_result.status == PositionStatus::CANT_AVOID_SUN ||
             tracking_result.status == PositionStatus::PREDICTION_ERROR)
    {
        tracking_result.mount_pos.reset();
    }

    return tracking_result.status;
}

void PredictorMountMove::analyzeTracking()
{
    // Results container and auxiliar.
    PredictionSunV results_sun;

    // Update flag.
    this->mount_track_.track_info.valid_pass = false;

    // Time transformations with milliseconds precision.
    // TODO Move to the Sun predictor class.
    J2000DateTime j2000_start = timing::timePointToJ2000DateTime(this->positions_.front().tp);
    J2000DateTime j2000_end = timing::timePointToJ2000DateTime(this->positions_.back().tp);

    // Parallel calculation of all Sun positions.
    results_sun = this->mount_track_.predictor_sun->predict(
        j2000_start, j2000_end, this->mount_track_.config.time_delta, false);
    
    // Create tracking predictions for TrackingAnalyzer
    TrackingPredictionV tr_predictions(this->positions_.size());
    for (std::size_t i = 0; i < this->positions_.size(); i++)
    {
        TrackingPrediction tp;
        tp.mjdt = timing::timePointToModifiedJulianDateTime(this->positions_[i].tp);
        tp.pos = this->positions_[i].pos;
        tp.sun_pred = results_sun[i];
        tr_predictions[i] = std::move(tp);
    }

    // Analyze tracking
    this->tr_analyzer_.analyzeTracking(tr_predictions);

    // Store data from analysis
    this->mount_track_.track_info = this->tr_analyzer_.getTrackingInfo();

    // Store all the generated data. At this momment, the
    // tracking positions have not been generated.
    this->mount_track_.predictions.resize(this->positions_.size());
    for (std::size_t i = 0; i < this->positions_.size(); i++)
    {
        MountPredictionMove tr;
        tr.tp = this->positions_[i].tp;
        tr.status = this->tr_analyzer_.getPredictions()[i].status;
        if (tr.status == PositionStatus::OUTSIDE_SUN ||
            tr.status == PositionStatus::INSIDE_SUN ||
            tr.status == PositionStatus::AVOIDING_SUN)
        {
            tr.sun_pred = results_sun[i];
            tr.mount_pos = this->tr_analyzer_.getPredictions()[i].pos;
        }
        else if (tr.status == PositionStatus::CANT_AVOID_SUN || tr.status == PositionStatus::PREDICTION_ERROR)
        {
            tr.sun_pred = results_sun[i];
        }

        this->mount_track_.predictions[i] = tr;
    }

}

bool PredictorMountMove::checkPositions(const MovePositionV &positions) const
{
    // If positions vector is empty, then it is not valid.
    if (positions.empty())
        return false;

    // Now we will check if time is strictly increasing.
    bool valid = true;

    auto it = positions.cbegin() + 1;

    while (it != positions.cend() && valid)
    {
        valid &= it->tp > (it - 1)->tp;
        it++;
    }

    // TODO: ensure positions are normalized 0-360 azimuth

    return valid;
}

AltAzPos PredictorMountMove::interpPos(const timing::types::HRTimePointStd &tp) const
{

    auto it_upper = this->positions_.cbegin() + 1;

    while (tp > it_upper->tp) it_upper++;

    auto it_lower = it_upper - 1;

    auto time_fract = static_cast<long double>((tp - it_lower->tp).count()) /
                      static_cast<long double>((it_upper->tp - it_lower->tp).count());

    const auto &pos_lower = it_lower->pos;
    const auto &pos_upper = it_upper->pos;

    Degrees diff_az = pos_upper.az - pos_lower.az;

    if (diff_az > 180.L)
    {
        diff_az = diff_az - 360.L;
    }
    else if (diff_az < -180.L)
    {
        diff_az = diff_az + 360.L;
    }

    AltAzPos intp_pos;

    intp_pos.az = pos_lower.az + (diff_az) * time_fract;

    if (intp_pos.az > 360.L)
        intp_pos.az -= 360.L;
    else if (intp_pos.az < 0.L)
        intp_pos.az += 360.L;

    // TODO: check normalization for elevation
    intp_pos.el = pos_lower.el + (pos_upper.el - pos_lower.el) * time_fract;

    return intp_pos;
}


}} // END NAMESPACES
// =====================================================================================================================
