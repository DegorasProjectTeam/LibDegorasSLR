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
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/TrackingMount/utils/tracking_analyzer/tracking_analyzer.h"
#include "LibDegorasSLR/Timing/time_utils.h"
// =====================================================================================================================

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
namespace utils{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace math::units;
using namespace timing;
using namespace timing::types;
using namespace astro::predictors;
using namespace mount::types;
// ---------------------------------------------------------------------------------------------------------------------

TrackingAnalyzer::TrackingAnalyzer(const TrackingAnalyzerConfig& config) :
    config_(config)
{
    // Check configured elevations.
    if(config.min_elev >= config.max_elev || config.min_elev > 90 ||
        config.max_elev > 90 || config.sun_avoid_angle > 90)
    {
        std::string submodule("[LibDegorasSLR,TrackingMount,TrackingAnalyzer]");
        std::string error("Invalid angles configuration.");
        throw std::invalid_argument(submodule + " " + error);
    }

    // Check too high values for the sun avoid angle, so the algorithm can fail.
    if((config.sun_avoid_angle*2) + config.min_elev >= 90 || (config.sun_avoid_angle*2)+(90 - config.max_elev) >= 90)
    {
        std::string submodule("[LibDegorasSLR,TrackingMount,TrackingAnalyzer]");
        std::string error("Sun avoid angle too high for the configured minimum and maximum elevations.");
        throw std::invalid_argument(submodule + " " + error);
    }
}

TrackingAnalysis TrackingAnalyzer::analyzeTracking(const MountPositionV& mount_positions,
                                                   const astro::types::SunPositionV& sun_positions) const
{
    // Check if we have data.
    if(mount_positions.empty() || sun_positions.empty())
    {
        std::string submodule("[LibDegorasSLR,TrackingMount,TrackingAnalyzer,analyzeTracking]");
        std::string error("Empty input data.");
        throw std::invalid_argument(submodule + " " + error);
    }

    // Check if sizes of mount_positions and sun_predictions are the same.
    if (mount_positions.size() != sun_positions.size())
    {
        std::string submodule("[LibDegorasSLR,TrackingMount,TrackingAnalyzer,analyzeTracking]");
        std::string error("Inconsistent data sizes between mount positions and sun predictions.");
        throw std::invalid_argument(submodule + " " + error);
    }

    // Check if Modified Julian Dates match for each corresponding position and prediction.
    for (size_t i = 0; i < mount_positions.size(); ++i)
    {
        if (modifiedJulianDateToJ2000DateTime(mount_positions[i].mjdt) != sun_positions[i].j2dt)
        {
            std::string submodule("[LibDegorasSLR,TrackingMount,TrackingAnalyzer,analyzeTracking]");
            std::string error("Mismatch between mount and sun times at index: " + std::to_string(i) + ".");
            throw std::invalid_argument(submodule + " " + error);
        }
    }

    // Check that the mount elevation positions are valid (>= 0).
    auto it = std::find_if(mount_positions.begin(), mount_positions.end(),
                    [](const MountPosition& pos){return math::isFloatingMinorThanZero(pos.altaz_coord.el);});
    if(it != mount_positions.end())
    {
        std::string submodule("[LibDegorasSLR,TrackingMount,TrackingAnalyzer,analyzeTracking]");
        std::string error("The mount elevation positions are invalid (minor than 0).");
        throw std::invalid_argument(submodule + " " + error);
    }

    // --------------------------------------------------------------

    // Result container.
    TrackingAnalysis track_analysis;

    // Store initial times and coords. This data could change during the analysis.
    track_analysis.mjdt_start = mount_positions.front().mjdt;
    track_analysis.mjdt_end = mount_positions.back().mjdt;
    track_analysis.start_coord = mount_positions.front().altaz_coord;
    track_analysis.end_coord = mount_positions.back().altaz_coord;

    // Store all the mount positions data. At this momment, the tracking positions are equal to mount positions.
    for(const auto& pos : mount_positions)
        track_analysis.track_positions.push_back(pos);

    // Store all the Sun positions data. This will not change.
    for(const auto& pos : sun_positions)
        track_analysis.sun_positions.push_back(pos);

    // Now, after positions have been calculated, check each situation.

    // Check the start and validate at this point.
    if (!this->analyzeTrackingStart(track_analysis))
    {
        track_analysis.empty_track = true;
        return track_analysis;
    }
    // Check the end and validate at this point.
    if (!this->analyzeTrackingEnd(track_analysis))
    {
        track_analysis.empty_track = true;
        return track_analysis;
    }

    // Check the middle and validate at this point.
    if (!this->analyzeTrackingMiddle(track_analysis))
        track_analysis.empty_track = true;

    // Return the track analysis.
    return track_analysis;
}

MountPositionAnalyzed TrackingAnalyzer::analyzePosition(const TrackingAnalysis& analysis,
                                                        const types::MountPosition& mount_pos,
                                                        const astro::types::SunPosition& sun_pos) const
{
    // Store the position. This data could change during the analysis.
    MountPositionAnalyzed analyzed_pos(mount_pos);

    // Check if requested position is inside valid tracking time. Otherwise return out of tracking error.
    if (mount_pos.mjdt < analysis.mjdt_start || mount_pos.mjdt > analysis.mjdt_end)
    {
        analyzed_pos.status =  AnalyzedPositionStatus::OUT_OF_TRACK;
        return analyzed_pos;
    }

    bool inside_sun = this->insideSunSector(pred.pos.altaz_coord, pred.sun_pred.altaz_coord);

    // If sun avoidance is applied and position for requested time is inside a sun security sector, map the point
    // to its corresponding point in the sun security sector circumference.
    // Otherwise, return calculated position.
    if (this->config_.sun_avoid )
    {
        // If sun avoidance is applied
        if (this->track_info_.sun_collision_high)
        {
            // If there was a collision with a high sun sector
            if (pred.pos.altaz_coord.el > this->track_info_.max_el)
            {
                // If the elevation of this position is greater than the maximum,
                // clip the elevation to the maximum. Position is avoiding high sun sector.
                pred.pos.altaz_coord.el = this->track_info_.max_el;
                pred.status = PositionStatus::AVOIDING_SUN;
            }
            else
            {
                // Otherwise return position as is. It is outside sun sector.
                pred.status = PositionStatus::OUTSIDE_SUN;
            }
        }
        else if (inside_sun)
        {
            // If this position is inside a sun security sector then calculate
            // the avoiding trajectory position if it is possible.
            auto sector_it = std::find_if(this->sun_sectors_.begin(), this->sun_sectors_.end(),
            [&mjdt = pred.mjdt](const auto& sector)
            {
                return mjdt > sector.mjdt_entry && mjdt < sector.mjdt_exit;
            });

            if (sector_it == this->sun_sectors_.end())
            {
                // Position is inside sun security sector, but no sector was found. It is impossible
                // to avoid sun. This should not happen.
                pred.status =  PositionStatus::CANT_AVOID_SUN;
            }
            else
            {
                // Calculate new position avoiding sun
                this->calcSunAvoidPos(pred, *sector_it);

                // Return the status.
                pred.status = PositionStatus::AVOIDING_SUN;
            }
        }
        else
        {
            // If this position is not within the previous cases, then it is outside any sun security sector.
            pred.status = PositionStatus::OUTSIDE_SUN;
        }
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

    // TODO: position status for clipped elevation? See high sun sector todo.
    if (pred.pos.altaz_coord.el > this->config_.max_elev)
    {
        pred.pos.altaz_coord.el = static_cast<long double>(this->config_.max_elev);
    }
}



bool TrackingAnalyzer::analyzeTrackingStart(TrackingAnalysis& analysis) const
{
    // Get the first prediction and min and max elevations.
    auto it_pred = this->predictions_.begin();
    const long double min_el = static_cast<long double>(this->config_.min_elev);
    const long double max_el = static_cast<long double>(this->config_.max_elev);

    // Get the first valid position due to minimum and maximum elevations.
    while (it_pred != this->predictions_.end() &&
            (it_pred->pos.altaz_coord.el < min_el || it_pred->pos.altaz_coord.el > max_el))
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

bool TrackingAnalyzer::analyzeTrackingEnd(TrackingAnalysis& analysis) const
{
    // Get the first prediction and min and max elevations.
    auto it_pred = this->predictions_.rbegin();
    const long double min_el = static_cast<long double>(this->config_.min_elev);
    const long double max_el = static_cast<long double>(this->config_.max_elev);

    // Get the first valid position due to minimum and maximum elevations.
    while (it_pred != this->predictions_.rend() &&
          (it_pred->pos.altaz_coord.el < min_el || it_pred->pos.altaz_coord.el > max_el))
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

bool TrackingAnalyzer::analyzeTrackingMiddle(TrackingAnalysis &analysis) const
{
    //MountPredictionSLR tr;
    bool in_sun_sector = false;
    bool inside_sun = false;
    bool sun_collision = false;
    SunCollisionSector sun_sector;
    MJDateTime max_elev_mjdt = 0;
    long double max_elev = -1.0L;
    const long double cfg_max_el = static_cast<long double>(this->config_.max_elev);
    long double sun_avoid_angle = static_cast<long double>(this->config_.sun_avoid_angle) + kAvoidAngleOffset;
    TrackingPredictionV::iterator sun_sector_start;

    // Check the tracking maximum altitudes. Positions that trespasses will be clipped to maximum elevation.
    for (auto it = this->begin_; it != this->end_; it++)
    {
        if(it->pos.altaz_coord.el > cfg_max_el)
        {
            it->pos.altaz_coord.el = cfg_max_el;
            it->status = PositionStatus::ELEVATION_CLIPPED;
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
                bool stop = false;
                auto sun_min_el_it = std::min_element(this->begin_, this->end_,
                    [](const auto& a, const auto& b){return a.sun_pred.altaz_coord.el < b.sun_pred.altaz_coord.el;});
                long double limit_el = sun_min_el_it->sun_pred.altaz_coord.el - sun_avoid_angle;
                limit_el = (limit_el < cfg_max_el) ? limit_el : cfg_max_el;
                this->track_info_.sun_deviation = true;
                this->track_info_.sun_collision_high_el = true;

                // Check backward.
                for (auto it_internal = it; it_internal != this->begin_ || !stop; it_internal--)
                {
                    if(it_internal->pos.altaz_coord.el >= limit_el)
                    {
                        it_internal->pos.altaz_coord.el = limit_el;
                        it_internal->status = PositionStatus::AVOIDING_SUN;
                    }
                    else
                        stop = true;
                }

                // Update stop flag.
                stop = false;

                // Check forward.
                for (auto it_internal = it; it_internal != this->end_ || !stop; it_internal++)
                {
                    if(it_internal->pos.altaz_coord.el >= limit_el)
                    {
                        it_internal->pos.altaz_coord.el = limit_el;
                        it_internal->status = PositionStatus::AVOIDING_SUN;
                    }
                    else
                    {
                        // Stop and recover iterator.
                        stop = true;
                        it = it_internal;
                    }
                }
            }
            // If sun avoid is applied we have to store the data for each sector  where the tracking goes through a sun
            // security sector.This data will be used for calculating an alternative trajetctory at those sectors.
            else if (inside_sun)
            {
                // If there is a sun collision, start saving sun positions for this sector. The first position is the
                // first position before entering the sector, i.e., it is ouside sun security sector.
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
                // If we were inside a sun sector, and we are going out of it, check sun sector rotation direction and
                // positions within the sector and store the sector. The last position stored is the first position
                // after exiting the sector, i.e., it is outside sun security sector.
                in_sun_sector = false;
                sun_sector.altaz_exit = it->pos.altaz_coord;
                sun_sector.mjdt_exit = it->mjdt;
                // If sector has no valid rotation direction, mark tracking as not valid, since sun cannot be avoided.
                if (!this->setSunSectorRotationDirection(sun_sector, sun_sector_start, it))
                    return false;
                // Check the Sun sector positions.
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
    long double sun_avoid_angle = static_cast<long double>(this->config_.sun_avoid_angle) + kAvoidAngleOffset;
    return std::sqrt(diff_az * diff_az + diff_el * diff_el) < sun_avoid_angle;
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

        long double sun_avoid_angle = static_cast<long double>(this->config_.sun_avoid_angle) + kAvoidAngleOffset;

        long double elev_cw = it->sun_pred.altaz_coord.el + sun_avoid_angle * std::sin(cw_angle);
        long double elev_ccw = it->sun_pred.altaz_coord.el + sun_avoid_angle * std::sin(ccw_angle);

        long double cfg_max_el = static_cast<long double>(this->config_.max_elev);
        long double cfg_min_el = static_cast<long double>(this->config_.min_elev);

        // Check if elevation is required to be below minimum or above 90 in each way. If that is the case,
        // that way will not be valid and the other must be used.
        if (elev_cw >= cfg_max_el || elev_cw <= cfg_min_el)
            valid_cw = false;

        if (elev_ccw >= cfg_max_el || elev_ccw <= cfg_min_el)
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
    // All ok.
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

void TrackingAnalyzer::calcSunAvoidPos(MountPositionAnalyzed &pred,
                                       const SunCollisionSector &sector) const
{
    long double sun_avoid_angle = static_cast<long double>(this->config_.sun_avoid_angle) + kAvoidAngleOffset;

    // Get the trajectory avoid angle.
    long double angle_avoid = this->calcSunAvoidTrajectory(pred.mjdt, sector, pred.sun_pred.altaz_coord);
    // Calculate new azimuth and elevation using trajectory avoid angle
    Degrees new_az = pred.sun_pred.altaz_coord.az + sun_avoid_angle * std::cos(angle_avoid);
    // Normalize new azimuth
    if (new_az < 0.L)
        new_az += 360.L;
    if (new_az > 360.L)
        new_az -= 360.L;

    Degrees new_el = pred.sun_pred.altaz_coord.el + sun_avoid_angle * std::sin(angle_avoid);

    // Store difference between original position and new position
    pred.pos.diff_az = new_az - pred.pos.altaz_coord.az;
    pred.pos.diff_el = new_el - pred.pos.altaz_coord.el;

    // Store new position
    pred.pos.altaz_coord.az = new_az;
    pred.pos.altaz_coord.el = new_el;

}

}}} // END NAMESPACES
// =====================================================================================================================
