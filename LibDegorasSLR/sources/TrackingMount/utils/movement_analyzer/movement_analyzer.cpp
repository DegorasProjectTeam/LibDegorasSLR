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
 * @file movement_analyzer.cpp
 * @author Degoras Project Team
 * @brief This file contains the implementation of the class TrackingAnalyzer.
 * @copyright EUPL License
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/TrackingMount/utils/movement_analyzer/movement_analyzer.h"
#include "LibDegorasSLR/Timing/time_utils.h"
#include "LibDegorasSLR/Mathematics/math_constants.h"
// =====================================================================================================================

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
namespace utils{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace timing;
using namespace timing::types;
using namespace mount::types;
using namespace math::units::literals;
using namespace math::units;
using namespace math;
// ---------------------------------------------------------------------------------------------------------------------

MovementAnalyzer::MovementAnalyzer(const MovementAnalyzerConfig& config) :
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

MovementAnalysis MovementAnalyzer::analyzeMovement(const MountPositionV& mount_positions,
                                                   const astro::types::SunPositionV& sun_positions)
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
    auto it_find = std::find_if(mount_positions.begin(), mount_positions.end(),
                    [](const MountPosition& pos){return math::isFloatingMinorThanZero(pos.altaz_coord.el);});
    if(it_find != mount_positions.end())
    {
        std::string submodule("[LibDegorasSLR,TrackingMount,TrackingAnalyzer,analyzeTracking]");
        std::string error("The mount elevation positions are invalid (minor than 0).");
        throw std::invalid_argument(submodule + " " + error);
    }

    // --------------------------------------------------------------

    // Result container.
    MovementAnalysis track_analysis;

    // Store initial times and coords. This data could change during the analysis.
    track_analysis.mjdt_start = mount_positions.front().mjdt;
    track_analysis.mjdt_end = mount_positions.back().mjdt;
    track_analysis.start_coord = mount_positions.front().altaz_coord;
    track_analysis.end_coord = mount_positions.back().altaz_coord;

    // Store all the mount positions data. At this momment, the tracking positions are equal to mount positions.
    for(const auto& pos : mount_positions)
    {
        track_analysis.analyzed_positions.push_back(pos);
        track_analysis.original_positions.push_back(pos);
    }

    // Store all the Sun positions data. This will not change.
    for(const auto& pos : sun_positions)
        track_analysis.sun_positions.push_back(pos);

    // Store internally in this class the references for handle mount and Sun positions at same time.
    for(size_t i = 0; i<track_analysis.analyzed_positions.size(); i++)
    {
        MountPositionAnalyzed& mount_pos = track_analysis.analyzed_positions[i];
        const astro::types::SunPosition& sun_pos = track_analysis.sun_positions[i];
        this->positions_.push_back({mount_pos, sun_pos});
    }

    // Now, after positions have been calculated, check each situation.

    // Check the start and validate at this point.
    if (!this->analyzeTrackingStart(track_analysis))
    {
        track_analysis.empty_movement = true;
        track_analysis.analyzed_positions.clear();
        return track_analysis;
    }
    // Check the end and validate at this point.
    if (!this->analyzeTrackingEnd(track_analysis))
    {
        track_analysis.empty_movement = true;
        track_analysis.analyzed_positions.clear();
        return track_analysis;
    }

    // Check the middle and validate at this point.
    if (!this->analyzeTrackingMiddle(track_analysis))
    {
        track_analysis.empty_movement = true;
        track_analysis.analyzed_positions.clear();
        return track_analysis;
    }

    // Update the start iterator.
    track_analysis.start_mov_it = std::find_if(track_analysis.analyzed_positions.begin(),
        track_analysis.analyzed_positions.end(), [](const MountPositionAnalyzed& pos)
        {return pos.status != AnalyzedPositionStatus::OUT_OF_TRACK;});

    // Update the end iterator.
    track_analysis.end_mov_it = std::find_if(track_analysis.analyzed_positions.rbegin(),
        track_analysis.analyzed_positions.rend(), [](const MountPositionAnalyzed& pos)
        {return pos.status != AnalyzedPositionStatus::OUT_OF_TRACK;}).base();

    // Return the track analysis.
    return track_analysis;
}

MountPositionAnalyzed MovementAnalyzer::analyzePosition(const MovementAnalysis& analysis,
                                                        const types::MountPosition& mount_pos,
                                                        const astro::types::SunPosition& sun_pos) const
{
    // Store the position. This data could change during the analysis.
    MountPositionAnalyzed analyzed_pos(mount_pos);

    // Check if requested position is inside valid tracking time. Otherwise return out of tracking error.
    if (analyzed_pos.mjdt < analysis.mjdt_start || analyzed_pos.mjdt > analysis.mjdt_end)
    {
        analyzed_pos.status =  AnalyzedPositionStatus::OUT_OF_TRACK;
        return analyzed_pos;
    }

    // Check if the position is in the Sun.
    bool inside_sun = this->insideSunSector(analyzed_pos.altaz_coord, sun_pos.altaz_coord);

    // If sun avoidance is applied and position for requested time is inside a sun security sector, map the point
    // to its corresponding point in the sun security sector circumference.
    // Otherwise, return calculated position.
    if (this->config_.sun_avoid)
    {
        // If sun avoidance is applied
        if (analysis.sun_collision_high)
        {
            // If there was a collision with a high sun sector
            if (analyzed_pos.altaz_coord.el >analysis.max_el)
            {
                // If the elevation of this position is greater than the maximum,
                // clip the elevation to the maximum. Position is avoiding high sun sector.
                analyzed_pos.altaz_coord.el = analysis.max_el;
                analyzed_pos.status = AnalyzedPositionStatus::AVOIDING_SUN;
            }
        }
        else if (inside_sun)
        {
            // If this position is inside a sun security sector then calculate
            // the avoiding trajectory position if it is possible.
            auto sector_it = std::find_if(analysis.sun_sectors.begin(), analysis.sun_sectors.end(),
            [&mjdt = analyzed_pos.mjdt](const auto& sector)
            {
                return mjdt > sector.mjdt_entry && mjdt < sector.mjdt_exit;
            });

            if (sector_it == analysis.sun_sectors.end())
            {
                // Position is inside sun security sector, but no sector was found. It is impossible
                // to avoid sun. This should not happen.
                analyzed_pos.status =  AnalyzedPositionStatus::CANT_AVOID_SUN;
            }
            else
            {
                // Calculate new position avoiding sun
                Positions aux_pos({analyzed_pos, sun_pos});
                this->calcSunAvoidPos(aux_pos, *sector_it);

                // Return the status.
                analyzed_pos.status = AnalyzedPositionStatus::AVOIDING_SUN;
            }
        }
    }
    else if(inside_sun)
    {
        analyzed_pos.status =  AnalyzedPositionStatus::INSIDE_SUN;
    }

    if (analyzed_pos.altaz_coord.el > this->config_.max_elev)
    {
        analyzed_pos.altaz_coord.el = static_cast<Degrees>(this->config_.max_elev);
        analyzed_pos.status =  AnalyzedPositionStatus::ELEVATION_CLIPPED;
    }

    // Return the analyzed position.
    return analyzed_pos;
}

bool MovementAnalyzer::analyzeTrackingStart(MovementAnalysis& analysis)
{ 
    // Get the first position and min and max elevations.
    auto it_pos = this->positions_.begin();
    const Degrees min_el = static_cast<Degrees>(this->config_.min_elev);
    const Degrees max_el = static_cast<Degrees>(this->config_.max_elev);

    // Get the first valid position due to minimum and maximum elevations.
    while (it_pos != this->positions_.end() &&
            (it_pos->mount_pos.altaz_coord.el < min_el || it_pos->mount_pos.altaz_coord.el > max_el))
    {
        it_pos->mount_pos.status = AnalyzedPositionStatus::OUT_OF_TRACK;
        it_pos++;
    }

    // If the whole tracking has incorrect elevation, return.
    if (it_pos == this->positions_.end())
        return false;

    // 1 - Check Sun collisions if avoiding is disable.
    // 2 - Check Sun collisions if avoiding is enable.
    if(!this->config_.sun_avoid)
    {
        // Iterate over the predictions. If sun avoid is disabled, check whether position
        // is inside or outside sun security sector without changing start.
        while (it_pos != this->positions_.end())
        {
            if (this->insideSunSector(it_pos->mount_pos.altaz_coord, it_pos->sun_pos.altaz_coord))
            {
                analysis.sun_collision = true;
                analysis.sun_collision_at_start = true;
                it_pos->mount_pos.status = AnalyzedPositionStatus::INSIDE_SUN;
                it_pos++;
            }
            else
                break;
        }
    }
    else
    {
        // Iterate over the predictions. If sun avoid is enable, move the tracking start to the end
        // of the sun sector if possible.
        while (it_pos != this->positions_.end())
        {
            if(this->insideSunSector(it_pos->mount_pos.altaz_coord, it_pos->sun_pos.altaz_coord))
            {
                analysis.sun_collision = true;
                analysis.sun_collision_at_start = true;
                it_pos->mount_pos.status = AnalyzedPositionStatus::OUT_OF_TRACK;
                it_pos++;
            }
            else
                break;
        }
    }

    // If the whole tracking is in the Sun, return.
    if (it_pos == this->positions_.end())
        return false;

    // If start has been moved, store new start.
    if (it_pos != this->positions_.begin())
    {
        analysis.trim_at_start = true;
        analysis.mjdt_start = it_pos->mount_pos.mjdt;
    }

    // Update the start position.
    analysis.start_coord = it_pos->mount_pos.altaz_coord;
    this->movement_begin_ = it_pos;

    // All ok, return true.
    return true;
}

bool MovementAnalyzer::analyzeTrackingEnd(MovementAnalysis& analysis)
{
    // Get the first prediction and min and max elevations.
    auto it_pos = this->positions_.rbegin();
    const Degrees min_el = static_cast<Degrees>(this->config_.min_elev);
    const Degrees max_el = static_cast<Degrees>(this->config_.max_elev);

    // Get the first valid position due to minimum and maximum elevations.
    while (it_pos != this->positions_.rend() &&
          (it_pos->mount_pos.altaz_coord.el < min_el || it_pos->mount_pos.altaz_coord.el > max_el))
    {
        it_pos->mount_pos.status = AnalyzedPositionStatus::OUT_OF_TRACK;
        it_pos++;
    }

    // If the whole tracking has low elevation, return.
    if (it_pos == this->positions_.rend())
        return false;

    // 1 - Check Sun collisions if avoiding is disable.
    // 2 - Check Sun collisions if avoiding is enable.
    if (!this->config_.sun_avoid)
    {
        // Iterate over the predictions. If sun avoid is disabled, check whether position
        // is inside or outside sun security sector without changing end.
        while (it_pos != this->positions_.rend())
        {
            if (this->insideSunSector(it_pos->mount_pos.altaz_coord, it_pos->sun_pos.altaz_coord))
            {
                analysis.sun_collision = true;
                analysis.sun_collision_at_end = true;
                it_pos->mount_pos.status = AnalyzedPositionStatus::INSIDE_SUN;
                it_pos++;
            }
            else
                break;
        }
    }
    else
    {
        // Iterate over the predictions. If sun avoid is enable, move the tracking end to the end
        // of the sun sector if possible.
        while (it_pos != this->positions_.rend())
        {
            if(this->insideSunSector(it_pos->mount_pos.altaz_coord, it_pos->sun_pos.altaz_coord))
            {
                analysis.sun_collision = true;
                analysis.sun_collision_at_end = true;
                it_pos->mount_pos.status = AnalyzedPositionStatus::OUT_OF_TRACK;
                it_pos++;
            }
            else
                break;
        }
    }

    // If the whole tracking is in the Sun, return.
    if (it_pos == this->positions_.rend())
        return false;

    // If end has been moved, store new end.
    if (it_pos != this->positions_.rbegin())
    {
        analysis.trim_at_end = true;
        analysis.mjdt_end = it_pos->mount_pos.mjdt;
    }

    // Update the end elevation and the real track end iterator.
    analysis.end_coord = it_pos->mount_pos.altaz_coord;
    this->movement_end_ = it_pos.base();

    // All ok, return true.
    return true;
}

bool MovementAnalyzer::analyzeTrackingMiddle(MovementAnalysis& analysis)
{
    //MountPredictionSLR tr;
    bool in_sun_sector = false;
    bool inside_sun = false;
    bool sun_collision = false;
    SunCollisionSector sun_sector;
    MJDateTime max_elev_mjdt = 0;
    Degrees max_elev = -1.0L;
    const Degrees cfg_max_el = static_cast<Degrees>(this->config_.max_elev);
    Degrees sun_avoid_angle = static_cast<Degrees>(this->config_.sun_avoid_angle) + kAvoidAngleOffset;
    std::vector<Positions>::iterator sun_sector_start;

    // Check the tracking maximum altitudes. Positions that trespasses will be clipped to maximum elevation.
    for (auto it = this->movement_begin_; it != this->movement_end_; it++)
    {
        if(it->mount_pos.altaz_coord.el > cfg_max_el)
        {
            it->mount_pos.altaz_coord.el = cfg_max_el;
            it->mount_pos.status = AnalyzedPositionStatus::ELEVATION_CLIPPED;
            analysis.el_deviation = true;
            analysis.max_el = cfg_max_el;
        }
    }

    // Check the tracking Sun collision.
    for (auto it = this->movement_begin_; it != this->movement_end_; it++)
    {
        // Check if this position is inside sun security sector and store it.
        inside_sun = this->insideSunSector(it->mount_pos.altaz_coord, it->sun_pos.altaz_coord);

        // Update sun collision variable.
        sun_collision |= inside_sun;

        // Store whether position is inside or outside sun. Later, if sun avoid algorithm is applied, those positions
        // which are inside sun will be checked to see if it is possible or not to avoid sun.
        if(inside_sun)
            it->mount_pos.status = AnalyzedPositionStatus::INSIDE_SUN;

        // If we need avoid the Sun, calculate the deviation.
        if (this->config_.sun_avoid)
        {
            // Check if the Sun is too high.
            bool sun_high = (it->sun_pos.altaz_coord.el + sun_avoid_angle) >= cfg_max_el;

            // // Update maximum elevations if the Sun is too high. In this case, the maximum el
            if(inside_sun && sun_high)
            {
                bool stop = false;
                auto sun_min_el_it = std::min_element(this->movement_begin_, this->movement_end_,
                    [](const auto& a, const auto& b){return a.sun_pos.altaz_coord.el < b.sun_pos.altaz_coord.el;});
                Degrees limit_el = sun_min_el_it->sun_pos.altaz_coord.el - sun_avoid_angle;
                limit_el = (limit_el < cfg_max_el) ? limit_el : cfg_max_el;
                analysis.sun_deviation = true;
                analysis.sun_collision_high_el = true;

                // Check backward.
                for (auto it_internal = it; it_internal != this->movement_begin_ || !stop; it_internal--)
                {
                    if(it_internal->mount_pos.altaz_coord.el >= limit_el)
                    {
                        it_internal->mount_pos.altaz_coord.el = limit_el;
                        it_internal->mount_pos.status = AnalyzedPositionStatus::AVOIDING_SUN;
                    }
                    else
                        stop = true;
                }

                // Update stop flag.
                stop = false;

                // Check forward.
                for (auto it_internal = it; it_internal != this->movement_end_ || !stop; it_internal++)
                {
                    if(it_internal->mount_pos.altaz_coord.el >= limit_el)
                    {
                        it_internal->mount_pos.altaz_coord.el = limit_el;
                        it_internal->mount_pos.status = AnalyzedPositionStatus::AVOIDING_SUN;
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
                    sun_sector.altaz_entry = sun_sector_start->mount_pos.altaz_coord;
                    sun_sector.mjdt_entry = sun_sector_start->mount_pos.mjdt;
                }
            }
            else if (!inside_sun && in_sun_sector)
            {
                // If we were inside a sun sector, and we are going out of it, check sun sector rotation direction and
                // positions within the sector and store the sector. The last position stored is the first position
                // after exiting the sector, i.e., it is outside sun security sector.
                in_sun_sector = false;
                sun_sector.altaz_exit = it->mount_pos.altaz_coord;
                sun_sector.mjdt_exit = it->mount_pos.mjdt;
                // If sector has no valid rotation direction, mark tracking as not valid, since sun cannot be avoided.
                if (!this->setSunSectorRotationDirection(sun_sector, sun_sector_start, it))
                    return false;
                // Check the Sun sector positions.
                this->checkSunSectorPositions(analysis, sun_sector, sun_sector_start, it);
                analysis.sun_sectors.push_back(std::move(sun_sector));
                sun_sector = {};
            }
        }

        if (it->mount_pos.altaz_coord.el > max_elev)
        {
            max_elev = it->mount_pos.altaz_coord.el;
            max_elev_mjdt = it->mount_pos.mjdt;
        }
    }

    // Finally update the movement information.
    analysis.max_el = max_elev;
    analysis.sun_collision |= sun_collision;
    analysis.sun_collision_at_middle = sun_collision;

    // All ok, return true.
    return true;
}

bool MovementAnalyzer::insideSunSector(const astro::types::AltAzPos& pass_pos,
                                       const astro::types::AltAzPos& sun_pos) const
{
    Degrees diff_az = pass_pos.az - sun_pos.az;
    // If azimuth difference is greater than 180, then take the shorter way
    if (diff_az > 180.0_deg)
        diff_az = 360.0_deg - diff_az;
    Degrees diff_el = pass_pos.el - sun_pos.el;
    Degrees sun_avoid_angle = static_cast<Degrees>(this->config_.sun_avoid_angle) + kAvoidAngleOffset;
    return std::sqrt(diff_az * diff_az + diff_el * diff_el) < sun_avoid_angle;
}

bool MovementAnalyzer::setSunSectorRotationDirection(SunCollisionSector& sector,
                                                     std::vector<Positions>::const_iterator sun_start,
                                                     std::vector<Positions>::const_iterator sun_end) const
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
        long double time_perc = (it->mount_pos.mjdt - sector.mjdt_entry) / (sector.mjdt_exit - sector.mjdt_entry);

        Degrees diff_az_entry = sector.altaz_entry.az - it->sun_pos.altaz_coord.az;
        if (diff_az_entry > 180._deg)
        {
            diff_az_entry = -(360._deg - diff_az_entry);
        }
        else if (diff_az_entry < -180._deg)
        {
            diff_az_entry = 360._deg + diff_az_entry;
        }

        Degrees diff_az_exit = sector.altaz_exit.az - it->sun_pos.altaz_coord.az;
        if (diff_az_exit > 180._deg)
        {
            diff_az_exit = -(360._deg - diff_az_exit);
        }
        else if (diff_az_exit < -180._deg)
        {
            diff_az_exit = 360._deg + diff_az_exit;
        }

        // Calculate the entry and exit angle.
        long double entry_angle = std::atan2(sector.altaz_entry.el - it->sun_pos.altaz_coord.el, diff_az_entry);
        long double exit_angle = std::atan2(sector.altaz_exit.el - it->sun_pos.altaz_coord.el, diff_az_exit);

        // Normalize entry angle between 0 and 2pi
        if (math::isFloatingMinorThanZero(entry_angle))
            entry_angle += 2*math::kPi;

        // Normalize exit angle between 0 and 2pi
        if (math::isFloatingMinorThanZero(exit_angle))
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

        // Get the configuration data.
        const Degrees sun_avoid_angle = static_cast<Degrees>(this->config_.sun_avoid_angle) + kAvoidAngleOffset;
        const Degrees cfg_max_el = static_cast<Degrees>(this->config_.max_elev);
        const Degrees cfg_min_el = static_cast<Degrees>(this->config_.min_elev);

        long double elev_cw = it->sun_pos.altaz_coord.el + sun_avoid_angle * std::sin(cw_angle);
        long double elev_ccw = it->sun_pos.altaz_coord.el + sun_avoid_angle * std::sin(ccw_angle);

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
        sector.cw = SunCollisionSector::AvoidanceDirection::COUNTERCLOCKWISE;
    else if (!valid_ccw)
        sector.cw = SunCollisionSector::AvoidanceDirection::CLOCKWISE;
    else
    {
        // Calculate angle between start and end to see which is shorter.
        Degrees diff_az_entry = sector.altaz_entry.az - sun_start->sun_pos.altaz_coord.az;
        if (diff_az_entry > 180._deg)
        {
            diff_az_entry = -(360._deg - diff_az_entry);
        }
        else if (diff_az_entry < -180._deg)
        {
            diff_az_entry = 360._deg + diff_az_entry;
        }

        long double diff_az_exit = sector.altaz_exit.az - sun_end->sun_pos.altaz_coord.az;
        if (diff_az_exit > 180._deg)
        {
            diff_az_exit = -(360._deg - diff_az_exit);
        }
        else if (diff_az_exit < -180._deg)
        {
            diff_az_exit = 360._deg + diff_az_exit;
        }

        long double entry_angle = std::atan2(sector.altaz_entry.el - sun_start->sun_pos.altaz_coord.el, diff_az_entry);
        long double exit_angle = std::atan2(sector.altaz_exit.el - sun_end->sun_pos.altaz_coord.el, diff_az_exit);

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
                        SunCollisionSector::AvoidanceDirection::CLOCKWISE :
                        SunCollisionSector::AvoidanceDirection::COUNTERCLOCKWISE;
    }
    // All ok.
    return true;
}

void MovementAnalyzer::calcSunAvoidPos(Positions& pos, const SunCollisionSector& sector) const
{
    // Get data.
    const Degrees sun_avoid_angle = static_cast<Degrees>(this->config_.sun_avoid_angle) + kAvoidAngleOffset;
    long double angle_avoid = this->calcSunAvoidTrajectory(pos.mount_pos.mjdt, sector, pos.sun_pos.altaz_coord);

    // Calculate new azimuth using trajectory avoid angle
    Degrees new_az = pos.sun_pos.altaz_coord.az + sun_avoid_angle * std::cos(angle_avoid);

    // Normalize new azimuth
    if (new_az < 0._deg)
        new_az += 360._deg;
    if (new_az > 360._deg)
        new_az -= 360._deg;

    // Calculate new elevation using trajectory avoid angle
    Degrees new_el = pos.sun_pos.altaz_coord.el + sun_avoid_angle * std::sin(angle_avoid);

    // Store difference between original position and new position
    pos.mount_pos.altaz_diff.az = new_az - pos.mount_pos.altaz_coord.az;
    pos.mount_pos.altaz_diff.el = new_el - pos.mount_pos.altaz_coord.el;

    // Store new position
    pos.mount_pos.altaz_coord.az = new_az;
    pos.mount_pos.altaz_coord.el = new_el;
}

void MovementAnalyzer::checkSunSectorPositions(MovementAnalysis& analysis,
                                               const SunCollisionSector &sector,
                                               std::vector<Positions>::iterator sun_start,
                                               std::vector<Positions>::iterator sun_end) const
{
    // Check positions within sun sector. First and last are excluded, since they are outside sun sector
    for (auto it = sun_start + 1; it != sun_end; it++)
    {
        // Calculate the avoiding position using the avoid angle and distance as radius. Store the difference between
        // original position and new position
        this->calcSunAvoidPos(*it, sector);
        it->mount_pos.status = AnalyzedPositionStatus::AVOIDING_SUN;

        // Update sun deviation flag.
        analysis.sun_deviation = true;
    }
}

long double MovementAnalyzer::calcSunAvoidTrajectory(const MJDateTime &mjdt,
                                                     const SunCollisionSector& sector,
                                                     const astro::types::AltAzPos& sun_pos) const
{
    long double time_perc = (mjdt - sector.mjdt_entry) / (sector.mjdt_exit - sector.mjdt_entry);

    long double diff_az_entry = sector.altaz_entry.az - sun_pos.az;

    // Normalize in range -180, 180
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
        if (sector.cw == SunCollisionSector::AvoidanceDirection::COUNTERCLOCKWISE)
            angle = -(2*math::kPi - angle);
    }
    else
    {
        if (sector.cw == SunCollisionSector::AvoidanceDirection::CLOCKWISE)
            angle = (2*math::kPi + angle);
    }

    return entry_angle + angle * time_perc;
}

}}} // END NAMESPACES
// =====================================================================================================================
