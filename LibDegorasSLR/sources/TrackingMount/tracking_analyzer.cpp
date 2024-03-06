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
 * @file tracking_analyzer.cpp
 * @author Degoras Project Team.
 * @brief This file contains the implementation of the class TrackingAnalyzer.
 * @copyright EUPL License
 * @version 2306.1
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/TrackingMount/types/tracking_analyzer.h"
#include "LibDegorasSLR/libdegorasslr_init.h"
// =====================================================================================================================

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using dpslr::mount::TrackingAnalyzer;
// ---------------------------------------------------------------------------------------------------------------------

TrackingAnalyzer::TrackingAnalyzer(const TrackingConfig &config) :

    config_(config)
{
    // Check Degoras initialization.
    DegorasInit::checkMandatoryInit();

    // Check configured elevations.
    if(config.min_elev >= config.max_elev || config.min_elev > 90 || config.max_elev > 90 || config.sun_avoid_angle > 90)
        throw std::invalid_argument("[LibDegorasSLR,TrackingMount,TrackingAnalyzer] Invalid angles configuration.");

    // Check too high values for the sun avoid angle, so the algorithm can fail.
    if((config.sun_avoid_angle*2) + config.min_elev >= 90 || (config.sun_avoid_angle*2)+(90 - config.max_elev) >= 90)
        throw std::invalid_argument("[LibDegorasSLR,TrackingMount,TrackingAnalyzer] Sun avoid angle too high for the "
                                    "configured minimum and maximum elevations.");
}

const TrackingPredictionV &TrackingAnalyzer::getPredictions() const
{
    return this->predictions_;
}

const TrackingInfo &TrackingAnalyzer::getTrackingInfo() const
{
    return this->track_info_;
}

TrackingPredictionV::const_iterator TrackingAnalyzer::trackingBegin() const
{
    return this->begin_;
}

TrackingPredictionV::const_iterator TrackingAnalyzer::trackingEnd() const
{
    return this->end_;
}

void TrackingAnalyzer::analyzePrediction(TrackingPrediction &pred) const
{
    // Check if requested position is inside valid tracking time. Otherwise return out of tracking error.
    if (pred.mjdt < this->config_.mjdt_start || pred.mjdt > this->config_.mjdt_end)
    {
        pred.status =  PositionStatus::OUT_OF_TRACK;
    }

    bool inside_sun = this->insideSunSector(pred.pos.altaz_coord, pred.sun_pred.altaz_coord);


    // If sun avoidance is applied and position for requested time is inside a sun security sector, map the point
    // to its corresponding point in the sun security sector circumference.
    // Otherwise, return calculated position.
    if (this->config_.sun_avoid )
    {
        auto sector_it = std::find_if(this->sun_sectors_.begin(), this->sun_sectors_.end(),
        [&mjdt = pred.mjdt](const auto& sector)
        {
            return mjdt > sector.mjdt_entry && mjdt < sector.mjdt_exit;
        });

        if (sector_it == this->sun_sectors_.end())
        {
            pred.status =  PositionStatus::CANT_AVOID_SUN;
            return;
        }

        // Calculate new position avoiding sun
        this->calcSunAvoidPos(pred, *sector_it);


        // Return the status.
        pred.status =  PositionStatus::AVOIDING_SUN;
    }
    else
    {
        // Set the difference in az, el to 0, since there is no sun deviation.
        pred.pos.diff_az = 0;
        pred.pos.diff_el = 0;

        // Check the Sun position for safety.
        if(inside_sun)
        {
            pred.status =  PositionStatus::INSIDE_SUN;
        }
        else
        {
            pred.status =  PositionStatus::OUTSIDE_SUN;
        }
    }
}


void TrackingAnalyzer::analyzeTracking(const TrackingPredictionV &predictions)
{
    // Update flag.
    this->track_info_.valid_pass = false;

    // Check if we have prediction results.
    if(predictions.empty())
        return;

    // Check that the predictions correspond to a pass.
    // Tracking is not valid if there are error or below 0. Minimum elevation will be checked afterwards.
    auto it = std::find_if(predictions.begin(), predictions.end(),
        [](const TrackingPrediction& pred)
        {
            return pred.pos.altaz_coord.el < 0;
        });
    if(it != predictions.end())
        return;
    // --------------------------------------------------------------

    this->track_info_.mjdt_start = this->config_.mjdt_start;
    this->track_info_.mjdt_end = this->config_.mjdt_end;
    this->track_info_.start_coord = predictions.front().pos.altaz_coord;
    this->track_info_.end_coord = predictions.back().pos.altaz_coord;
    
    // Store all the generated data. At this momment, the
    // tracking positions have not been generated.
    this->predictions_ = predictions;

    // Now, after positions have been calculated, check each situation.

    // Check the start and validate at this point.
    if (!(this->track_info_.valid_pass = this->analyzeTrackingStart()))
        return;

    // Check the end and validate at this point.
    if (!(this->track_info_.valid_pass = this->analyzeTrackingEnd()))
        return;

    // Check the middle and validate at this point.
    if (!(this->track_info_.valid_pass = this->analyzeTrackingMiddle()))
        return;

    // Finally store if a collision exits.

}

bool TrackingAnalyzer::analyzeTrackingStart()
{
    // Get the first prediction and min and max elevations.
    auto it_pred = this->predictions_.begin();
    const long double min_el = static_cast<long double>(this->config_.min_elev);
    const long double max_el = static_cast<long double>(this->config_.max_elev);

    // Get the first valid position due to minimum and maximum elevations.
    while (it_pred != this->predictions_.end() &&
           it_pred->pos.altaz_coord.el < min_el &&
           it_pred->pos.altaz_coord.el > max_el)
    {
        it_pred->status = PositionStatus::OUT_OF_TRACK;
        it_pred++;
    }

    // If the whole tracking has incorrect elevation, return.
    if (it_pred == this->predictions_.end())
        return false;

    // 1 - Check Sun collisions if avoiding is disable.
    // 2 - Check Sun collisions if avoiding is enable.
    if(!this->config_.sun_avoid)
    {
        // Iterate over the predictions. If sun avoid is disabled, check whether position
        // is inside or outside sun security sector without changing start.
        while (it_pred != this->predictions_.end())
        {
            if (this->insideSunSector(it_pred->pos.altaz_coord, it_pred->sun_pred.altaz_coord))
            {
                this->track_info_.sun_collision = true;
                this->track_info_.sun_collision_at_start = true;
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
        while (it_pred != this->predictions_.end())
        {
            if(this->insideSunSector(it_pred->pos.altaz_coord, it_pred->sun_pred.altaz_coord))
            {
                this->track_info_.sun_collision = true;
                this->track_info_.sun_collision_at_start = true;
                it_pred->status = PositionStatus::OUT_OF_TRACK;
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
    if (it_pred == this->predictions_.end())
        return false;

    // If start has been moved, store new start.
    if (it_pred != this->predictions_.begin())
    {
        this->track_info_.trim_at_start = true;
        this->track_info_.mjdt_start = it_pred->mjdt;
    }

    // Update the start position.
    this->track_info_.start_coord = it_pred->pos.altaz_coord;
    this->begin_ = it_pred;

    // All ok, return true.
    return true;
}

bool TrackingAnalyzer::analyzeTrackingEnd()
{
    // Get the first prediction and min and max elevations.
    auto it_pred = this->predictions_.rbegin();
    const long double min_el = static_cast<long double>(this->config_.min_elev);
    const long double max_el = static_cast<long double>(this->config_.max_elev);

    // Get the first valid position due to minimum and maximum elevations.
    while (it_pred != this->predictions_.rend() &&
           it_pred->pos.altaz_coord.el < min_el &&
           it_pred->pos.altaz_coord.el > max_el)
    {
        it_pred->status = PositionStatus::OUT_OF_TRACK;
        it_pred++;
    }

    // If the whole tracking has low elevation, return.
    if (it_pred == this->predictions_.rend())
        return false;

    // 1 - Check Sun collisions if avoiding is disable.
    // 2 - Check Sun collisions if avoiding is enable.
    if (!this->config_.sun_avoid)
    {
        // Iterate over the predictions. If sun avoid is disabled, check whether position
        // is inside or outside sun security sector without changing end.
        while (it_pred != this->predictions_.rend())
        {
            if (this->insideSunSector(it_pred->pos.altaz_coord, it_pred->sun_pred.altaz_coord))
            {
                this->track_info_.sun_collision = true;
                this->track_info_.sun_collision_at_end = true;
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
        while (it_pred != this->predictions_.rend())
        {
            if(this->insideSunSector(it_pred->pos.altaz_coord, it_pred->sun_pred.altaz_coord))
            {
                this->track_info_.sun_collision = true;
                this->track_info_.sun_collision_at_end = true;
                it_pred->status = PositionStatus::OUT_OF_TRACK;
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
    if (it_pred == this->predictions_.rend())
        return false;

    // If end has been moved, store new end.
    if (it_pred != this->predictions_.rbegin())
    {
        this->track_info_.trim_at_end = true;
        this->track_info_.mjdt_end = it_pred->mjdt;
    }

    // Update the end elevation and the real track end iterator.
    this->track_info_.end_coord = it_pred->pos.altaz_coord;
    // TODO: check this:
    this->end_ = it_pred.base();

    // All ok, return true.
    return true;
}

bool TrackingAnalyzer::analyzeTrackingMiddle()
{
    //MountSLRPrediction tr;
    bool in_sun_sector = false;
    bool inside_sun = false;
    bool sun_collision = false;
    SunCollisionSector sun_sector;
    MJDateTime max_elev_mjdt = 0;
    long double max_elev = -1.0L;
    const long double cfg_max_el = static_cast<long double>(this->config_.max_elev);
    const long double sun_avoid_angle = static_cast<long double>(this->config_.sun_avoid_angle);
    TrackingPredictionV::iterator sun_sector_start;

    // Check the tracking maximum altitudes. Positions that trespasses will be clipped to maximum elevation.
    for (auto it = this->begin_; it != this->end_; it++)
    {
        if(it->pos.altaz_coord.el > cfg_max_el)
        {
            it->pos.altaz_coord.el = cfg_max_el;
            this->track_info_.el_deviation = true;
            this->track_info_.max_el = cfg_max_el;
        }
    }

    // Check the tracking Sun collision.
    for (auto it = this->begin_; it != this->end_; it++)
    {
        // Check if this position is inside sun security sector and store it.
        inside_sun = this->insideSunSector(it->pos.altaz_coord, it->sun_pred.altaz_coord);

        // Update sun collision variable.
        sun_collision |= inside_sun;

        // Store whether position is inside or outside sun. Later, if sun avoid algorithm is applied, those positions
        // which are inside sun will be checked to see if it is possible or not to avoid sun.
        it->status = inside_sun ? PositionStatus::INSIDE_SUN : PositionStatus::OUTSIDE_SUN;

        if (this->config_.sun_avoid)
        {
            // Check if the Sun is too high.
            bool sun_high = (it->sun_pred.altaz_coord.el + sun_avoid_angle) >= cfg_max_el;

            // // Update maximum elevations if the Sun is too high. In this case, the maximum el
            if(inside_sun && sun_high)
            {
                long double limit_el = it->sun_pred.altaz_coord.el - sun_avoid_angle;
                limit_el = (limit_el < cfg_max_el) ? limit_el : cfg_max_el;

                for (auto it = this->begin_; it != this->end_; it++)
                {
                    if(it->pos.altaz_coord.el >= limit_el)
                    {
                        it->pos.altaz_coord.el = limit_el;
                        it->status = PositionStatus::AVOIDING_SUN;

                       // Update sun deviation flag.
                       this->track_info_.sun_deviation = true;
                    }
                }
                break;
            }

            // If sun avoid is applied we have to store the data for each sector  where the tracking goes through a sun
            // security sector.This data will be used for calculating an alternative trajetctory at those sectors.
            else if (inside_sun)
            {
                // If there is a sun collision, start saving sun positions for this sector.
                // The first position is the first position before entering
                // the sector, i.e., it is ouside sun security sector.
                if (!in_sun_sector)
                {
                    in_sun_sector = true;
                    sun_sector_start = it-1;
                    sun_sector.altaz_entry = sun_sector_start->pos.altaz_coord;
                    sun_sector.mjdt_entry = sun_sector_start->mjdt;

                }
            }
            else if (!inside_sun && in_sun_sector)
            {
                // If we were inside a sun sector, and we are going out of it,
                // check sun sector rotation direction and positions within the sector and store the sector.
                // The last position stored is the first position after exiting the sector,
                // i.e., it is outside sun security sector.
                in_sun_sector = false;
                sun_sector.altaz_exit = it->pos.altaz_coord;
                sun_sector.mjdt_exit = it->mjdt;
                // If sector has no valid rotation direction, mark tracking as not valid, since sun cannot be avoided.
                if (!this->setSunSectorRotationDirection(sun_sector, sun_sector_start, it))
                    return false;
                this->checkSunSectorPositions(sun_sector, sun_sector_start, it);
                this->sun_sectors_.push_back(std::move(sun_sector));
                sun_sector = {};
            }
        }

        if (it->pos.altaz_coord.el > max_elev)
        {
            max_elev = it->pos.altaz_coord.el;
            max_elev_mjdt = it->mjdt;
        }

    }

    // Finally update the track information.
    this->track_info_.max_el = max_elev;
    this->track_info_.sun_collision |= sun_collision;
    this->track_info_.sun_collision_at_middle = sun_collision;

    // All ok, return true.
    return true;
}

bool TrackingAnalyzer::insideSunSector(const AltAzPos& pass_pos, const AltAzPos& sun_pos) const
{

    long double diff_az = pass_pos.az - sun_pos.az;
    // If azimuth difference is greater than 180, then take the shorter way
    if (diff_az > 180.L)
        diff_az = 360.L - diff_az;
    long double diff_el = pass_pos.el - sun_pos.el;
    return std::sqrt(diff_az * diff_az + diff_el * diff_el) < this->config_.sun_avoid_angle;
}

bool TrackingAnalyzer::setSunSectorRotationDirection(SunCollisionSector &sector,
                                                     TrackingPredictionV::const_iterator sun_start,
                                                     TrackingPredictionV::const_iterator sun_end)
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

        long double diff_az_entry = sector.altaz_entry.az - it->sun_pred.altaz_coord.az;
        if (diff_az_entry > 180.L)
        {
            diff_az_entry = -(360.L - diff_az_entry);
        }
        else if (diff_az_entry < -180.L)
        {
            diff_az_entry = 360.L + diff_az_entry;
        }

        long double diff_az_exit = sector.altaz_exit.az - it->sun_pred.altaz_coord.az;
        if (diff_az_exit > 180.L)
        {
            diff_az_exit = -(360.L - diff_az_exit);
        }
        else if (diff_az_exit < -180.L)
        {
            diff_az_exit = 360.L + diff_az_exit;
        }
        long double entry_angle = std::atan2(sector.altaz_entry.el - it->sun_pred.altaz_coord.el, diff_az_entry);

        long double exit_angle = std::atan2(sector.altaz_exit.el - it->sun_pred.altaz_coord.el, diff_az_exit);

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

        long double elev_cw = it->sun_pred.altaz_coord.el +
                              this->config_.sun_avoid_angle * std::sin(cw_angle);
        long double elev_ccw = it->sun_pred.altaz_coord.el +
                               this->config_.sun_avoid_angle * std::sin(ccw_angle);

        long double cfg_max_el = static_cast<long double>(this->config_.max_elev);

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
        sector.cw = SunCollisionSector::RotationDirection::COUNTERCLOCKWISE;
    else if (!valid_ccw)
        sector.cw = SunCollisionSector::RotationDirection::CLOCKWISE;
    else
    {
        // Calculate angle between start and end to see which is shorter.
        long double diff_az_entry = sector.altaz_entry.az - sun_start->sun_pred.altaz_coord.az;
        if (diff_az_entry > 180.L)
        {
            diff_az_entry = -(360.L - diff_az_entry);
        }
        else if (diff_az_entry < -180.L)
        {
            diff_az_entry = 360.L + diff_az_entry;
        }

        long double diff_az_exit = sector.altaz_exit.az - sun_end->sun_pred.altaz_coord.az;
        if (diff_az_exit > 180.L)
        {
            diff_az_exit = -(360.L - diff_az_exit);
        }
        else if (diff_az_exit < -180.L)
        {
            diff_az_exit = 360.L + diff_az_exit;
        }

        long double entry_angle = std::atan2(sector.altaz_entry.el - sun_start->sun_pred.altaz_coord.el, diff_az_entry);
        long double exit_angle = std::atan2(sector.altaz_exit.el - sun_end->sun_pred.altaz_coord.el, diff_az_exit);

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
                        SunCollisionSector::RotationDirection::CLOCKWISE :
                        SunCollisionSector::RotationDirection::COUNTERCLOCKWISE;

    }

    return true;
}

void TrackingAnalyzer::checkSunSectorPositions(const SunCollisionSector &sector,
                                               TrackingPredictionV::iterator sun_start,
                                               TrackingPredictionV::iterator sun_end)
{
    // Check positions within sun sector. First and last are excluded, since they are outside sun sector
    for (auto it = sun_start + 1; it != sun_end; it++)
    {
        // Calculate the avoiding position using the avoid angle and distance as radius. Store the difference between
        // original position and new position
        this->calcSunAvoidPos(*it, sector);
        it->status = PositionStatus::AVOIDING_SUN;

        // Update sun deviation flag.
        this->track_info_.sun_deviation = true;
    }
}

long double TrackingAnalyzer::calcSunAvoidTrajectory(const MJDateTime &mjdt,
                                                     const SunCollisionSector& sector,
                                                     const AltAzPos& sun_pos) const
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
        if (sector.cw == SunCollisionSector::RotationDirection::COUNTERCLOCKWISE)
            angle = -(2*math::kPi - angle);
    }
    else
    {
        if (sector.cw == SunCollisionSector::RotationDirection::CLOCKWISE)
            angle = (2*math::kPi + angle);
    }

    return entry_angle + angle * time_perc;
}

void TrackingAnalyzer::calcSunAvoidPos(TrackingPrediction &pred,
                                       const SunCollisionSector &sector) const
{
    // Get the trajectory avoid angle.
    long double angle_avoid = this->calcSunAvoidTrajectory(pred.mjdt, sector, pred.sun_pred.altaz_coord);
    // Calculate new azimuth and elevation using trajectory avoid angle
    Degrees new_az = pred.sun_pred.altaz_coord.az + this->config_.sun_avoid_angle * std::cos(angle_avoid);
    // Normalize new azimuth
    if (new_az < 0.L)
        new_az += 360.L;
    if (new_az > 360.L)
        new_az -= 360.L;

    Degrees new_el = pred.sun_pred.altaz_coord.el + this->config_.sun_avoid_angle * std::sin(angle_avoid);

    // Store difference between original position and new position
    pred.pos.diff_az = pred.pos.altaz_coord.az - new_az;
    pred.pos.diff_el = pred.pos.altaz_coord.el - new_el;

    // Store new position
    pred.pos.altaz_coord.az = new_az;
    pred.pos.altaz_coord.el = new_el;

}





}} // END NAMESPACES
// =====================================================================================================================
