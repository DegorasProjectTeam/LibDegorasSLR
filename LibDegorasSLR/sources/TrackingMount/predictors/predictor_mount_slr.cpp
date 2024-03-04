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
 * @file tracking_slr.cpp
 * @author Degoras Project Team.
 * @brief This file contains the implementation of the class TrackingSLR.
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
#include "LibDegorasSLR/Mathematics/math_constants.h"
// =====================================================================================================================

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace timing::types;
using namespace astro::types;
// ---------------------------------------------------------------------------------------------------------------------

PredictorMountSLR::PredictorMountSLR(std::shared_ptr<PredictorSLR> pred_slr,
                                     std::shared_ptr<PredictorSun> pred_sun,
                                     const MJDateTime& mjdt_start,
                                     const MJDateTime& mjdt_end,
                                     MillisecondsU time_delta, DegreesU min_elev,
                                     DegreesU max_elev, DegreesU sun_avoid_angle, bool sun_avoid) :
    predictor_(std::move(pred_slr)),
    sun_predictor_(std::move(pred_sun)),
    mount_track_(*this->predictor_, *this->sun_predictor_)
{
    // Check Degoras initialization.
    DegorasInit::checkMandatoryInit();

    // Store the configuration data.
    this->mount_track_.config.mjdt_start = mjdt_start;
    this->mount_track_.config.mjdt_end = mjdt_end;
    this->mount_track_.config.min_elev = min_elev;
    this->mount_track_.config.max_elev = max_elev;
    this->mount_track_.config.time_delta = time_delta;
    this->mount_track_.config.sun_avoid_angle = sun_avoid_angle;
    this->mount_track_.config.sun_avoid = sun_avoid;

    // Check configured elevations.
    if(min_elev >= max_elev || min_elev > 90 || max_elev > 90 || sun_avoid_angle > 90)
        throw std::invalid_argument("[LibDegorasSLR,TrackingMount,PredictorMountSLR] Invalid angles configuration.");

    // Check too high values for the sun avoid angle, so the algorithm can fail.
    if((sun_avoid_angle*2)+min_elev >= 90 || (sun_avoid_angle*2)+(90-max_elev) >= 90)
        throw std::invalid_argument("[LibDegorasSLR,TrackingMount,PredictorMountSLR] Sun avoid angle too high for the "
                                    "configured minimum and maximum elevations.");

    // Configure predictor slr in instant vector mode.
    this->predictor_->setPredictionMode(PredictorSLR::PredictionMode::INSTANT_VECTOR);

    // Analyze the tracking.
    this->analyzeTracking();
}

PredictorMountSLR::PredictorMountSLR(std::shared_ptr<PredictorSLR> pred_slr,
                                     std::shared_ptr<PredictorSun> pred_sun,
                                     const HRTimePointStd& tp_start,
                                     const HRTimePointStd &tp_end, MillisecondsU time_delta, DegreesU min_elev,
                                     DegreesU max_elev, DegreesU sun_avoid_angle, bool sun_avoid) :
    PredictorMountSLR(pred_slr,
                      pred_sun,
                      timing::timePointToModifiedJulianDateTime(tp_start),
                      timing::timePointToModifiedJulianDateTime(tp_end),
                      time_delta,
                      min_elev,
                      max_elev,
                      sun_avoid_angle,
                      sun_avoid)
{

}

bool PredictorMountSLR::isValid() const
{
    return this->mount_track_.track_info.valid_pass;
}

const PredictorMountSLR::MountTrackSLR& PredictorMountSLR::getMountTrack() const
{
    return this->mount_track_;
}



void PredictorMountSLR::getTrackingStart(MJDateTime &mjdt) const
{
    //mjd = this->mount_track_.cfg_mjd_start;
    //sod = this->mount_track_.cfg_sod_start;
}

void PredictorMountSLR::getTrackingEnd(MJDateTime &mjdt) const
{
   // mjd = this->mount_track_.mjd_end;
    //sod = this->mount_track_.sod_end;
}

PredictorMountSLR::MountSLRPredictions::const_iterator PredictorMountSLR::getTrackingBegin() const
{
    return this->mount_track_.track_info.valid_pass ? this->tracking_begin_ : this->mount_track_.predictions.cend();
}

PredictorMountSLR::MountSLRPredictions::const_iterator PredictorMountSLR::getTrackingEnd() const
{
    return this->mount_track_.track_info.valid_pass ? this->tracking_end_ : this->mount_track_.predictions.cend();
}

PredictorMountSLR::PositionStatus PredictorMountSLR::predict(const timing::HRTimePointStd& tp_time,
                                                 MountSLRPrediction &tracking_result)
{
    MJDateTime mjdt = timing::timePointToModifiedJulianDateTime(tp_time);
    return predict(mjdt, tracking_result);
}

PredictorMountSLR::PositionStatus PredictorMountSLR::predict(const MJDateTime &mjdt, MountSLRPrediction &tracking_result)
{

    // Update the times.
    tracking_result.mjdt = mjdt;

    // Check if requested position is inside valid tracking time. Otherwise return out of tracking error.
    if (mjdt < this->mount_track_.config.mjdt_start || mjdt > this->mount_track_.config.mjdt_end)
    {
        tracking_result.status =  PositionStatus::OUT_OF_TRACK;
        return PositionStatus::OUT_OF_TRACK;
    }

    // Calculates the Sun position.
    long double j2000 = dpslr::timing::mjdtToJ2000Datetime(mjdt);
    SunPrediction sun_pos = this->sun_predictor_->predict(j2000, false);

    // Calculates the space object position.
    SLRPrediction prediction_result;
    auto pred_error = this->predictor_->predict(mjdt, prediction_result);

    // Store the info.
    tracking_result.slr_pred = prediction_result;
    tracking_result.sun_pred = sun_pos;

    // Check for errors.
    if (0 != pred_error)
    {
        tracking_result.status =  PositionStatus::PREDICTION_ERROR;
        return PositionStatus::PREDICTION_ERROR;
    }

    // Final position.
    MountPosition tracking_position;


    // TODO REFACTOR
    AltAzPos obj_altazpos = {prediction_result.instant_data->altaz_coord.az,
                                  prediction_result.instant_data->altaz_coord.el};

    bool inside_sun = this->insideSunSector(obj_altazpos, sun_pos.altaz_coord);


    // If sun avoidance is applied and position for requested time is inside a sun security sector, map the point
    // to its corresponding point in the sun security sector circumference.
    // Otherwise, return calculated position.
    if (this->mount_track_.config.sun_avoid )
    {
        auto sector_it = std::find_if(this->mount_track_.sun_sectors.begin(),
                                      this->mount_track_.sun_sectors.end(), [mjdt](const auto& sector)
                                      {
                                          return mjdt >= sector.mjdt_entry && mjdt <= sector.mjdt_exit;
                                      });

        if (sector_it == this->mount_track_.sun_sectors.end())
        {
            tracking_result.status =  PositionStatus::CANT_AVOID_SUN;
            return PositionStatus::CANT_AVOID_SUN;
        }

        long double angle_avoid = this->calcSunAvoidTrajectory(mjdt, *sector_it, sun_pos.altaz_coord);


        // Calculate the final tracking position and differences.
        tracking_position.altaz_coord.az = sun_pos.altaz_coord.az + this->mount_track_.config.sun_avoid_angle * std::cos(angle_avoid);
        if (tracking_position.altaz_coord.az < 0.L)
            tracking_position.altaz_coord.az += 360.L;
        tracking_position.altaz_coord.el = sun_pos.altaz_coord.el + this->mount_track_.config.sun_avoid_angle * std::sin(angle_avoid);
        tracking_position.diff_az = prediction_result.instant_data->altaz_coord.az - tracking_position.altaz_coord.az;
        tracking_position.diff_el = prediction_result.instant_data->altaz_coord.el - tracking_position.altaz_coord.el;
        tracking_result.mount_pos = std::move(tracking_position);

        // Return the status.
        tracking_result.status =  PositionStatus::AVOIDING_SUN;
        return PositionStatus::AVOIDING_SUN;
    }
    else
    {
        // Store the info.
        tracking_position.altaz_coord.az = prediction_result.instant_data->altaz_coord.az;
        tracking_position.altaz_coord.el = prediction_result.instant_data->altaz_coord.el;
        tracking_position.diff_az = 0;
        tracking_position.diff_el = 0;
        tracking_result.mount_pos = std::move(tracking_position);

        AltAzPos obj_altazpos = {prediction_result.instant_data->altaz_coord.az,
                                 prediction_result.instant_data->altaz_coord.el};

        // Check the Sun position for safety.
        if(inside_sun)
        {
            tracking_result.status =  PositionStatus::INSIDE_SUN;
            return PositionStatus::INSIDE_SUN;
        }
        else
        {
            tracking_result.status =  PositionStatus::OUTSIDE_SUN;
            return PositionStatus::OUTSIDE_SUN;
        }
    }

}

void PredictorMountSLR::analyzeTracking()
{
    // Results container and auxiliar.
    unsigned step_ms = this->mount_track_.config.time_delta;
    SLRPredictionV results_slr;
    SunPredictionV results_sun;

    // Update flag.
    this->mount_track_.track_info.valid_pass = false;

    // --------------------------------------------------------------
    // TODO MOVE TO PASS GENERATOR
    // Parallel calculation of all SLR positions.
    results_slr = this->predictor_->predict(this->mount_track_.config.mjdt_start,
                                           this->mount_track_.config.mjdt_end,
                                           step_ms);

    // Check if we have prediction results.
    if(results_slr.empty())
        return;

    // Check that the predictions correspond to a pass.
    auto it = std::find_if(results_slr.begin(), results_slr.end(),
        [](const auto& pred)
        {
            if(pred.error != 0)
                    return true;
            else
                return pred.instant_data->altaz_coord.el < 0;
        });
    if(it != results_slr.end())
        return;
    // --------------------------------------------------------------

    // TODO
    // COPIAR INFO DEL PASE AL TRACK. AHORA LO HACEMOS A MANO.
    this->mount_track_.track_info.mjdt_start = this->mount_track_.config.mjdt_start;
    this->mount_track_.track_info.mjdt_end = this->mount_track_.config.mjdt_end;
    // TODO MAX EL
    this->mount_track_.track_info.start_coord = results_slr.front().instant_data->altaz_coord;
    this->mount_track_.track_info.end_coord = results_slr.back().instant_data->altaz_coord;

    // Time transformations with milliseconds precision.
    // TODO Move to the Sun predictor class.
    J2000DateTime j2000_start = timing::modifiedJulianDateToJ2000DateTime(this->mount_track_.config.mjdt_start);
    J2000DateTime j2000_end = timing::modifiedJulianDateToJ2000DateTime(this->mount_track_.config.mjdt_end);

    // Parallel calculation of all Sun positions.
    results_sun = this->sun_predictor_->predict(j2000_start, j2000_end, step_ms, false);
    
    // Store all the generated data. At this momment, the
    // tracking positions have not been generated.
    for (std::size_t i = 0; i < results_slr.size(); i++)
    {
        MountSLRPrediction tr;
        tr.mjdt = results_slr[i].instant_data->mjdt;
        tr.slr_pred = results_slr[i];
        tr.sun_pred = results_sun[i];
        tr.mount_pos = results_slr[i].instant_data->altaz_coord;
        this->mount_track_.predictions.push_back(std::move(tr));
    }

    // Now, after positions have been calculated, check each situation.

    // Check the start and validate at this point.
    if (!(this->mount_track_.track_info.valid_pass = this->analyzeTrackingStart()))
        return;

    // Check the end and validate at this point.
    if (!(this->mount_track_.track_info.valid_pass = this->analyzeTrackingEnd()))
        return;

    // Check the middle and validate at this point.
    if (!(this->mount_track_.track_info.valid_pass = this->analyzeTrackingMiddle()))
        return;

    // Finally store if a collision exits.

}

bool PredictorMountSLR::analyzeTrackingStart()
{
    // Get the first prediction and min and max elevations.
    auto it_pred = this->mount_track_.predictions.begin();
    const long double min_el = static_cast<long double>(this->mount_track_.config.min_elev);
    const long double max_el = static_cast<long double>(this->mount_track_.config.max_elev);

    // Get the first valid position due to minimum and maximum elevations.
    while (it_pred != this->mount_track_.predictions.end() &&
           it_pred->slr_pred->instant_data->altaz_coord.el < min_el &&
           it_pred->slr_pred->instant_data->altaz_coord.el > max_el)
    {
        it_pred->status = PositionStatus::OUT_OF_TRACK;
        it_pred->mount_pos.reset();
        it_pred++;
    }

    // If the whole tracking has incorrect elevation, return.
    if (it_pred == this->mount_track_.predictions.end())
        return false;

    // 1 - Check Sun collisions if avoiding is disable.
    // 2 - Check Sun collisions if avoiding is enable.
    if(!this->mount_track_.config.sun_avoid)
    {
        // Iterate over the predictions. If sun avoid is disabled, check whether position
        // is inside or outside sun security sector without changing start.
        while (it_pred != this->mount_track_.predictions.end())
        {
            if (this->insideSunSector(it_pred->slr_pred->instant_data->altaz_coord, it_pred->sun_pred->altaz_coord))
            {
                this->mount_track_.track_info.sun_collision = true;
                this->mount_track_.track_info.sun_collision_at_start = true;
                it_pred->status = PositionStatus::INSIDE_SUN;
                it_pred++;
            }
            else
            {
                it_pred->status = PositionStatus::OUTSIDE_SUN;
                break;
            }
        }
    }
    else
    {
        // Iterate over the predictions. If sun avoid is enable, move the tracking start to the end
        // of the sun sector if possible.
        while (it_pred != this->mount_track_.predictions.end())
        {
            if(this->insideSunSector(it_pred->slr_pred->instant_data->altaz_coord, it_pred->sun_pred->altaz_coord))
            {
                this->mount_track_.track_info.sun_collision = true;
                this->mount_track_.track_info.sun_collision_at_start = true;
                it_pred->status = PositionStatus::OUT_OF_TRACK;
                it_pred->mount_pos.reset();
                it_pred++;
            }
            else
            {
                it_pred->status = PositionStatus::OUTSIDE_SUN;
                break;
            }
        }
    }

    // If the whole tracking is in the Sun, return.
    if (it_pred == this->mount_track_.predictions.end())
        return false;

    // If start has been moved, store new start.
    if (it_pred != this->mount_track_.predictions.begin())
    {
        this->mount_track_.track_info.trim_at_start = true;
        this->mount_track_.track_info.mjdt_start = it_pred->mjdt;
    }

    // Update the start position and the real track begin iterator.
    this->mount_track_.track_info.start_coord = it_pred->mount_pos->altaz_coord;
    this->tracking_begin_ = it_pred;

    // All ok, return true.
    return true;
}

bool PredictorMountSLR::analyzeTrackingEnd()
{
    // Get the first prediction and min and max elevations.
    auto it_pred = this->mount_track_.predictions.rbegin();
    const long double min_el = static_cast<long double>(this->mount_track_.config.min_elev);
    const long double max_el = static_cast<long double>(this->mount_track_.config.max_elev);

    // Get the first valid position due to minimum and maximum elevations.
    while (it_pred != this->mount_track_.predictions.rend() &&
           it_pred->slr_pred->instant_data->altaz_coord.el < min_el &&
           it_pred->slr_pred->instant_data->altaz_coord.el > max_el)
    {
        it_pred->status = PositionStatus::OUT_OF_TRACK;
        it_pred->mount_pos.reset();
        it_pred++;
    }

    // If the whole tracking has low elevation, return.
    if (it_pred == this->mount_track_.predictions.rend())
        return false;

    // 1 - Check Sun collisions if avoiding is disable.
    // 2 - Check Sun collisions if avoiding is enable.
    if (!this->mount_track_.config.sun_avoid)
    {
        // Iterate over the predictions. If sun avoid is disabled, check whether position
        // is inside or outside sun security sector without changing end.
        while (it_pred != this->mount_track_.predictions.rend())
        {
            if (this->insideSunSector(it_pred->slr_pred->instant_data->altaz_coord, it_pred->sun_pred->altaz_coord))
            {
                this->mount_track_.track_info.sun_collision = true;
                this->mount_track_.track_info.sun_collision_at_end = true;
                it_pred->status = PositionStatus::INSIDE_SUN;
                it_pred++;
            }
            else
            {
                it_pred->status = PositionStatus::OUTSIDE_SUN;
                break;
            }
        }
    }
    else
    {
        // Iterate over the predictions. If sun avoid is enable, move the tracking end to the end
        // of the sun sector if possible.
        while (it_pred != this->mount_track_.predictions.rend())
        {
            if(this->insideSunSector(it_pred->slr_pred->instant_data->altaz_coord, it_pred->sun_pred->altaz_coord))
            {
                this->mount_track_.track_info.sun_collision = true;
                this->mount_track_.track_info.sun_collision_at_end = true;
                it_pred->status = PositionStatus::OUT_OF_TRACK;
                it_pred->mount_pos.reset();
                it_pred++;
            }
            else
            {
                it_pred->status = PositionStatus::OUTSIDE_SUN;
                break;
            }
        }
    }

    // If the whole tracking is in the Sun, return.
    if (it_pred == this->mount_track_.predictions.rend())
        return false;

    // If end has been moved, store new end.
    if (it_pred != this->mount_track_.predictions.rbegin())
    {
        this->mount_track_.track_info.trim_at_end = true;
        this->mount_track_.track_info.mjdt_end = it_pred->mjdt;
    }

    // Update the end elevation and the real track end iterator.
    this->mount_track_.track_info.end_coord = it_pred->mount_pos->altaz_coord;
    this->tracking_end_ = it_pred.base();

    // All ok, return true.
    return true;
}

bool PredictorMountSLR::analyzeTrackingMiddle()
{
    MountSLRPrediction tr;
    bool in_sun_sector = false;
    bool inside_sun = false;
    bool sun_collision = false;
    SunCollisionSector sun_sector;
    MJDateTime max_elev_mjdt = 0;
    long double max_elev = -1.0L;
    const long double cfg_max_el = static_cast<long double>(this->mount_track_.config.max_elev);
    const long double sun_avoid_angle = static_cast<long double>(this->mount_track_.config.sun_avoid_angle);
    MountSLRPredictions::iterator sun_sector_start;

    // Check the tracking maximum altitudes. Positions that trespasses will be clipped to maximum elevation.
    for (auto it = this->tracking_begin_; it != this->tracking_end_; it++)
    {
        if(it->mount_pos->altaz_coord.el > cfg_max_el)
        {
            it->mount_pos->altaz_coord.el = cfg_max_el;
            this->mount_track_.track_info.el_deviation = true;
            this->mount_track_.track_info.max_el = cfg_max_el;
        }
    }

    // Check the tracking Sun collision.
    for (auto it = this->tracking_begin_; it != this->tracking_end_; it++)
    {
        // TODO REFACTOR
        AltAzPos obj_altazpos = it->mount_pos->altaz_coord;

        // Check if this position is inside sun security sector and store it.
        inside_sun = this->insideSunSector(obj_altazpos, it->sun_pred->altaz_coord);

        // Update sun collision variable.
        sun_collision |= inside_sun;

        // Store whether position is inside or outside sun. Later, if sun avoid algorithm is applied, those positions
        // which are inside sun will be checked to see if it is possible or not to avoid sun.
        it->status = inside_sun ? PositionStatus::INSIDE_SUN : PositionStatus::OUTSIDE_SUN;

        if (this->mount_track_.config.sun_avoid)
        {
            // Check if the Sun is too high.
            bool sun_high = (it->sun_pred->altaz_coord.el + sun_avoid_angle) >= cfg_max_el;

            // // Update maximum elevations if the Sun is too high. In this case, the maximum el
            if(inside_sun && sun_high)
            {
                long double limit_el = it->sun_pred->altaz_coord.el - sun_avoid_angle;
                limit_el = (limit_el < cfg_max_el) ? limit_el : cfg_max_el;

                for (auto it = this->tracking_begin_; it != this->tracking_end_; it++)
                {
                    if(it->mount_pos->altaz_coord.el >= limit_el)
                    {
                        it->mount_pos->altaz_coord.el = limit_el;
                        it->status = PositionStatus::AVOIDING_SUN;

                       // Update sun deviation flag.
                       this->mount_track_.track_info.sun_deviation = true;
                    }
                }
                break;
            }

            // If sun avoid is applied we have to store the data for each sector  where the tracking goes through a sun
            // security sector.This data will be used for calculating an alternative trajetctory at those sectors.
            else if (inside_sun)
            {
                // If there is a sun collision, start saving sun positions for this sector. The first positions is the first position before entering
                // the sector, i.e., it is ouside sun security sector.
                if (!in_sun_sector)
                {
                    in_sun_sector = true;
                    sun_sector_start = it-1;
                    sun_sector.altaz_entry = sun_sector_start->mount_pos->altaz_coord;
                    sun_sector.mjdt_entry = sun_sector_start->mjdt;

                }
            }
            else if (!inside_sun && in_sun_sector)
            {
                // If we were inside a sun sector, and we are going out of it, check sun sector rotation direction and positions within the sector and
                // store the sector. The last position stored is the first position after exiting the sector, i.e., it is outside sun security sector.
                in_sun_sector = false;
                sun_sector.altaz_exit = it->mount_pos->altaz_coord;
                sun_sector.mjdt_exit = it->mjdt;
                // If sector has no valid rotation direction, mark tracking as not valid, since sun cannot be avoided.
                if (!this->setSunSectorRotationDirection(sun_sector, sun_sector_start, it))
                    return false;
                this->checkSunSectorPositions(sun_sector, sun_sector_start, it);
                this->mount_track_.sun_sectors.push_back(std::move(sun_sector));
                sun_sector = {};
            }
        }

        if (it->mount_pos->altaz_coord.el > max_elev)
        {
            max_elev = it->mount_pos->altaz_coord.el;
            max_elev_mjdt = it->mjdt;
        }

    }

    // If there was a sun security sector until the position previous to the last one, finish the tracking before
    // starting the sector. This case should be almost impossible.
    // if (in_sun_sector)
    // {
    //     timing::MjdtToMjdAndSecs(sun_sector.mjdt_entry, this->mount_track_.mjd_end, this->mount_track_.sod_end);
    //     for (auto it = sun_sector_start + 1; it !=this->mount_track_.predictions.end(); it++)
    //         it->status = PositionStatus::OUT_OF_TRACK;
    //     this->tracking_end_ = sun_sector_start;
    //     this->mount_track_.track_info.sun_collision_at_end = true;
    // }

    // Finally update the track information.
    this->mount_track_.track_info.max_el = max_elev;
    this->mount_track_.track_info.sun_collision |= sun_collision;
    this->mount_track_.track_info.sun_collision_at_middle = sun_collision;

    // All ok, return true.
    return true;
}

bool PredictorMountSLR::insideSunSector(const AltAzPos& pass_pos, const AltAzPos& sun_pos) const
{

    long double diff_az = pass_pos.az - sun_pos.az;
    // If azimuth difference is greater than 180, then take the shorter way
    if (diff_az > 180.L)
        diff_az = 360.L - diff_az;
    long double diff_el = pass_pos.el - sun_pos.el;
    return std::sqrt(diff_az * diff_az + diff_el * diff_el) < this->mount_track_.config.sun_avoid_angle;
}

bool PredictorMountSLR::setSunSectorRotationDirection(SunCollisionSector &sector,
                                                      MountSLRPredictions::const_iterator sun_start,
                                                      MountSLRPredictions::const_iterator sun_end)
{
    bool valid_cw = true;
    bool valid_ccw = true;

    // To avoid the sun, we will calculate a circumference with radius sun_avoid_angle_ and we will map each point of
    // the original trajectory to a point of the circumference between the entry and the exit point. We can travel
    // around the circumference clockwise or counterclockwise.
    // We have to check if every point in the sun avoid trajectory is in a valid trajectory, i.e., the elevation is
    // between the minimum and 90 degrees. If the clockwise trajectory around the sun security sector is not valid,
    // then we have to choose the counterclockwise trajectory and viceversa.
    // If both trajectories are valid, we will choose the shorter one.

    for (auto it = sun_start; it <= sun_end; it++)
    {
        long double time_perc = (it->mjdt - sector.mjdt_entry) / (sector.mjdt_exit - sector.mjdt_entry);

        long double diff_az_entry = sector.altaz_entry.az - it->sun_pred->altaz_coord.az;
        if (diff_az_entry > 180.L)
        {
            diff_az_entry = -(360.L - diff_az_entry);
        }
        else if (diff_az_entry < -180.L)
        {
            diff_az_entry = 360.L + diff_az_entry;
        }

        long double diff_az_exit = sector.altaz_exit.az - it->sun_pred->altaz_coord.az;
        if (diff_az_exit > 180.L)
        {
            diff_az_exit = -(360.L - diff_az_exit);
        }
        else if (diff_az_exit < -180.L)
        {
            diff_az_exit = 360.L + diff_az_exit;
        }
        long double entry_angle = std::atan2(sector.altaz_entry.el - it->sun_pred->altaz_coord.el, diff_az_entry);

        long double exit_angle = std::atan2(sector.altaz_exit.el - it->sun_pred->altaz_coord.el, diff_az_exit);

        // Normalize angle between 0, 2pi
        if (entry_angle < 0)
            entry_angle += 2*math::kPi;

        if (exit_angle < 0)
            exit_angle += 2*math::kPi;

        // Get clockwise and counterclockwise angular distance between entry and exit angles
        long double cw_angle;
        long double ccw_angle;
        long double angle = exit_angle - entry_angle;

        if (angle > 0.L)
        {
            // If substract is positive, the angle is clockwise.
            cw_angle = angle;
            ccw_angle = -(2*math::kPi - cw_angle);
        }
        else
        {
            // If substract is negative, the angle is counterclockwise.
            ccw_angle = angle;
            cw_angle = (2*math::kPi + ccw_angle);
        }

        // Calculate avoid trajectory angle for this timestamp using time as a percentage of the angle.
        cw_angle = entry_angle + time_perc * cw_angle;
        ccw_angle = entry_angle + time_perc * ccw_angle;

        long double elev_cw = it->sun_pred->altaz_coord.el +
                              this->mount_track_.config.sun_avoid_angle * std::sin(cw_angle);
        long double elev_ccw = it->sun_pred->altaz_coord.el +
                               this->mount_track_.config.sun_avoid_angle * std::sin(ccw_angle);

        long double cfg_max_el = static_cast<long double>(this->mount_track_.config.max_elev);

        // Check if elevation is required to be below minimum or above 90 in each way. If that is the case,
        // that way will not be valid and the other must be used.
        if (elev_cw >= cfg_max_el || elev_cw < 0.0L)
            valid_cw = false;

        if (elev_ccw >= cfg_max_el || elev_ccw < 0.0L)
            valid_ccw = false;

    }

    // If no way is valid, then tracking is not valid. If one of the ways is not valid, use the other.
    // If both ways are valid, choose the shorter one.
    if (!valid_cw && !valid_ccw)
        return false;
    if (!valid_cw)
        sector.cw = RotationDirection::COUNTERCLOCKWISE;
    else if (!valid_ccw)
        sector.cw = RotationDirection::CLOCKWISE;
    else
    {
        // Calculate angle between start and end to see which is shorter.
        long double diff_az_entry = sector.altaz_entry.az - sun_start->sun_pred->altaz_coord.az;
        if (diff_az_entry > 180.L)
        {
            diff_az_entry = -(360.L - diff_az_entry);
        }
        else if (diff_az_entry < -180.L)
        {
            diff_az_entry = 360.L + diff_az_entry;
        }

        long double diff_az_exit = sector.altaz_exit.az - sun_end->sun_pred->altaz_coord.az;
        if (diff_az_exit > 180.L)
        {
            diff_az_exit = -(360.L - diff_az_exit);
        }
        else if (diff_az_exit < -180.L)
        {
            diff_az_exit = 360.L + diff_az_exit;
        }

        long double entry_angle = std::atan2(sector.altaz_entry.el - sun_start->sun_pred->altaz_coord.el, diff_az_entry);
        long double exit_angle = std::atan2(sector.altaz_exit.el - sun_end->sun_pred->altaz_coord.el, diff_az_exit);

        // Normalize angle between 0, 2pi
        if (entry_angle < 0)
            entry_angle += 2*math::kPi;

        if (exit_angle < 0)
            exit_angle += 2*math::kPi;

        // Get clockwise and counterclockwise angular distance between entry and exit angles
        long double cw_angle;
        long double ccw_angle;
        long double angle = exit_angle - entry_angle;

        if (angle > 0.L)
        {
            // If substract is positive, the angle is clockwise.
            cw_angle = angle;
            ccw_angle = -(2*math::kPi - cw_angle);
        }
        else
        {
            // If substract is negative, the angle is counterclockwise.
            ccw_angle = angle;
            cw_angle = (2*math::kPi + ccw_angle);
        }

        sector.cw = std::abs(cw_angle) < std::abs(ccw_angle) ?
                        RotationDirection::CLOCKWISE : RotationDirection::COUNTERCLOCKWISE;

    }

    return true;
}

void PredictorMountSLR::checkSunSectorPositions(
    const SunCollisionSector &sector, MountSLRPredictions::iterator sun_start, MountSLRPredictions::iterator sun_end)
{
    // Store the avoid angle.
    long double cfg_avoid_angle = static_cast<long double>(this->mount_track_.config.sun_avoid_angle);
    // Check positions within sun sector. First and last are excluded, since they are outside sun sector
    for (auto it = sun_start + 1; it != sun_end; it++)
    {
        long double angle_avoid = this->calcSunAvoidTrajectory(it->mjdt, sector, it->sun_pred->altaz_coord);
        it->mount_pos->altaz_coord.az = it->sun_pred->altaz_coord.az + cfg_avoid_angle * std::cos(angle_avoid);
        it->mount_pos->altaz_coord.el = it->sun_pred->altaz_coord.el + cfg_avoid_angle * std::sin(angle_avoid);
        it->mount_pos->diff_az = it->slr_pred->instant_data->altaz_coord.az - it->mount_pos->altaz_coord.az;
        it->mount_pos->diff_el = it->slr_pred->instant_data->altaz_coord.el - it->mount_pos->altaz_coord.el;
        it->status = PositionStatus::AVOIDING_SUN;

        // Update sun deviation flag.
        this->mount_track_.track_info.sun_deviation = true;
    }
}

long double PredictorMountSLR::calcSunAvoidTrajectory(const MJDateTime &mjdt,
                                                      const SunCollisionSector& sector,
                                                      const AltAzPos& sun_pos)
{
    long double time_perc = (mjdt - sector.mjdt_entry) / (sector.mjdt_exit - sector.mjdt_entry);

    long double diff_az_entry = sector.altaz_entry.az - sun_pos.az;
    // Normalize in range -180, 180
    // TODO: this does not work if azimuths are not normalized between 0, 360
    if (diff_az_entry > 180.L)
    {
        diff_az_entry = -(360.L - diff_az_entry);
    }
    else if (diff_az_entry < -180.L)
    {
        diff_az_entry = 360.L + diff_az_entry;
    }

    long double diff_az_exit = sector.altaz_exit.az - sun_pos.az;
    // Normalize in range -180, 180
    // TODO: this does not work if azimuths are not normalized between 0, 360
    if (diff_az_exit > 180.L)
    {
        diff_az_exit = -(360.L - diff_az_exit);
    }
    else if (diff_az_exit < -180.L)
    {
        diff_az_exit = 360.L + diff_az_exit;
    }

    long double entry_angle = std::atan2(sector.altaz_entry.el - sun_pos.el, diff_az_entry);

    long double exit_angle = std::atan2(sector.altaz_exit.el - sun_pos.el, diff_az_exit);

    if (entry_angle < 0)
        entry_angle += 2*math::kPi;

    if (exit_angle < 0)
        exit_angle += 2*math::kPi;


    long double angle = exit_angle - entry_angle;

    if (angle > 0.L)
    {
        if (sector.cw == RotationDirection::COUNTERCLOCKWISE)
            angle = -(2*math::kPi - angle);
    }
    else
    {
        if (sector.cw == RotationDirection::CLOCKWISE)
            angle = (2*math::kPi + angle);
    }

    return entry_angle + angle * time_perc;
}

PredictorMountSLR::MountTrackSLR::MountTrackSLR(const PredictorSLR &predictor_slr,
                                                const PredictorSun &predictor_sun) :
    predictor_slr(predictor_slr),
    predictor_sun(predictor_sun)
{}


}} // END NAMESPACES
// =====================================================================================================================
