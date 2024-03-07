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
 * @version 2306.1
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_init.h"
#include "LibDegorasSLR/TrackingMount/predictors/predictor_mount_slr.h"
// =====================================================================================================================

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace timing::types;
using namespace astro::types;
using math::units::Milliseconds;
// ---------------------------------------------------------------------------------------------------------------------

PredictorMountSLR::PredictorMountSLR(const MJDateTime& pass_start, const MJDateTime& pass_end,
                                     PredictorSlrPtr pred_slr, PredictorSunPtr pred_sun,
                                     const TrackingAnalyzerConfig& config) :
    tr_analyzer_(config)
{
    // Check Degoras initialization.
    DegorasInit::checkMandatoryInit();

    // TODO: First generate the pass and store this info.
    this->mount_track_.pass_mjdt_start = pass_start;
    this->mount_track_.pass_mjdt_end = pass_end;

    // Store the data.
    this->mount_track_.predictor_slr = pred_slr;
    this->mount_track_.predictor_sun = pred_sun;
    this->mount_track_.config = config;

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

    // Configure predictor slr in instant vector mode.
    pred_slr->setPredictionMode(PredictorSlrBase::PredictionMode::INSTANT_VECTOR);

    // Analyze the tracking.
    this->analyzeTracking();
}


bool PredictorMountSLR::isReady() const
{
    return this->mount_track_.track_info.valid_pass;
}

const MountTrackingSLR& PredictorMountSLR::getMountTrackingSLR() const
{
    return this->mount_track_;
}


PositionStatus PredictorMountSLR::predict(const timing::HRTimePointStd& tp_time,
                                                 MountSLRPrediction &tracking_result) const
{
    MJDateTime mjdt = timing::timePointToModifiedJulianDateTime(tp_time);
    return predict(mjdt, tracking_result);
}

PositionStatus PredictorMountSLR::predict(const MJDateTime &mjdt, MountSLRPrediction &tracking_result) const
{
    // Auxiliar.
    const long double cfg_max_el = static_cast<long double>(this->mount_track_.config.max_elev);


    // Calculates the Sun position.
    long double j2000 = dpslr::timing::modifiedJulianDateToJ2000DateTime(mjdt).datetime();
    SunPrediction sun_pos = this->mount_track_.predictor_sun->predict(j2000, false);

    // Calculates the space object position.
    SLRPrediction prediction_result;
    auto pred_error = this->mount_track_.predictor_slr->predict(mjdt, prediction_result);

    // Store the info at result.
    tracking_result.mjdt = mjdt;
    tracking_result.slr_pred = prediction_result;
    tracking_result.sun_pred = sun_pos;

    // Check for errors. If there was an error at prediction, return with sun and slr predictions.
    if (0 != pred_error)
    {
        tracking_result.status =  PositionStatus::PREDICTION_ERROR;
        return PositionStatus::PREDICTION_ERROR;
    }

    // Store the info at TrackingPrediction for analyzing
    TrackingPrediction tp;
    tp.mjdt = mjdt;
    tp.pos = prediction_result.instant_data->altaz_coord;
    tp.sun_pred = sun_pos;

    this->tr_analyzer_.analyzePrediction(tp);

    tracking_result.status = tp.status;
    if (tracking_result.status == PositionStatus::OUT_OF_TRACK)
    {
        tracking_result.slr_pred.reset();
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

void PredictorMountSLR::analyzeTracking()
{
    // Results container and auxiliar.
    SLRPredictionV results_slr;
    SunPredictionV results_sun;
    Milliseconds step_ms = static_cast<long double>(this->mount_track_.config.time_delta);

    // Update flag.
    this->mount_track_.track_info.valid_pass = false;

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
    [](const SLRPrediction& pred)
    {
        return pred.error != 0;
    });

    if(it != results_slr.end())
        return;
    // --------------------------------------------------------------

    // Time transformations with milliseconds precision.
    // TODO Move to the Sun predictor class.
    J2000DateTime j2000_start = timing::modifiedJulianDateToJ2000DateTime(this->mount_track_.pass_mjdt_start);
    J2000DateTime j2000_end = timing::modifiedJulianDateToJ2000DateTime(this->mount_track_.pass_mjdt_end);

    // Parallel calculation of all Sun positions.
    results_sun = this->mount_track_.predictor_sun->predict(
        j2000_start, j2000_end, this->mount_track_.config.time_delta, false);
    

    // Create tracking predictions for TrackingAnalyzer
    TrackingPredictionV tr_predictions(results_slr.size());
    for (std::size_t i = 0; i < results_slr.size(); i++)
    {
        TrackingPrediction tp;
        tp.mjdt = results_slr[i].instant_data->mjdt;
        tp.pos = results_slr[i].instant_data->altaz_coord;
        tp.sun_pred = results_sun[i];
        tr_predictions[i] = std::move(tp);
    }

    // Analyze tracking
    this->tr_analyzer_.analyzeTracking(tr_predictions);

    // Store data from analysis
    this->mount_track_.track_info = this->tr_analyzer_.getTrackingInfo();

    // Store all the generated data. At this momment, the
    // tracking positions have not been generated.
    this->mount_track_.predictions.resize(results_slr.size());
    for (std::size_t i = 0; i < results_slr.size(); i++)
    {
        MountSLRPrediction tr;
        tr.mjdt = results_slr[i].instant_data->mjdt;
        tr.status = this->tr_analyzer_.getPredictions()[i].status;
        if (tr.status == PositionStatus::OUTSIDE_SUN ||
            tr.status == PositionStatus::INSIDE_SUN ||
            tr.status == PositionStatus::AVOIDING_SUN)
        {
            tr.slr_pred = results_slr[i];
            tr.sun_pred = results_sun[i];
            tr.mount_pos = this->tr_analyzer_.getPredictions()[i].pos;
        }
        else if (tr.status == PositionStatus::CANT_AVOID_SUN || tr.status == PositionStatus::PREDICTION_ERROR)
        {
            tr.slr_pred = results_slr[i];
            tr.sun_pred = results_sun[i];
        }

        this->mount_track_.predictions[i] = std::move(tr);
    }

}


}} // END NAMESPACES
// =====================================================================================================================
