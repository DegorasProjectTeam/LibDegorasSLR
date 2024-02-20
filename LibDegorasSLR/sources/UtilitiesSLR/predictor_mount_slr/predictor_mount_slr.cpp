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
#include "LibDegorasSLR/UtilitiesSLR/predictor_mount_slr/predictor_mount_slr.h"
#include "LibDegorasSLR/Timing/time_constants.h"
#include "LibDegorasSLR/Mathematics/math_constants.h"
// =====================================================================================================================

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace utils{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace dpslr::timing::types;
// ---------------------------------------------------------------------------------------------------------------------

PredictorMountSLR::PredictorMountSLR(PredictorSLR&& predictor, MJDate mjd_start, SoD sod_start, MJDate mjd_end, SoD sod_end,
                         unsigned min_elev_deg, unsigned time_delta_ms, bool sun_avoid, unsigned sun_avoid_angle) :
    predictor_(std::move(predictor)),
    sun_predictor_(this->predictor_.getGeodeticLocation()),
    mount_track_(this->predictor_.getCPF(), this->predictor_, this->sun_predictor_)
{
    // Check too high values for the sun avoid angle, so the algorithm can fail.
    if(sun_avoid_angle >= 30)
        throw std::invalid_argument("[LibDegorasSLR,UtilitiesSLR,PredictorMountSLR] Sun avoid angle too high: "
                                        + std::to_string(sun_avoid_angle));

    // Check too high values for the minimum elevation, so the algorithm can fail.
    if(min_elev_deg >= 70)
        throw std::invalid_argument("[LibDegorasSLR,UtilitiesSLR,PredictorMountSLR] Min elevation too high: "
                                    + std::to_string(min_elev_deg));

    // Check Degoras initialization.
    dpslr::DegorasInit::checkMandatoryInit();

    // Store the data.
    this->mount_track_.mjd_start = mjd_start;
    this->mount_track_.sod_start = sod_start;
    this->mount_track_.mjd_end = mjd_end;
    this->mount_track_.sod_end = sod_end;
    this->mount_track_.cfg_min_elev = min_elev_deg;
    this->mount_track_.cfg_time_delta = time_delta_ms;
    this->mount_track_.cfg_sun_avoid_angle = sun_avoid_angle;
    this->mount_track_.cfg_sun_avoid = sun_avoid;
    this->mount_track_.sun_collision_at_start = false;
    this->mount_track_.sun_collision_at_end = false;

    // Configure the predictor for fast instant vector mode, enough for an astronomical mount.
    this->predictor_.setPredictionMode(PredictorSLR::PredictionMode::INSTANT_VECTOR);

    // Analyze the tracking.
    this->analyzeTracking();

}

PredictorMountSLR::PredictorMountSLR(PredictorSLR&& predictor, const timing::types::HRTimePointStd &tp_start,
                         const timing::types::HRTimePointStd &tp_end, unsigned min_elev_deg, unsigned time_delta_ms,
                         bool sun_avoid, unsigned sun_avoid_angle) :

    predictor_(std::move(predictor)),
    sun_predictor_(this->predictor_.getGeodeticLocation()),
    mount_track_(this->predictor_.getCPF(), this->predictor_, this->sun_predictor_)
{
    // Check too high values for the sun avoid angle, so the algorithm can fail.
    if(sun_avoid_angle >= 30)
        throw std::invalid_argument("[LibDegorasSLR,UtilitiesSLR,PredictorMountSLR] Sun avoid angle too high: "
                                    + std::to_string(sun_avoid_angle));

    // Check too high values for the minimum elevation, so the algorithm can fail.
    if(min_elev_deg >= 70)
        throw std::invalid_argument("[LibDegorasSLR,UtilitiesSLR,PredictorMountSLR] Min elevation too high: "
                                    + std::to_string(min_elev_deg));

    // Check Degoras initialization.
    dpslr::DegorasInit::checkMandatoryInit();

    // Store the data.
    this->mount_track_.cfg_min_elev = min_elev_deg;
    this->mount_track_.cfg_time_delta = time_delta_ms;
    this->mount_track_.cfg_sun_avoid_angle = sun_avoid_angle;
    this->mount_track_.cfg_sun_avoid = sun_avoid;
    this->mount_track_.sun_collision_at_start = false;
    this->mount_track_.sun_collision_at_end = false;
    timing::timePointToModifiedJulianDate(tp_start, this->mount_track_.mjd_start, this->mount_track_.sod_start);
    timing::timePointToModifiedJulianDate(tp_end, this->mount_track_.mjd_end, this->mount_track_.sod_end);

    // Configure the predictor for fast instant vector mode, enough for an astronomical mount.
    this->predictor_.setPredictionMode(PredictorSLR::PredictionMode::INSTANT_VECTOR);

    // Analyze the tracking.
    this->analyzeTracking();
}

bool PredictorMountSLR::isValid() const
{
    return this->mount_track_.valid_pass;
}

const PredictorMountSLR::MountTrackSLR& PredictorMountSLR::getMountTrack() const
{
    return this->mount_track_;
}

unsigned PredictorMountSLR::getMinElev() const
{
    return static_cast<unsigned>(std::round(this->mount_track_.cfg_min_elev));
}

void PredictorMountSLR::getTrackingStart(MJDate &mjd, SoD &sod) const
{
    mjd = this->mount_track_.mjd_start;
    sod = this->mount_track_.sod_start;
}

void PredictorMountSLR::getTrackingEnd(MJDate &mjd, SoD &sod) const
{
    mjd = this->mount_track_.mjd_end;
    sod = this->mount_track_.sod_end;
}

PredictorMountSLR::MountSLRPredictions::const_iterator PredictorMountSLR::getTrackingBegin() const
{
    return this->mount_track_.valid_pass ? this->tracking_begin_ : this->mount_track_.predictions.cend();
}

PredictorMountSLR::MountSLRPredictions::const_iterator PredictorMountSLR::getTrackingEnd() const
{
    return this->mount_track_.valid_pass ? this->tracking_end_ : this->mount_track_.predictions.cend();
}

bool PredictorMountSLR::getSunAvoidApplied() const
{
    return this->mount_track_.cfg_sun_avoid;
}

bool PredictorMountSLR::isSunOverlapping() const
{
    return (!this->mount_track_.sun_sectors.empty() ||
            this->mount_track_.sun_collision_at_start ||
            this->mount_track_.sun_collision_at_end) ;
}

bool PredictorMountSLR::isSunAtStart() const
{
    return this->mount_track_.sun_collision_at_start;
}

bool PredictorMountSLR::isSunAtEnd() const
{
    return this->mount_track_.sun_collision_at_end;
}

unsigned PredictorMountSLR::getSunAvoidAngle() const
{
    return static_cast<unsigned>(this->mount_track_.cfg_sun_avoid_angle);
}

PredictorMountSLR::PositionStatus PredictorMountSLR::predict(const timing::HRTimePointStd& tp_time,
                                                 MountSLRPrediction &tracking_result)
{
    MJDate mjd;
    SoD sod;
    timing::timePointToModifiedJulianDate(tp_time, mjd, sod);
    return predict(mjd, sod, tracking_result);
}

PredictorMountSLR::PositionStatus PredictorMountSLR::predict(MJDate mjd, SoD sod,
                                                 MountSLRPrediction &tracking_result)
{
    // Update the times.
    tracking_result.mjd = mjd;
    tracking_result.sod = sod;
    tracking_result.mjdt = timing::modifiedJulianDateToModifiedJulianDatetime(mjd, sod);

    // Check if requested position is inside valid tracking time. Otherwise return out of tracking error.
    if (!dpslr::timing::mjdInsideTimeWindow(mjd, sod, this->mount_track_.mjd_start, this->mount_track_.sod_start,
                                            this->mount_track_.mjd_end, this->mount_track_.sod_end))
    {
        tracking_result.status =  PositionStatus::OUT_OF_TRACK;
        return PositionStatus::OUT_OF_TRACK;
    }

    // Calculates the Sun position.
    long double j2000 = dpslr::timing::mjdToJ2000Datetime(mjd, sod);
    SunPosition sun_pos = this->sun_predictor_.fastPredict(j2000, false).position;

    // Calculates the space object position.
    PredictorSLR::SLRPrediction prediction_result;
    auto pred_error = this->predictor_.predict(mjd, sod, prediction_result);

    // Store the info.
    tracking_result.prediction_result = prediction_result;
    tracking_result.sun_position = sun_pos;

    // Check for errors.
    if (pred_error != PredictorSLR::PredictionError::NO_ERROR &&
        pred_error != PredictorSLR::PredictionError::INTERPOLATION_NOT_IN_THE_MIDDLE)
    {
        tracking_result.status =  PositionStatus::PREDICTION_ERROR;
        return PositionStatus::PREDICTION_ERROR;
    }

    // Final position.
    MountPosition tracking_position;

    // If sun avoidance is applied and position for requested time is inside a sun security sector, map the point
    // to its corresponding point in the sun security sector circumference.
    // Otherwise, return calculated position.
    if (this->mount_track_.cfg_sun_avoid && this->insideSunSector(*prediction_result.instant_data, sun_pos))
    {
        MJDateTime mjdt = dpslr::timing::modifiedJulianDateToModifiedJulianDatetime(mjd, sod);

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

        long double angle_avoid = this->calcSunAvoidTrajectory(mjdt, *sector_it, sun_pos);


        // Calculate the final tracking position and differences.
        tracking_position.az = sun_pos.az + this->mount_track_.cfg_sun_avoid_angle * std::cos(angle_avoid);
        tracking_position.el = sun_pos.el + this->mount_track_.cfg_sun_avoid_angle * std::sin(angle_avoid);
        tracking_position.diff_az = prediction_result.instant_data->az - tracking_position.az;
        tracking_position.diff_el = prediction_result.instant_data->el - tracking_position.el;
        tracking_result.tracking_position = std::move(tracking_position);

        // Return the status.
        tracking_result.status =  PositionStatus::AVOIDING_SUN;
        return PositionStatus::AVOIDING_SUN;
    }
    else
    {
        // Store the info.
        tracking_position.az = prediction_result.instant_data->az;
        tracking_position.el = prediction_result.instant_data->el;
        tracking_position.diff_az = 0;
        tracking_position.diff_el = 0;
        tracking_result.tracking_position = std::move(tracking_position);

        // Check the Sun position for safety.
        if(this->insideSunSector(*prediction_result.instant_data, sun_pos))
        {
            tracking_result.status =  PositionStatus::INSIDE_SUN;
            return PositionStatus::INSIDE_SUN;
        }

        tracking_result.status =  PositionStatus::OUTSIDE_SUN;
        return PositionStatus::OUTSIDE_SUN;
    }
}

void PredictorMountSLR::analyzeTracking()
{
    // Results container and auxiliar.
    unsigned step_ms = this->mount_track_.cfg_time_delta;
    PredictorSLR::SLRPredictions results_slr;
    astro::PredictorSun::SunPredictions results_sun;

    // Update flag.
    this->mount_track_.valid_pass = false;

    // Parallel calculation of all SLR positions.
    results_slr = this->predictor_.predict(this->mount_track_.mjd_start, this->mount_track_.sod_start,
                                           this->mount_track_.mjd_end, this->mount_track_.sod_end, step_ms);

    // Check if we have prediction results.
    if(results_slr.empty())
        return;

    // Check that the predictions correspond to a pass.
    auto it = std::find_if(results_slr.begin(), results_slr.end(),
        [](const PredictorSLR::SLRPrediction& pred)
        {
            if(pred.error != PredictorSLR::PredictionError::NO_ERROR &&
               pred.error != PredictorSLR::PredictionError::INTERPOLATION_NOT_IN_THE_MIDDLE)
                    return true;
            else
                return pred.instant_data->el < 0;
        });
    if(it != results_slr.end())
        return;

    // Time transformations with milliseconds precision.
    J2DateTime j2000_start = timing::mjdToJ2000Datetime(this->mount_track_.mjd_start, this->mount_track_.sod_start);
    J2DateTime j2000_end = timing::mjdToJ2000Datetime(this->mount_track_.mjd_end, this->mount_track_.sod_end);

    // Adds time delta to end date to ensure there is enough sun positions
    j2000_end += this->mount_track_.cfg_time_delta / timing::kSecsPerDay;

    // Parallel calculation of all Sun positions.
    results_sun = this->sun_predictor_.fastPredict(j2000_start, j2000_end, step_ms);
    
    // Auxliar container.
    MountSLRPrediction tr;

    // Store all the generated data. At this momment, the
    // tracking positions have not been generated.
    for (std::size_t i = 0; i < results_slr.size(); i++)
    {
        tr.mjd = results_slr[i].instant_data->mjd;
        tr.sod = results_slr[i].instant_data->sod;
        tr.mjdt = results_slr[i].instant_data->mjdt;
        tr.prediction_result = results_slr[i];
        tr.sun_position = results_sun[i].position;
        tr.tracking_position = MountPosition{results_slr[i].instant_data->az, results_slr[i].instant_data->el, 0, 0};
        this->mount_track_.predictions.push_back(std::move(tr));
        tr = {};
    }

    // Now, after positions have been calculated, check each situation.
    this->mount_track_.valid_pass = this->checkTrackingStart();

    // Check the validation at this point.
    if(!this->mount_track_.valid_pass)
        return;

    //this->mount_track_.valid_pass =  this->checkTrackingEnd() &&
    //                                 this->checkTracking();

    // Finally store if a collision exits.
    //this->mount_track_.sun_collision = this->mount_track_.sun_sectors.empty() ||
    //                                   this->mount_track_.sun_collision_at_start ||
    //                                   this->mount_track_.sun_collision_at_end;
}

bool PredictorMountSLR::checkTrackingStart()
{
    // Get the first prediction.
    auto it_pred = this->mount_track_.predictions.begin();

    // Get the first valid position due to elevation
    while (it_pred != this->mount_track_.predictions.end() &&
           it_pred->prediction_result->instant_data->el < this->mount_track_.cfg_min_elev)
    {
        it_pred++;
        it_pred->status = PositionStatus::OUT_OF_TRACK;
    }

    // If the whole tracking has low elevation, return.
    if (it_pred == this->mount_track_.predictions.end())
        return false;

    // 1 - Check Sun collisions if avoiding is disable.
    // 2 - Check Sun collisions if avoiding is enable.
    if(!this->mount_track_.cfg_sun_avoid)
    {
        // Iterate over the predictions. If sun avoid is disabled, check whether position
        // is inside or outside sun security sector without changing start.
        while (it_pred != this->mount_track_.predictions.end())
        {
            if (this->insideSunSector(*it_pred->prediction_result->instant_data, *it_pred->sun_position))
            {
                this->mount_track_.sun_collision = true;
                this->mount_track_.sun_collision_at_start = true;
                it_pred->status = PositionStatus::INSIDE_SUN;
            }
            else
            {
                it_pred->status = PositionStatus::OUTSIDE_SUN;
                break;
            }
            it_pred++;
        }
    }
    else
    {
        // Iterate over the predictions. If sun avoid is enable, move the tracking start to the end
        // of the sun sector if possible.
        while (it_pred != this->mount_track_.predictions.end())
        {
            if(this->insideSunSector(*it_pred->prediction_result->instant_data, *it_pred->sun_position))
            {
                this->mount_track_.sun_collision = true;
                this->mount_track_.sun_collision_at_start = true;
                it_pred->status = PositionStatus::OUT_OF_TRACK;
                it_pred->tracking_position.reset();
            }
            else
            {
                it_pred->status = PositionStatus::OUTSIDE_SUN;
                break;
            }
            it_pred++;
        }
    }

    // If the whole tracking is in the Sun, return.
    if (it_pred == this->mount_track_.predictions.end())
        return false;

    // If start has been moved, store new start.
    if (it_pred != this->mount_track_.predictions.begin())
    {
        this->mount_track_.mjd_start = it_pred->mjd;
        this->mount_track_.sod_start = it_pred->sod;
    }

    // Update the start elevation and the real track begin iterator.
    this->mount_track_.start_elev = it_pred->tracking_position->el;
    this->tracking_begin_ = it_pred;

    // All ok, return true.
    return true;
}

bool PredictorMountSLR::checkTrackingEnd()
{

    auto last = this->mount_track_.predictions.rbegin();

    if (this->mount_track_.cfg_sun_avoid)
    {
        // If sun avoid is activated check if tracking ends inside a sun sector. If that is the case, move the
        // tracking end to the start of the sun sector if possible
        while (last != this->mount_track_.predictions.rend() &&
               this->insideSunSector(*last->prediction_result->instant_data, *last->sun_position))
        {
            // If there is an error or the sun sector covers the whole tracking, then return false, to mark this pass
            // as invalid.
            if (last->prediction_result->error != PredictorSLR::PredictionError::NO_ERROR ||
                last->tracking_position->el < this->mount_track_.cfg_min_elev)
                return false;

            // Set current end as out of track
            last->status = PositionStatus::OUT_OF_TRACK;

            // Advance to previous time position.
            last++;
        }

        // If the whole tracking is at sun, then it is invalid. This should be impossible, since it should have been
        // detected at checkTrackingStart.
        if (last == this->mount_track_.predictions.rend())
            return false;

        if (last != this->mount_track_.predictions.rbegin())
        {
            // If end has been moved, store new end
            this->mount_track_.sun_collision_at_end = true;
            this->mount_track_.mjd_end = last->mjd;
            this->mount_track_.sod_end = last->sod;
        }

        last->status = PositionStatus::OUTSIDE_SUN;
    }
    else
    {
        // If there was an error with prediction at end, or its elevation is below minimum, then the tracking
        // is not valid.
        if (last->prediction_result->error != PredictorSLR::PredictionError::NO_ERROR ||
            last->tracking_position->el < this->mount_track_.cfg_min_elev)
            return false;

        // If sun avoid is disabled, check whether position is inside or outside sun security sector
        // without changing end
        if (this->insideSunSector(*last->prediction_result->instant_data, *last->sun_position))
        {
            last->status = PositionStatus::INSIDE_SUN;
        }
        else
        {
            last->status = PositionStatus::OUTSIDE_SUN;
        }
    }

    this->mount_track_.end_elev = last->tracking_position->el;

    this->tracking_end_ = (last + 1).base();

    return true;
}

bool PredictorMountSLR::checkTracking()
{
    MountSLRPrediction tr;
    bool in_sun_sector = false;
    bool sun_collision = false;
    SunCollisionSector sun_sector;
    MJDate max_elev_mjd = 0;
    SoD max_elev_sod = 0;
    long double max_elev = -1.0L;
    MountSLRPredictions::iterator sun_sector_start;

    // Check the tracking.
    for (auto it = this->tracking_begin_ + 1; it != this->tracking_end_ - 1; it++)
    {

        // If position cannot be predicted for this time, or elevation is below minimum, then tracking is not valid.
        if (it->prediction_result->error != PredictorSLR::PredictionError::NO_ERROR ||
            it->tracking_position->el < this->mount_track_.cfg_min_elev)
            return false;

        // Check if this position is inside sun security sector
        sun_collision = this->insideSunSector(*it->prediction_result->instant_data, *it->sun_position);

        // Store whether position is inside or outside sun. Later, if sun avoid algorithm is applied, those positions
        // which are inside sun will be checked to see if it is possible or not to avoid sun.
        it->status = sun_collision ? PositionStatus::INSIDE_SUN : PositionStatus::OUTSIDE_SUN;


        if (this->mount_track_.cfg_sun_avoid)
        {
            // If sun avoid is applied we have to store the data for each sector  where the tracking goes through a sun security sector.
            // This data will be used for calculating an alternative trajetctory at those sectors.
            if (sun_collision)
            {
                // If there is a sun collision, start saving sun positions for this sector. The first positions is the first position before entering
                // the sector, i.e., it is ouside sun security sector.
                if (!in_sun_sector)
                {
                    in_sun_sector = true;
                    sun_sector_start = it-1;
                    sun_sector.az_entry = sun_sector_start->tracking_position->az;
                    sun_sector.el_entry = sun_sector_start->tracking_position->el;
                    sun_sector.mjdt_entry = sun_sector_start->mjdt;

                }

            }
            else if (!sun_collision && in_sun_sector)
            {
                // If we were inside a sun sector, and we are going out of it, check sun sector rotation direction and positions within the sector and
                // store the sector. The last position stored is the first position after exiting the sector, i.e., it is outside sun security sector.
                in_sun_sector = false;
                sun_sector.az_exit = it->tracking_position->az;
                sun_sector.el_exit = it->tracking_position->el;
                sun_sector.mjdt_exit = it->mjdt;
                // If sector has no valid rotation direction, mark tracking as not valid, since sun cannot be avoided.
                if (!this->setSunSectorRotationDirection(sun_sector, sun_sector_start, it))
                    return false;
                this->checkSunSectorPositions(sun_sector, sun_sector_start, it);
                this->mount_track_.sun_sectors.push_back(std::move(sun_sector));
                sun_sector = {};
            }
        }

        if (it->tracking_position->el > max_elev)
        {
            max_elev = it->tracking_position->el;
            max_elev_mjd = it->mjd;
            max_elev_sod = it->sod;
        }

    }

    // If there was a sun security sector until the position previous to the last one, finish the tracking before
    // starting the sector. This case should be almost impossible.
    if (in_sun_sector)
    {
        timing::MjdtToMjdAndSecs(sun_sector.mjdt_entry, this->mount_track_.mjd_end, this->mount_track_.sod_end);
        for (auto it = sun_sector_start + 1; it !=this->mount_track_.predictions.end(); it++)
            it->status = PositionStatus::OUT_OF_TRACK;
        this->tracking_end_ = sun_sector_start;
        this->mount_track_.sun_collision_at_end = true;
    }

    this->mount_track_.max_elev = max_elev;
    this->mount_track_.mjd_max_elev = max_elev_mjd;
    this->mount_track_.sod_max_elev = max_elev_sod;

    return true;
}

bool PredictorMountSLR::insideSunSector(const InstantData& pos, const SunPosition& sun_pos) const
{
    // Store the avoid angle.
    long double avoid_angle = static_cast<long double>(this->mount_track_.cfg_sun_avoid_angle);

    if (sun_pos.el < -avoid_angle)
    {
        // If sun security sector is below horizon, return false
        return false;
    }
    else if (sun_pos.el > -avoid_angle && sun_pos.el < 0.L)
    {
        // If sun is below horizon, but the security sector is above, calculate distance as cartesian points.
        long double diff_az = pos.az - sun_pos.az;
        // If azimuth difference is greater than 180, then take the shorter way
        if (diff_az > 180.L)
            diff_az = 360.L - diff_az;
        long double diff_el = pos.el - sun_pos.el;
        return std::sqrt(diff_az * diff_az + diff_el * diff_el) < avoid_angle;
    }
    else
    {
        // If sun is above the horizon, calculate the distance between polar coordinates.
        // We have to convert elevation to zenith angle, since the 0 must be at zenith.
        // We also have to convert angle from north to east direction (0 at north, increases cw)
        // to goniometric (0 is at east increases ccw)

        long double zenith = 90.L - pos.el;
        long double zenith_sun = 90.L - sun_pos.el;
        long double az_gon = 360 - (pos.az - 90.L);
        if (az_gon >= 360.L)
            az_gon -= 360.L;
        long double sun_az_gon = 360 - (sun_pos.az - 90.L);
        if (sun_az_gon >= 360.L)
            sun_az_gon -= 360.L;

        long double diff_angles = (az_gon - sun_az_gon) * math::kPi / 180.L;

        return std::sqrt(zenith * zenith + zenith_sun * zenith_sun - 2.L*zenith*zenith_sun*std::cos(diff_angles)) <
               avoid_angle;
    }


}

bool PredictorMountSLR::setSunSectorRotationDirection(
    SunCollisionSector &sector, MountSLRPredictions::const_iterator sun_start, MountSLRPredictions::const_iterator sun_end)
{
    long double sec_delta = static_cast<long double>(this->mount_track_.cfg_time_delta)/1000.0L;
    long double avoid_angle = static_cast<long double>(this->mount_track_.cfg_sun_avoid_angle);
    MJDateTime mjdt = sector.mjdt_entry + sec_delta / static_cast<long double>(timing::kSecsPerDay);
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
        long double time_perc = (mjdt - sector.mjdt_entry) / (sector.mjdt_exit - sector.mjdt_entry);

        long double entry_angle = std::atan2(sector.el_entry - it->sun_position->el,
                                             sector.az_entry - it->sun_position->az);

        long double exit_angle = std::atan2(sector.el_exit - it->sun_position->el,
                                            sector.az_exit - it->sun_position->az);

        long double cw_angle;
        long double ccw_angle;

        if (exit_angle > entry_angle)
        {

            ccw_angle = entry_angle + time_perc * (exit_angle - entry_angle);
            cw_angle = entry_angle - time_perc * (2 * math::kPi - exit_angle + entry_angle);
            if (cw_angle < 0.L)
                cw_angle += 2 * math::kPi;
        }
        else
        {
            cw_angle = entry_angle - time_perc * (entry_angle - exit_angle);
            ccw_angle = entry_angle + time_perc * (2 * dpslr::math::kPi - entry_angle + exit_angle);
            if (ccw_angle >= 2 * math::kPi)
                ccw_angle -= 2 * math::kPi;
        }


        long double elev_cw = it->sun_position->el + avoid_angle * std::sin(cw_angle);
        long double elev_ccw = it->sun_position->el + avoid_angle * std::sin(ccw_angle);


        if (elev_cw > 90.L || elev_cw < this->mount_track_.cfg_min_elev)
            valid_cw = false;

        if (elev_ccw > 90.L || elev_ccw < this->mount_track_.cfg_min_elev)
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
        long double entry_angle = std::atan2(sector.el_entry - sun_start->sun_position->el,
                                             sector.az_entry - sun_start->sun_position->az);

        long double exit_angle = std::atan2(sector.el_exit - sun_end->sun_position->el,
                                            sector.az_exit - sun_end->sun_position->az);

        long double cw_angle;
        long double ccw_angle;

        if (exit_angle > entry_angle)
        {
            ccw_angle = exit_angle - entry_angle;
            cw_angle = 2 * math::kPi - exit_angle + entry_angle;
        }
        else
        {
            cw_angle = entry_angle - exit_angle;
            ccw_angle = 2 * math::kPi - entry_angle + exit_angle;
        }

        sector.cw = (cw_angle < ccw_angle) ? PredictorMountSLR::RotationDirection::CLOCKWISE :
                        PredictorMountSLR::RotationDirection::COUNTERCLOCKWISE;
    }

    return true;
}

void PredictorMountSLR::checkSunSectorPositions(
    const SunCollisionSector &sector, MountSLRPredictions::iterator sun_start, MountSLRPredictions::iterator sun_end)
{
    // Store the avoid angle.
    long double cfg_avoid_angle = static_cast<long double>(this->mount_track_.cfg_sun_avoid_angle);
    // Check positions within sun sector. First and last are excluded, since they are outside sun sector
    for (auto it = sun_start + 1; it != sun_end; it++)
    {
        long double angle_avoid = this->calcSunAvoidTrajectory(it->mjdt, sector, *it->sun_position);
        it->tracking_position->az = it->sun_position->az + cfg_avoid_angle * std::cos(angle_avoid);
        it->tracking_position->el = it->sun_position->el + cfg_avoid_angle * std::sin(angle_avoid);
        it->tracking_position->diff_az = it->prediction_result->instant_data->az - it->tracking_position->az;
        it->tracking_position->diff_el = it->prediction_result->instant_data->el - it->tracking_position->el;
        it->status = PositionStatus::AVOIDING_SUN;
    }
}

long double PredictorMountSLR::calcSunAvoidTrajectory(MJDateTime mjdt, const SunCollisionSector &sector,
                                                SunPosition &sun_pos)
{
    long double time_perc = (mjdt - sector.mjdt_entry) / (sector.mjdt_exit - sector.mjdt_entry);

    long double entry_angle = std::atan2(sector.el_entry - sun_pos.el,
                                         sector.az_entry - sun_pos.az);

    long double exit_angle = std::atan2(sector.el_exit - sun_pos.el,
                                        sector.az_exit - sun_pos.az);

    long double angle;

    if (exit_angle > entry_angle)
    {
        if (sector.cw == RotationDirection::CLOCKWISE)
        {
            angle = entry_angle - time_perc * (2 * math::kPi - exit_angle + entry_angle);
            if (angle < 0.L)
                angle += 2 * math::kPi;
        }
        else
            angle = entry_angle + time_perc * (exit_angle - entry_angle);

    }
    else
    {
        if (sector.cw == RotationDirection::CLOCKWISE)
        {
            angle = entry_angle - time_perc * (entry_angle - exit_angle);
        }
        else
        {
            angle = entry_angle + time_perc * (2 * math::kPi - entry_angle + exit_angle);
            if (angle >= 2 * math::kPi)
                angle -= 2 * math::kPi;
        }
    }

    return angle;
}

PredictorMountSLR::MountTrackSLR::MountTrackSLR(const CPF &cpf, const PredictorSLR &predictor_slr,
                                                const PredictorSun &predictor_sun) :
    mjd_start(0),
    sod_start(0),
    mjd_end(0),
    sod_end(0),
    mjd_max_elev(0),
    sod_max_elev(0),
    start_elev(0),
    end_elev(0),
    max_elev(0),
    valid_pass(false),
    sun_collision(false),
    sun_collision_at_start(false),
    sun_collision_at_end(false),
    cfg_sun_avoid(false),
    cfg_time_delta(0),
    cfg_sun_avoid_angle(0),
    cfg_min_elev(0),
    cpf(cpf),
    predictor_slr(predictor_slr),
    predictor_sun(predictor_sun)
{}


}} // END NAMESPACES
// =====================================================================================================================
