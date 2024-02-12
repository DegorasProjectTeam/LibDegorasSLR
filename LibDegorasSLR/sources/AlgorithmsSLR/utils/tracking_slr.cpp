/***********************************************************************************************************************
 *   LibDPSLR (Degoras Project SLR Library): A libre base library for SLR related developments.                        *
 *                                                                                                                     *
 *                                                                                                                     *
 *   Copyright (C) 2023 Degoras Project Team                                                                           *
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >                             *
 *                      < Jesús Relinque Madroñal >                                                                    *
 *                                                                                                                     *
 *   This file is part of LibDPSLR.                                                                                    *
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
//======================================================================================================================
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/AlgorithmsSLR/utils/tracking_slr.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
//======================================================================================================================
namespace dpslr{
namespace algoslr{
namespace utils{


TrackingSLR::TrackingSLR(unsigned min_elev, MJDate mjd_start, SoD sod_start, MJDate mjd_end, SoD sod_end,
                         PredictorSLR &&predictor, unsigned time_delta_ms, bool avoid_sun, unsigned sun_avoid_angle) :
    predictor_(std::move(predictor)),
    sun_predictor_(this->predictor_.getGeodeticLocation())
{
    this->track_info_.mjd_start = mjd_start;
    this->track_info_.sod_start = sod_start;
    this->track_info_.mjd_end = mjd_end;
    this->track_info_.sod_end = sod_end;
    this->track_info_.min_elev = min_elev;
    this->track_info_.time_delta = time_delta_ms / 1000.L;
    this->track_info_.sun_avoid_angle = sun_avoid_angle;
    this->track_info_.avoid_sun = avoid_sun;
    this->track_info_.sun_collision_at_start = false;
    this->track_info_.sun_collision_at_end = false;
    this->predictor_.setPredictionMode(PredictorSLR::PredictionMode::INSTANT_VECTOR);
    this->analyzeTracking();
}

TrackingSLR::TrackingSLR(unsigned min_elev, const timing::HRTimePointStd& tp_start,
                         const timing::common::HRTimePointStd &tp_end, PredictorSLR &&predictor,
                         unsigned time_delta_ms, bool avoid_sun, unsigned sun_avoid_angle) :

    predictor_(std::move(predictor)),
    sun_predictor_(this->predictor_.getGeodeticLocation())
{
    this->track_info_.min_elev = min_elev;
    this->track_info_.time_delta = time_delta_ms / 1000.L;
    this->track_info_.sun_avoid_angle = sun_avoid_angle;
    this->track_info_.avoid_sun = avoid_sun;
    this->track_info_.sun_collision_at_start = false;
    this->track_info_.sun_collision_at_end = false;
    timing::timePointToModifiedJulianDate(tp_start, this->track_info_.mjd_start, this->track_info_.sod_start);
    timing::timePointToModifiedJulianDate(tp_end, this->track_info_.mjd_end, this->track_info_.sod_end);
    this->predictor_.setPredictionMode(PredictorSLR::PredictionMode::INSTANT_VECTOR);
    this->analyzeTracking();
}

bool TrackingSLR::isValid() const
{
    return this->track_info_.valid_pass;
}

const TrackingSLR::TrackSLR &TrackingSLR::getTrackingInfo() const
{
    return this->track_info_;
}

unsigned TrackingSLR::getMinElev() const
{
    return static_cast<unsigned>(std::round(this->track_info_.min_elev));
}

void TrackingSLR::getTrackingStart(timing::common::MJDate &mjd, timing::common::SoD &sod) const
{
    mjd = this->track_info_.mjd_start;
    sod = this->track_info_.sod_start;
}

void TrackingSLR::getTrackingEnd(timing::common::MJDate &mjd, timing::common::SoD &sod) const
{
    mjd = this->track_info_.mjd_end;
    sod = this->track_info_.sod_end;
}

TrackingSLR::TrackingResults::const_iterator TrackingSLR::getTrackingBegin() const
{
    return this->track_info_.valid_pass ? this->tracking_begin_ : this->track_info_.positions.cend();
}

TrackingSLR::TrackingResults::const_iterator TrackingSLR::getTrackingEnd() const
{
    return this->track_info_.valid_pass ? this->tracking_end_ : this->track_info_.positions.cend();
}

bool TrackingSLR::getSunAvoidApplied() const
{
    return this->track_info_.avoid_sun;
}

bool TrackingSLR::isSunOverlapping() const
{
    // If sun avoid is enabled, check if there is a sun security sector in the middle of the pass, or if the start or
    // end of the pass were modified due to the sun.
    return this->track_info_.avoid_sun && (!this->track_info_.sun_sectors.empty() ||
                                           this->track_info_.sun_collision_at_start || this->track_info_.sun_collision_at_end) ;
}

bool TrackingSLR::isSunAtStart() const
{
    return this->track_info_.avoid_sun && this->track_info_.sun_collision_at_start;
}

bool TrackingSLR::isSunAtEnd() const
{
    return this->track_info_.avoid_sun && this->track_info_.sun_collision_at_end;
}

unsigned TrackingSLR::getSunAvoidAngle() const
{
    return this->track_info_.sun_avoid_angle;
}

TrackingSLR::PositionStatus TrackingSLR::predictTrackingPosition(const timing::HRTimePointStd& tp_time,
                                                                 TrackingResult &tracking_result)
{
    MJDate mjd;
    SoD sod;
    timing::timePointToModifiedJulianDate(tp_time, mjd, sod);
    return predictTrackingPosition(mjd, sod, tracking_result);
}

TrackingSLR::PositionStatus TrackingSLR::predictTrackingPosition(MJDate mjd, SoD sod,
                                                                 TrackingResult &tracking_result)
{
    // Update the times.
    tracking_result.mjd = mjd;
    tracking_result.sod = sod;
    tracking_result.mjdt = timing::modifiedJulianDateToModifiedJulianDatetime(mjd, sod);

    // Check if requested position is inside valid tracking time. Otherwise return out of tracking error.
    if (!dpslr::timing::mjdInsideTimeWindow(mjd, sod, this->track_info_.mjd_start, this->track_info_.sod_start,
                                            this->track_info_.mjd_end, this->track_info_.sod_end))
    {
        tracking_result.status =  PositionStatus::OUT_OF_TRACK;
        return PositionStatus::OUT_OF_TRACK;
    }

    // Calculates the Sun position.
    long double j2000 = dpslr::timing::mjdToJ2000Datetime(mjd, sod);
    astro::PredictorSun::SunPosition sun_pos = this->sun_predictor_.fastPredict(j2000, false);

    // Calculates the space object position.
    PredictorSLR::PredictionResult prediction_result;
    auto pred_error = this->predictor_.predict(mjd, sod, prediction_result);

    // Store the info.
    tracking_result.prediction_result = prediction_result;
    tracking_result.sun_pos = sun_pos;

    // Check for errors.
    if (pred_error != PredictorSLR::PredictionError::NO_ERROR &&
        pred_error != PredictorSLR::PredictionError::INTERPOLATION_NOT_IN_THE_MIDDLE)
    {
        tracking_result.status =  PositionStatus::PREDICTION_ERROR;
        return PositionStatus::PREDICTION_ERROR;
    }

    // Final position.
    TrackingPosition tracking_position;

    // If sun avoidance is applied and position for requested time is inside a sun security sector, map the point
    // to its corresponding point in the sun security sector circumference.
    // Otherwise, return calculated position.
    if (this->track_info_.avoid_sun && this->insideSunSector(*prediction_result.instant_data, sun_pos))
    {
        MJDateTime mjdt = dpslr::timing::modifiedJulianDateToModifiedJulianDatetime(mjd, sod);

        auto sector_it = std::find_if(this->track_info_.sun_sectors.begin(),
                                      this->track_info_.sun_sectors.end(), [mjdt](const auto& sector)
                                      {
                                          return mjdt >= sector.mjdt_entry && mjdt <= sector.mjdt_exit;
                                      });

        if (sector_it == this->track_info_.sun_sectors.end())
        {
            tracking_result.status =  PositionStatus::CANT_AVOID_SUN;
            return PositionStatus::CANT_AVOID_SUN;
        }

        long double angle_avoid = this->calcSunAvoidTrajectory(mjdt, *sector_it, sun_pos);


        // Calculate the final tracking position and differences.
        tracking_position.az = sun_pos.azimuth + this->track_info_.sun_avoid_angle * std::cos(angle_avoid);
        tracking_position.el = sun_pos.elevation + this->track_info_.sun_avoid_angle * std::sin(angle_avoid);
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

void TrackingSLR::analyzeTracking()
{
    // Results container and auxiliar.
    unsigned step_ms = static_cast<unsigned>(this->track_info_.time_delta*1000);
    PredictorSLR::PredictionResults results_slr;
    astro::PredictorSun::SunPositions results_sun;

    // Parallel calculation of all SLR positions.
    results_slr = this->predictor_.predict(this->track_info_.mjd_start, this->track_info_.sod_start,
                                           this->track_info_.mjd_end, this->track_info_.sod_end, step_ms);

    // Store the validation and check.
    this->track_info_.valid_pass = !results_slr.empty();
    if(!this->track_info_.valid_pass)
        return;

    // Time transformations.
    J2000 j2000_start = timing::mjdToJ2000Datetime(this->track_info_.mjd_start, this->track_info_.sod_start);
    J2000 j2000_end = timing::mjdToJ2000Datetime(this->track_info_.mjd_end, this->track_info_.sod_end);

    // Parallel calculation of all Sun positions. Adds time delta to end date to ensure there is enough sun positions
    // calculated.
    results_sun = this->sun_predictor_.fastPredict(
        j2000_start, j2000_end + this->track_info_.time_delta / timing::kSecsPerDay, step_ms);

    TrackingResult tr;

    for (std::size_t i = 0; i < results_slr.size(); i++)
    {
        tr.mjd = results_slr[i].instant_data->mjd;
        tr.sod = results_slr[i].instant_data->sod;
        tr.mjdt = results_slr[i].instant_data->mjdt;
        tr.prediction_result = results_slr[i];
        tr.tracking_position = TrackingPosition{results_slr[i].instant_data->az, results_slr[i].instant_data->el, 0, 0};
        tr.sun_pos = results_sun[i];

        this->track_info_.positions.push_back(std::move(tr));
        tr = {};
    }

    // Now, after positions have been calculated, check them

    this->track_info_.valid_pass =  this->checkTrackingStart() &&
                                   this->checkTrackingEnd() &&
                                   this->checkTracking();
}

bool TrackingSLR::checkTrackingStart()
{
    auto start = this->track_info_.positions.begin();

    if (this->track_info_.avoid_sun)
    {
        // If sun avoid is activated check if tracking starts inside a sun sector. If that is the case, move the
        // tracking start to the end of the sun sector if possible
        while (start != this->track_info_.positions.end() &&
               this->insideSunSector(*start->prediction_result->instant_data, *start->sun_pos))
        {
            // If there is an error or the sun sector covers the whole tracking, then return false, to mark this pass
            // as invalid.
            if (start->prediction_result->error != PredictorSLR::PredictionError::NO_ERROR ||
                start->tracking_position->el < this->track_info_.min_elev)
                return false;

            // Set current start out of track
            start->status = PositionStatus::OUT_OF_TRACK;

            // Advance to next time position.
            start++;
        }

        // If the whole tracking is within sun sector, then it is invalid.
        if (start == this->track_info_.positions.end())
            return false;

        if (start != this->track_info_.positions.begin())
        {
            // If start has been moved, store new start
            this->track_info_.sun_collision_at_start = true;
            this->track_info_.mjd_start = start->mjd;
            this->track_info_.sod_start = start->sod;
        }

        start->status = PositionStatus::OUTSIDE_SUN;

    }
    else
    {
        // If there was an error with prediction at start, or its elevation is below minimum, then the tracking
        // is not valid.
        if (start->prediction_result->error != PredictorSLR::PredictionError::NO_ERROR ||
            start->tracking_position->el < this->track_info_.min_elev)
            return false;

        // If sun avoid is disabled, check whether position is inside or outside sun security sector
        // without changing start
        if (this->insideSunSector(*start->prediction_result->instant_data, *start->sun_pos))
        {
            start->status = PositionStatus::INSIDE_SUN;
        }
        else
        {
            start->status = PositionStatus::OUTSIDE_SUN;
        }
    }

    this->track_info_.start_elev = start->tracking_position->el;

    this->tracking_begin_ = start;

    return true;
}

bool TrackingSLR::checkTrackingEnd()
{

    auto last = this->track_info_.positions.rbegin();

    if (this->track_info_.avoid_sun)
    {
        // If sun avoid is activated check if tracking ends inside a sun sector. If that is the case, move the
        // tracking end to the start of the sun sector if possible
        while (last != this->track_info_.positions.rend() &&
               this->insideSunSector(*last->prediction_result->instant_data, *last->sun_pos))
        {
            // If there is an error or the sun sector covers the whole tracking, then return false, to mark this pass
            // as invalid.
            if (last->prediction_result->error != PredictorSLR::PredictionError::NO_ERROR ||
                last->tracking_position->el < this->track_info_.min_elev)
                return false;

            // Set current end as out of track
            last->status = PositionStatus::OUT_OF_TRACK;

            // Advance to previous time position.
            last++;
        }

        // If the whole tracking is at sun, then it is invalid. This should be impossible, since it should have been
        // detected at checkTrackingStart.
        if (last == this->track_info_.positions.rend())
            return false;

        if (last != this->track_info_.positions.rbegin())
        {
            // If end has been moved, store new end
            this->track_info_.sun_collision_at_end = true;
            this->track_info_.mjd_end = last->mjd;
            this->track_info_.sod_end = last->sod;
        }

        last->status = PositionStatus::OUTSIDE_SUN;
    }
    else
    {
        // If there was an error with prediction at end, or its elevation is below minimum, then the tracking
        // is not valid.
        if (last->prediction_result->error != PredictorSLR::PredictionError::NO_ERROR ||
            last->tracking_position->el < this->track_info_.min_elev)
            return false;

        // If sun avoid is disabled, check whether position is inside or outside sun security sector
        // without changing end
        if (this->insideSunSector(*last->prediction_result->instant_data, *last->sun_pos))
        {
            last->status = PositionStatus::INSIDE_SUN;
        }
        else
        {
            last->status = PositionStatus::OUTSIDE_SUN;
        }
    }

    this->track_info_.end_elev = last->tracking_position->el;

    this->tracking_end_ = (last + 1).base();

    return true;
}

bool TrackingSLR::checkTracking()
{
    TrackingResult tr;
    bool in_sun_sector = false;
    bool sun_collision = false;
    SunSector sun_sector;
    MJDate max_elev_mjd = 0;
    SoD max_elev_sod = 0;
    double max_elev = -1.;
    TrackingResults::iterator sun_sector_start;

    // Check the tracking.
    for (auto it = this->tracking_begin_ + 1; it != this->tracking_end_ - 1; it++)
    {

        // If position cannot be predicted for this time, or elevation is below minimum, then tracking is not valid.
        if (it->prediction_result->error != PredictorSLR::PredictionError::NO_ERROR ||
            it->tracking_position->el < this->track_info_.min_elev)
            return false;

        // Check if this position is inside sun security sector
        sun_collision = this->insideSunSector(*it->prediction_result->instant_data, *it->sun_pos);

        // Store whether position is inside or outside sun. Later, if sun avoid algorithm is applied, those positions
        // which are inside sun will be checked to see if it is possible or not to avoid sun.
        it->status = sun_collision ? PositionStatus::INSIDE_SUN : PositionStatus::OUTSIDE_SUN;


        if (this->track_info_.avoid_sun)
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
                this->setSunSectorRotationDirection(sun_sector, sun_sector_start, it);
                this->checkSunSectorPositions(sun_sector, sun_sector_start, it);
                this->track_info_.sun_sectors.push_back(std::move(sun_sector));
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
        timing::MjdtToMjdAndSecs(sun_sector.mjdt_entry, this->track_info_.mjd_end, this->track_info_.sod_end);
        for (auto it = sun_sector_start + 1; it !=this->track_info_.positions.end(); it++)
            it->status = PositionStatus::OUT_OF_TRACK;
        this->tracking_end_ = sun_sector_start;
        this->track_info_.sun_collision_at_end = true;
    }

    this->track_info_.max_elev = max_elev;
    this->track_info_.mjd_max_elev = max_elev_mjd;
    this->track_info_.sod_max_elev = max_elev_sod;

    return true;
}

bool TrackingSLR::insideSunSector(const PredictorSLR::InstantData &pos,
                                  const astro::PredictorSun::SunPosition &sun_pos) const
{
    long double diff_az = pos.az - sun_pos.azimuth;
    long double diff_el = pos.el - sun_pos.elevation;
    return std::sqrt(diff_az * diff_az + diff_el * diff_el) < this->track_info_.sun_avoid_angle;
}

void TrackingSLR::setSunSectorRotationDirection(
    SunSector &sector, TrackingResults::const_iterator sun_start, TrackingResults::const_iterator sun_end)
{

    MJDateTime mjdt = sector.mjdt_entry + this->track_info_.time_delta /
                                              static_cast<long double>(dpslr::timing::common::kSecsPerDay);
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

        long double entry_angle = std::atan2(sector.el_entry - it->sun_pos->elevation,
                                             sector.az_entry - it->sun_pos->azimuth);

        long double exit_angle = std::atan2(sector.el_exit - it->sun_pos->elevation,
                                            sector.az_exit - it->sun_pos->azimuth);

        long double cw_angle;
        long double ccw_angle;

        if (exit_angle > entry_angle)
        {

            ccw_angle = entry_angle + time_perc * (exit_angle - entry_angle);
            cw_angle = entry_angle - time_perc * (2 * dpslr::math::common::pi - exit_angle + entry_angle);
            if (cw_angle < 0.L)
                cw_angle += 2 * dpslr::math::common::pi;
        }
        else
        {
            cw_angle = entry_angle - time_perc * (entry_angle - exit_angle);
            ccw_angle = entry_angle + time_perc * (2 * dpslr::math::common::pi - entry_angle + exit_angle);
            if (ccw_angle >= 2 * dpslr::math::common::pi)
                ccw_angle -= 2 * dpslr::math::common::pi;
        }

        long double elev_cw = it->sun_pos->elevation + this->track_info_.sun_avoid_angle * std::sin(cw_angle);
        long double elev_ccw = it->sun_pos->elevation + this->track_info_.sun_avoid_angle * std::sin(ccw_angle);


        if (elev_cw > 90.L || elev_cw < this->track_info_.min_elev)
            valid_cw = false;

        if (elev_ccw > 90.L || elev_ccw < this->track_info_.min_elev)
            valid_ccw = false;

    }

    if (!valid_cw)
        sector.cw = false;
    else if (!valid_ccw)
        sector.cw = true;
    else
    {
        long double entry_angle = std::atan2(sector.el_entry - sun_start->sun_pos->elevation,
                                             sector.az_entry - sun_start->sun_pos->azimuth);

        long double exit_angle = std::atan2(sector.el_exit - sun_end->sun_pos->elevation,
                                            sector.az_exit - sun_end->sun_pos->azimuth);

        long double cw_angle;
        long double ccw_angle;

        if (exit_angle > entry_angle)
        {
            ccw_angle = exit_angle - entry_angle;
            cw_angle = 2 * dpslr::math::common::pi - exit_angle + entry_angle;
        }
        else
        {
            cw_angle = entry_angle - exit_angle;
            ccw_angle = 2 * dpslr::math::common::pi - entry_angle + exit_angle;
        }

        sector.cw = cw_angle < ccw_angle;
    }

    // TODO: what happens if the two ways are not valid?

}

void TrackingSLR::checkSunSectorPositions(
    const SunSector &sector, TrackingResults::iterator sun_start, TrackingResults::iterator sun_end)
{
    // Check positions within sun sector. First and last are excluded, since they are outside sun sector
    for (auto it = sun_start + 1; it != sun_end; it++)
    {
        long double angle_avoid = this->calcSunAvoidTrajectory(it->mjdt, sector, *it->sun_pos);
        it->tracking_position->az = it->sun_pos->azimuth + this->track_info_.sun_avoid_angle * std::cos(angle_avoid);
        it->tracking_position->el = it->sun_pos->elevation + this->track_info_.sun_avoid_angle * std::sin(angle_avoid);
        it->tracking_position->diff_az = it->prediction_result->instant_data->az - it->tracking_position->az;
        it->tracking_position->diff_el = it->prediction_result->instant_data->el - it->tracking_position->el;
        it->status = PositionStatus::AVOIDING_SUN;
    }
}

long double TrackingSLR::calcSunAvoidTrajectory(timing::common::MJDateTime mjdt, const SunSector &sector,
                                                const astro::PredictorSun::SunPosition &sun_pos)
{
    long double time_perc = (mjdt - sector.mjdt_entry) / (sector.mjdt_exit - sector.mjdt_entry);

    long double entry_angle = std::atan2(sector.el_entry - sun_pos.elevation,
                                         sector.az_entry - sun_pos.azimuth);

    long double exit_angle = std::atan2(sector.el_exit - sun_pos.elevation,
                                        sector.az_exit - sun_pos.azimuth);

    long double angle;

    if (exit_angle > entry_angle)
    {
        if (sector.cw)
        {
            angle = entry_angle - time_perc * (2 * dpslr::math::common::pi - exit_angle + entry_angle);
            if (angle < 0.L)
                angle += 2 * dpslr::math::common::pi;
        }
        else
            angle = entry_angle + time_perc * (exit_angle - entry_angle);

    }
    else
    {
        if (sector.cw)
        {
            angle = entry_angle - time_perc * (entry_angle - exit_angle);
        }
        else
        {
            angle = entry_angle + time_perc * (2 * dpslr::math::common::pi - entry_angle + exit_angle);
            if (angle >= 2 * dpslr::math::common::pi)
                angle -= 2 * dpslr::math::common::pi;
        }
    }

    return angle;
}


}}} // END NAMESPACES
// =====================================================================================================================
