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
 * @file predictor_mount_slr.cpp
 * @author Degoras Project Team.
 * @brief This file contains the implementation of the class PredictorMountSLR.
 * @copyright EUPL License
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_init.h"
#include "LibDegorasSLR/Timing/utils/time_utils.h"
#include "LibDegorasSLR/TrackingMount/predictors/predictor_mount_slr.h"
// =====================================================================================================================

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
namespace predictors{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace timing::types;
using namespace astro;
using namespace astro::types;
using namespace math::units;
using namespace slr;
// ---------------------------------------------------------------------------------------------------------------------

PredictorMountSLR::PredictorMountSLR(const timing::dates::MJDateTime& pass_start,
                                     const timing::dates::MJDateTime& pass_end,
                                     slr::predictors::PredictorSlrPtr pred_slr,
                                     astro::predictors::PredictorSunPtr pred_sun,
                                     const utils::MovementAnalyzerConfig &config,
                                     math::units::MillisecondsU time_delta) :
    tr_analyzer_(config),
    time_delta_(time_delta)
{
    // Check Degoras initialization.
    DegorasInitGuard();

    // Check that start is before end.
    if (pass_start > pass_end)
    {
        throw std::invalid_argument("[LibDegorasSLR,TrackingMount,PredictorMountSLR] Pass start is after end.");
    }

    // Check SLR predictor
    if (!pred_slr || !pred_slr->isReady())
    {
        throw std::invalid_argument("[LibDegorasSLR,TrackingMount,PredictorMountSLR] Invalid SLR predictor.");
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
        throw std::invalid_argument("[LibDegorasSLR,TrackingMount,PredictorMountSLR] Invalid angles configuration.");
    }

    // Check too high values for the sun avoid angle, so the algorithm can fail.
    if((config.sun_avoid_angle*2) + config.min_elev >= 90 || (config.sun_avoid_angle*2)+(90 - config.max_elev) >= 90)
    {
        throw std::invalid_argument("[LibDegorasSLR,TrackingMount,PredictorMountSLR] Sun avoid angle too high for the "
                                    "configured minimum and maximum elevations.");
    }

    // TODO: First generate the pass and store this info.
    this->mount_track_.pass_mjdt_start = pass_start;
    this->mount_track_.pass_mjdt_end = pass_end;

    // Store the data.
    this->mount_track_.predictor_slr = pred_slr;
    this->mount_track_.predictor_sun = pred_sun;
    this->mount_track_.config = config;

    // Configure predictor slr in instant vector mode.
    pred_slr->setPredictionMode(slr::predictors::PredictorSlrBase::PredictionMode::INSTANT_VECTOR);

    // Analyze the tracking.
    this->analyzeTracking();
}

PredictorMountSLR::PredictorMountSLR(const HRTimePointStd &pass_start,
                                     const HRTimePointStd &pass_end,
                                     slr::predictors::PredictorSlrPtr pred_slr,
                                     astro::predictors::PredictorSunPtr pred_sun,
                                     const utils::MovementAnalyzerConfig &config,
                                     math::units::MillisecondsU time_delta) :
    PredictorMountSLR(timing::timePointToModifiedJulianDateTime(pass_start),
                      timing::timePointToModifiedJulianDateTime(pass_end),
                      pred_slr,
                      pred_sun,
                      config,
                      time_delta)

{

}


bool PredictorMountSLR::isReady() const
{
    std::unique_lock<std::mutex> lock(this->mtx_);
    return this->mount_track_.track_info.valid_movement;
}

const MountTrackingSLR &PredictorMountSLR::getMountTrackingSLR() const
{
    std::unique_lock<std::mutex> lock(this->mtx_);
    return this->mount_track_;
}

PredictionMountSLR PredictorMountSLR::predict(const timing::types::HRTimePointStd& tp_time) const
{
    timing::dates::MJDateTime mjdt = timing::timePointToModifiedJulianDateTime(tp_time);
    return this->predict(mjdt);
}

PredictionMountSLR PredictorMountSLR::predict(const timing::dates::MJDateTime &mjdt) const
{
    // Safe mutex lock
    std::unique_lock<std::mutex> lock(this->mtx_);

    // Calculates the Sun position.
    timing::dates::J2000DateTime j2000 = dpslr::timing::modifiedJulianDateToJ2000DateTime(mjdt);
    astro::predictors::PredictionSun sun_pos = this->mount_track_.predictor_sun->predict(j2000, false);

    // Calculates the space object position.
    slr::predictors::PredictionSLR prediction_result;
    auto pred_error = this->mount_track_.predictor_slr->predict(mjdt, prediction_result);

    types::MountPosition mount_pos;
    mount_pos.mjdt = mjdt;
    mount_pos.altaz_coord = prediction_result.instant_data->altaz_coord;

    PredictionMountSLR mount_pred(
        this->tr_analyzer_.analyzePosition(this->mount_track_.track_info, mount_pos, sun_pos), prediction_result);

    if (pred_error != 0)
    {
        mount_pred.pred_status = PredictionMountSLRStatus::SLR_PREDICTION_ERROR;
    }
    else if (utils::AnalyzedPositionStatus::OUT_OF_TRACK == mount_pred.status)
    {
        mount_pred.pred_status = PredictionMountSLRStatus::OUT_OF_TRACK;
    }
    else
    {
        mount_pred.pred_status = PredictionMountSLRStatus::VALID_PREDICTION;
    }

    return mount_pred;

}

slr::predictors::PredictorSlrPtr PredictorMountSLR::getPredictorSLR()
{
    std::unique_lock<std::mutex> lock(this->mtx_);
    return this->mount_track_.predictor_slr;
}

void PredictorMountSLR::analyzeTracking()
{
    // Results container and auxiliar.
    slr::predictors::PredictionSLRV results_slr;
    astro::predictors::PredictionSunV results_sun;
    const Milliseconds step_ms = static_cast<long double>(this->time_delta_);


    // --------------------------------------------------------------
    // TODO MOVE TO PASS GENERATOR
    // Parallel calculation of all SLR positions.
    results_slr = this->mount_track_.predictor_slr->predict(
        this->mount_track_.pass_mjdt_start, this->mount_track_.pass_mjdt_end, step_ms);

    // Check if we have prediction results.
    if(results_slr.empty())
        return;

    // Check that the predictions correspond to a pass.
    // Tracking is not valid if there are errors
    auto it = std::find_if(results_slr.begin(), results_slr.end(),
    [](const auto& pred)
    {
        return pred.error != 0;
    });

    if(it != results_slr.end())
        return;
    // --------------------------------------------------------------

    // Time transformations with milliseconds precision.
    // TODO Move to the Sun predictor class.
    auto j2000_start = timing::modifiedJulianDateToJ2000DateTime(this->mount_track_.pass_mjdt_start);
    auto j2000_end = timing::modifiedJulianDateToJ2000DateTime(this->mount_track_.pass_mjdt_end);

    // Parallel calculation of all Sun positions.
    results_sun = this->mount_track_.predictor_sun->predict(
        j2000_start, j2000_end + this->time_delta_ / 1000.L, this->time_delta_, false);
    
    // Create mount positions vector from SLR predictions. Ensure there is at least one more.
    types::MountPositionV mount_positions(results_slr.size());
    std::transform(results_slr.begin(), results_slr.end(), mount_positions.begin(), [](const auto& pred)
                   {
        types::MountPosition mount_pos;
        mount_pos.mjdt = pred.instant_range.mjdt;
        mount_pos.altaz_coord = pred.instant_data->altaz_coord;
        return mount_pos;
    });

    // Create local sun positions from sun predictions
    astro::types::LocalSunPositionV sun_positions(results_slr.size());
    std::copy(results_sun.begin(), results_sun.begin() + results_slr.size(), sun_positions.begin());

    // Analyze tracking
    this->mount_track_.track_info = this->tr_analyzer_.analyzeMovement(mount_positions, sun_positions);

    // Store all generated predictions if movement analysis was successful
    if (this->mount_track_.track_info.valid_movement)
    {

        this->mount_track_.predictions.resize(results_slr.size());
        for (std::size_t i = 0; i < results_slr.size(); i++)
        {
            this->mount_track_.predictions[i] = PredictionMountSLR(this->mount_track_.track_info.analyzed_positions[i],
                                                                   results_slr[i]);
        }
    }

}


}}} // END NAMESPACES
// =====================================================================================================================
