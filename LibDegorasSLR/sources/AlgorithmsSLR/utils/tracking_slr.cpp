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


TrackingSLR::TrackingSLR(unsigned int min_elev, MJDate mjd_start, SoD sod_start, MJDate mjd_end, SoD sod_end,
                         PredictorSLR &&predictor, unsigned int time_delta, bool avoid_sun, unsigned int sun_avoid_angle) :
    predictor_(std::move(predictor)),
    sun_predictor_(this->predictor_.getGeodeticLocation())
{
    this->track_info_.mjd_start = mjd_start;
    this->track_info_.sod_start = sod_start;
    this->track_info_.mjd_end = mjd_end;
    this->track_info_.sod_end = sod_end;
    this->track_info_.min_elev = min_elev;
    this->track_info_.time_delta = time_delta / 1000.;
    this->track_info_.sun_avoid_angle = sun_avoid_angle;
    this->track_info_.avoid_sun = avoid_sun;
    this->track_info_.sun_collision_at_start = false;
    this->track_info_.sun_collision_at_end = false;
    this->analyzeTracking();
}

TrackingSLR::TrackingSLR(unsigned int min_elev, const timing::HRTimePointStd& tp_start,
                         const timing::common::HRTimePointStd &tp_end, PredictorSLR &&predictor,
                         unsigned int time_delta_ms, bool avoid_sun, unsigned int sun_avoid_angle) :

    predictor_(std::move(predictor)),
    sun_predictor_(this->predictor_.getGeodeticLocation())
{
    this->track_info_.min_elev = min_elev;
    this->track_info_.time_delta = time_delta_ms / 1000.;
    this->track_info_.sun_avoid_angle = sun_avoid_angle;
    this->track_info_.avoid_sun = avoid_sun;
    this->track_info_.sun_collision_at_start = false;
    this->track_info_.sun_collision_at_end = false;
    timing::timePointToModifiedJulianDate(tp_start, this->track_info_.mjd_start, this->track_info_.sod_start);
    timing::timePointToModifiedJulianDate(tp_end, this->track_info_.mjd_end, this->track_info_.sod_end);
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
    tracking_result.mjdt = timing::mjdAndSecsToMjdt(mjd, sod);

    // Check if requested position is inside valid tracking time. Otherwise return out of tracking error.
    if (!dpslr::timing::mjdInsideTimeWindow(mjd, sod, this->track_info_.mjd_start, this->track_info_.sod_start,
                                            this->track_info_.mjd_end, this->track_info_.sod_end))
    {
        tracking_result.status =  PositionStatus::OUT_OF_TRACK;
        return PositionStatus::OUT_OF_TRACK;
    }

    // Calculates the Sun position.
    long double j2000 = dpslr::timing::mjdToJ2000Datetime(mjd, sod);
    dpslr::astro::SunPosition<long double> sun_pos = this->sun_predictor_.fastPredict(j2000, false);

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
        MJDateTime mjdt = dpslr::timing::mjdAndSecsToMjdt(mjd, sod);

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

        long double time_perc = (mjdt - sector_it->mjdt_entry) / (sector_it->mjdt_exit - sector_it->mjdt_entry);

        long double entry_angle = std::atan2(sector_it->el_entry - sun_pos.elevation,
                                             sector_it->az_entry - sun_pos.azimuth);

        long double exit_angle = std::atan2(sector_it->el_exit - sun_pos.elevation,
                                            sector_it->az_exit - sun_pos.azimuth);

        long double angle;

        if (exit_angle > entry_angle)
        {
            if (sector_it->cw)
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
            if (sector_it->cw)
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

        // Calculate the final tracking position and differences.
        tracking_position.az = sun_pos.azimuth + this->track_info_.sun_avoid_angle * std::cos(angle);
        tracking_position.el = sun_pos.elevation + this->track_info_.sun_avoid_angle * std::sin(angle);
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
    bool valid_time = this->track_info_.mjd_start < this->track_info_.mjd_end ||
                      (this->track_info_.mjd_start == this->track_info_.mjd_end &&
                       this->track_info_.sod_start < this->track_info_.sod_end);

    this->track_info_.valid_pass = valid_time &&
                                    this->predictor_.isReady() &&
                                    this->checkTrackingStart() &&
                                    this->checkTrackingEnd() &&
                                    this->checkTracking();
}

bool TrackingSLR::checkTrackingStart()
{
    MJDate predict_mjd_start, predict_mjd_end;
    SoD predict_sod_start, predict_sod_end;
    this->predictor_.getTimeWindow(predict_mjd_start, predict_sod_start, predict_mjd_end, predict_sod_end);
    // Check if start time is inside predictor available time window
    if (!dpslr::timing::mjdInsideTimeWindow(this->track_info_.mjd_start, this->track_info_.sod_start,
                                            predict_mjd_start, predict_sod_start, predict_mjd_end, predict_sod_end))
        return false;

    PredictorSLR::PredictionResult result;
    PredictorSLR::PredictionResult previous_result;
    PredictorSLR::PredictionError error_code = this->predictor_.predict(
        this->track_info_.mjd_start, this->track_info_.sod_start, result);

    // If there was an error with prediction at start, or its elevation is below minimum, then the tracking
    // is not valid.
    if (error_code != PredictorSLR::PredictionError::NO_ERROR ||
        result.instant_data->el < this->track_info_.min_elev)
        return false;

    // If sun avoid is activated check if tracking starts inside a sun sector. If that is the case, move the
    // tracking start to the end of the sun sector if possible
    if (this->track_info_.avoid_sun)
    {
        double j2000 = dpslr::timing::mjdToJ2000Datetime(this->track_info_.mjd_start, this->track_info_.sod_start);

        while (this->insideSunSector(*result.instant_data, this->sun_predictor_.fastPredict(j2000, false)))
        {
            // Advance to next time position.
            this->track_info_.sod_start += this->track_info_.time_delta;
            if (this->track_info_.sod_start > dpslr::timing::common::kSecsInDay)
            {
                this->track_info_.mjd_start++;
                this->track_info_.sod_start -= dpslr::timing::common::kSecsInDay;
            }

            j2000 += this->track_info_.time_delta / static_cast<double>(dpslr::timing::common::kSecsInDay);

            error_code = this->predictor_.predict(this->track_info_.mjd_start, this->track_info_.sod_start, result);

            // If there is an error or the sun sector covers the whole tracking, then return false, to mark this pass
            // as invalid.
            if (error_code != PredictorSLR::PredictionError::NO_ERROR ||
                result.instant_data->el < this->track_info_.min_elev)
                return false;

            this->track_info_.sun_collision_at_start = true;
        }

    }

    this->track_info_.start_elev = result.instant_data->el;

    return true;
}

bool TrackingSLR::checkTrackingEnd()
{
    MJDate predict_mjd_start, predict_mjd_end;
    SoD predict_sod_start, predict_sod_end;
    this->predictor_.getTimeWindow(predict_mjd_start, predict_sod_start, predict_mjd_end, predict_sod_end);
    // Check if end time is inside predictor available time window
    if (!dpslr::timing::mjdInsideTimeWindow(this->track_info_.mjd_end, this->track_info_.sod_end,
                                            predict_mjd_start, predict_sod_start, predict_mjd_end, predict_sod_end))
        return false;

    PredictorSLR::PredictionResult result;
    PredictorSLR::PredictionResult previous_result;
    PredictorSLR::PredictionError error_code = this->predictor_.predict(this->track_info_.mjd_end,
                                                                        this->track_info_.sod_end, result);

    // If sun avoid is activated check if tracking ends inside a sun sector. If that is the case, move the
    // tracking end to the start of the sun sector if possible
    if (this->track_info_.avoid_sun)
    {

        // If there was an error with prediction at start, or its elevation is below minimum, then the tracking
        // is not valid.
        if (error_code != PredictorSLR::PredictionError::NO_ERROR ||
            result.instant_data->el < this->track_info_.min_elev)
            return false;

        double j2000 = dpslr::timing::mjdToJ2000Datetime(this->track_info_.mjd_end, this->track_info_.sod_end);

        while (this->insideSunSector(*result.instant_data, this->sun_predictor_.fastPredict(j2000, false)))
        {
            // Advance to previous time position.
            this->track_info_.sod_end -= this->track_info_.time_delta;
            if (this->track_info_.sod_end < 0.L)
            {
                this->track_info_.mjd_start--;
                this->track_info_.sod_start += dpslr::timing::common::kSecsInDay;
            }

            j2000 -= this->track_info_.time_delta / static_cast<long double>(dpslr::timing::common::kSecsInDay);

            error_code = this->predictor_.predict(this->track_info_.mjd_end, this->track_info_.sod_end, result);

            // If there is an error or the sun sector covers the whole tracking, then return false, to mark this pass
            // as invalid.
            if (error_code != PredictorSLR::PredictionError::NO_ERROR ||
                result.instant_data->el < this->track_info_.min_elev)
                return false;

            this->track_info_.sun_collision_at_end = true;
        }

    }

    this->track_info_.end_elev = result.instant_data->el;

    return true;
}

bool TrackingSLR::checkTracking()
{
    bool in_sun_sector = false;
    std::vector<dpslr::astro::SunPosition<long double>> sun_positions;
    SunSector sun_sector;
    MJDate mjd = this->track_info_.mjd_start;
    SoD sod = this->track_info_.sod_start;
    long double j2000 = dpslr::timing::mjdToJ2000Datetime(mjd, sod);
    MJDate max_elev_mjd = 0;
    SoD max_elev_sod = 0;
    double max_elev = -1.;

    PredictorSLR::PredictionResult result;
    PredictorSLR::PredictionResult previous_result;
    // Store the prediction for tracking start position at previous_result. It could be used later.
    PredictorSLR::PredictionError error_code = this->predictor_.predict(mjd, sod, previous_result);

    if (error_code != PredictorSLR::PredictionError::NO_ERROR)
        return false;

    // Check the tracking.
    do
    {
        // Advance to next time position.
        sod += this->track_info_.time_delta;
        if (sod > dpslr::timing::common::kSecsInDay)
        {
            mjd++;
            sod -= dpslr::timing::common::kSecsInDay;
        }

        j2000 += this->track_info_.time_delta / static_cast<long double>(dpslr::timing::common::kSecsInDay);

        error_code = this->predictor_.predict(mjd, sod, result);

        // If position cannot be predicted for this time, or elevation is below minimum, then tracking is not valid.
        if (error_code != PredictorSLR::PredictionError::NO_ERROR ||
            result.instant_data->el < this->track_info_.min_elev)
            return false;

        // If sun avoid is applied we have to check if each position is inside sun sector.
        // We will store the data for each sector where the tracking goes through the sun security sector.
        if (this->track_info_.avoid_sun)
        {
            auto sun_pos = this->sun_predictor_.fastPredict(j2000, false);
            bool sun_collision = this->insideSunSector(*result.instant_data, sun_pos);
            if (sun_collision)
            {
                // If there is a sun collision, start saving sun positions for this sector
                if (!in_sun_sector)
                {
                    in_sun_sector = true;
                    sun_sector.az_entry = previous_result.instant_data->az;
                    sun_sector.el_entry = previous_result.instant_data->el;
                    sun_sector.mjdt_entry = previous_result.instant_data->mjdt;
                }
                sun_positions.push_back(sun_pos);

            }
            else if (!sun_collision && in_sun_sector)
            {
                // If we were inside a sun sector, and we are going out of it, check sun sector rotation direction and
                // store the sector
                in_sun_sector = false;
                sun_sector.az_exit = result.instant_data->az;
                sun_sector.el_exit = result.instant_data->el;
                sun_sector.mjdt_exit = result.instant_data->mjdt;
                this->setSunSectorRotationDirection(sun_sector, sun_positions);
                this->track_info_.sun_sectors.push_back(std::move(sun_sector));
                sun_sector = {};
                sun_positions = {};
            }

        }

        if (result.instant_data->el > max_elev)
        {
            max_elev = result.instant_data->el;
            max_elev_mjd = mjd;
            max_elev_sod = sod;
        }

        previous_result = std::move(result);
        result = {};

    } while (mjd < this->track_info_.mjd_end || sod < this->track_info_.sod_end);

    // If there was a sun security sector until the position previous to the last one, finish the tracking before
    // starting the sector.
    if (in_sun_sector)
    {
        timing::MjdtToMjdAndSecs(sun_sector.mjdt_entry, this->track_info_.mjd_end, this->track_info_.sod_end);
        this->track_info_.sun_collision_at_end = true;
    }

    this->track_info_.max_elev = max_elev;
    this->track_info_.mjd_max_elev = max_elev_mjd;
    this->track_info_.sod_max_elev = max_elev_sod;

    return true;
}

bool TrackingSLR::insideSunSector(const PredictorSLR::InstantData &pos,
                                  const dpslr::astro::SunPosition<long double> &sun_pos) const
{
    long double diff_az = pos.az - sun_pos.azimuth;
    long double diff_el = pos.el - sun_pos.elevation;
    return std::sqrt(diff_az * diff_az + diff_el * diff_el) < this->track_info_.sun_avoid_angle;
}

void TrackingSLR::setSunSectorRotationDirection(
    SunSector &sector, const std::vector<dpslr::astro::SunPosition<long double>> &sun_positions)
{
    dpslr::astro::SunPosition<long double> sun_start(
        this->sun_predictor_.fastPredict(dpslr::timing::mjdtToJ2000Datetime(sector.mjdt_entry), false));
    dpslr::astro::SunPosition<long double> sun_end(
        this->sun_predictor_.fastPredict(dpslr::timing::mjdtToJ2000Datetime(sector.mjdt_exit), false));

    MJDateTime mjdt = sector.mjdt_entry + this->track_info_.time_delta /
                                              static_cast<long double>(dpslr::timing::common::kSecsInDay);
    bool valid_cw = true;
    bool valid_ccw = true;

    // To avoid the sun, we will calculate a circumference with radius sun_avoid_angle_ and we will map each point of
    // the original trajectory to a point of the circumference between the entry and the exit point. We can travel
    // around the circumference clockwise or counterclockwise.
    // We have to check if every point in the sun avoid trajectory is in a valid trajectory, i.e., the elevation is
    // between the minimum and 90 degrees. If the clockwise trajectory around the sun security sector is not valid,
    // then we have to choose the counterclockwise trajectory and viceversa.
    // If both trajectories are valid, we will choose the shorter one.
    for (const auto& pos : sun_positions)
    {
        long double time_perc = (mjdt - sector.mjdt_entry) / (sector.mjdt_exit - sector.mjdt_entry);

        long double entry_angle = std::atan2(sector.el_entry - pos.elevation,
                                             sector.az_entry - pos.azimuth);

        long double exit_angle = std::atan2(sector.el_exit - pos.elevation,
                                            sector.az_exit - pos.azimuth);

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

        long double elev_cw = pos.elevation + this->track_info_.sun_avoid_angle * std::sin(cw_angle);
        long double elev_ccw = pos.elevation + this->track_info_.sun_avoid_angle * std::sin(ccw_angle);


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
        long double entry_angle = std::atan2(sector.el_entry - sun_start.elevation,
                                             sector.az_entry - sun_start.azimuth);

        long double exit_angle = std::atan2(sector.el_exit - sun_end.elevation,
                                            sector.az_exit - sun_end.azimuth);

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


}}} // END NAMESPACES
// =====================================================================================================================
