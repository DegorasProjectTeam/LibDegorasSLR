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
#include <LibDegorasSLR/AlgorithmsSLR/utils/tracking_slr.h>
// =====================================================================================================================

// LIBDPSLR NAMESPACES
//======================================================================================================================
namespace dpslr{
namespace algoslr{
namespace utils{

// TODO: maybe this time delta should be configurable
constexpr long double kTimeDelta = 0.5L;  // The time delta used for analyzing the tracking in seconds.

TrackingSLR::TrackingSLR(long double min_elev, unsigned int mjd_start, long double sod_start, PredictorSLR &&predictor,
                         bool avoid_sun, long double sun_avoid_angle) :
    min_elev_(min_elev),
    avoid_sun_(avoid_sun),
    sun_avoid_angle_(sun_avoid_angle),
    predictor_(std::move(predictor))
{
    this->analyzeTracking(mjd_start, sod_start);
}

bool TrackingSLR::isValid() const
{
    return this->valid_pass_;
}

long double TrackingSLR::minElev() const
{
    return this->min_elev_;
}

void TrackingSLR::getTrackingStart(unsigned int &mjd, long double &sod) const
{
    mjd = this->mjd_start_;
    sod = this->sod_start_;
}

void TrackingSLR::getTrackingEnd(unsigned int &mjd, long double &sod) const
{
    mjd = this->mjd_end_;
    sod = this->sod_end_;
}

bool TrackingSLR::getSunAvoidApplied() const
{
    return this->avoid_sun_;
}

long double TrackingSLR::getSunAvoidAngle() const
{
    return this->sun_avoid_angle_;
}

TrackingSLR::Position TrackingSLR::getPosition(unsigned int mjd, long double sod)
{
    PredictorSLR::PredictionResult result;
    this->predictor_.predict(mjd, sod, result);

    // TODO: return error code, and position by reference.

    return {};
}

void TrackingSLR::analyzeTracking(unsigned int mjd_start, long double sod_start)
{   
    this->valid_pass_ = this->predictor_.isReady() &&
                        this->findTrackingStart(mjd_start, sod_start) &&
                        this->findTrackingEnd();
}

bool TrackingSLR::findTrackingStart(unsigned int mjd_start, long double sod_start)
{
    // This function will look for the next tracking, starting at mjd_start, sod_start datetime.
    bool start_found = false;
    unsigned int mjd = mjd_start;
    long double sod = sod_start;
    long double j2000;
    PredictorSLR::PredictionResult result;
    PredictorSLR::PredictionResult previous_result;
    PredictorSLR::PredictionError error_code = this->predictor_.predict(mjd, sod, result);
    dpslr::astro::PredictorSun<long double> sun_pred(this->predictor_.getGeodeticLocation());

    if (error_code != PredictorSLR::PredictionError::NO_ERROR)
        return false;

    // If there is a pass ongoing, i.e., elevation is higher than minimum, then we have to go backward to find the
    // start of the current pass. Otherwise we will move forward.
    bool look_backward = result.instant_data->el > this->min_elev_;

    do
    {
        previous_result = std::move(result);
        result = {};

        // Advance to next time position.
        if (look_backward)
        {
            sod -= kTimeDelta;
            if (sod < 0.L)
            {
                mjd--;
                sod += 86400.L;
            }
        }
        else
        {
            sod += kTimeDelta;
            if (sod > 86400.L)
            {
                mjd++;
                sod -= 86400.L;
            }
        }

        error_code = this->predictor_.predict(mjd, sod, result);

        if (error_code != PredictorSLR::PredictionError::NO_ERROR)
            return false;

        // Look if we reached the tracking start point, i.e, the elevation passes through minimum elevation point
        // If we are going backward, then the first point is before minimum
        // If we are going forward, then the first point is after the minimum
        if (look_backward && result.instant_data->el < this->min_elev_)
        {
            start_found = true;
            this->mjd_start_ = previous_result.instant_data->mjd;
            this->sod_start_ = previous_result.instant_data->sod;

            result = std::move(previous_result);
            previous_result = {};

        }
        else if (!look_backward && result.instant_data->el > this->min_elev_)
        {
            start_found = true;
            this->mjd_start_ = result.instant_data->mjd;
            this->sod_start_ = result.instant_data->sod;

            previous_result = {};
        }

    } while (!start_found);

    j2000 = dpslr::timing::mjdToJ2000Datetime(this->mjd_start_, this->sod_start_);

    // If sun avoid is activated check if tracking starts inside a sun sector. If that is the case, move the
    // tracking start to the end of the sun sector
    if (this->avoid_sun_)
    {
        while (this->insideSunSector(*result.instant_data, sun_pred.fastPredict(j2000, false)))
        {
            // Advance to next time position.
            this->sod_start_ += kTimeDelta;
            if (this->sod_start_ > 86400.L)
            {
                this->mjd_start_++;
                this->sod_start_ -= 86400.L;
            }

            j2000 += kTimeDelta / dpslr::timing::common::kSecsInDay;
        }

    }

    return true;
}

bool TrackingSLR::findTrackingEnd()
{
    // Now, after tracking start has been found, we will find the tracking end.
    // This function MUST NOT be called before finding tracking start.
    bool end_found = false;
    bool in_sun_sector = false;
    unsigned int mjd = this->mjd_start_;
    long double sod = this->sod_start_;
    long double j2000 = dpslr::timing::mjdToJ2000Datetime(mjd, sod);

    PredictorSLR::PredictionResult result;
    PredictorSLR::PredictionResult previous_result;
    // Store the prediction for tracking start position at previous_result. It could be used later.
    PredictorSLR::PredictionError error_code = this->predictor_.predict(mjd, sod, previous_result);

    if (error_code != PredictorSLR::PredictionError::NO_ERROR)
        return false;

    dpslr::astro::PredictorSun<long double> sun_pred(this->predictor_.getGeodeticLocation());
    std::vector<dpslr::astro::SunPosition<long double>> sun_positions;
    SunSector sun_sector;

    do
    {
        // Advance to next time position.
        sod += kTimeDelta;
        if (sod > 86400.L)
        {
            mjd++;
            sod -= 86400.L;
        }

        j2000 += kTimeDelta / dpslr::timing::common::kSecsInDay;

        error_code = this->predictor_.predict(mjd, sod, result);

        if (error_code != PredictorSLR::PredictionError::X_INTERPOLATED_OUT_OF_BOUNDS &&
            error_code != PredictorSLR::PredictionError::INTERPOLATION_NOT_IN_THE_MIDDLE &&
            error_code != PredictorSLR::PredictionError::NO_ERROR)
            return false;

        // If sun avoid is applied we have to check if each position is inside sun sector.
        // We will store the data for each sector where the tracking goes through the sun security sector.
        if (this->avoid_sun_)
        {
            auto sun_pos = sun_pred.fastPredict(j2000, false);
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
                this->getSunSectorRotationDirection(sun_sector, sun_positions);
                this->sun_sectors_.push_back(std::move(sun_sector));
                sun_sector = {};
                sun_positions = {};
            }

        }

        // If X is out of predictor bounds (the predictor range ends in the middle of a pass) or we reach the end of
        // the pass (the elevation is below the minimum), store the tracking end time
        if (error_code == PredictorSLR::PredictionError::X_INTERPOLATED_OUT_OF_BOUNDS ||
            result.instant_data->el < this->min_elev_)
        {
            end_found = true;
            this->mjd_end_ = previous_result.instant_data->mjd;
            this->sod_end_ = previous_result.instant_data->sod;
        }
        else
        {
            previous_result = std::move(result);
            result = {};
        }

    } while (!end_found);

    return true;
}

bool TrackingSLR::insideSunSector(const PredictorSLR::InstantData &pos,
                                  const dpslr::astro::SunPosition<long double> &sun_pos) const
{
    long double diff_az = pos.az - sun_pos.azimuth;
    long double diff_el = pos.el - sun_pos.elevation;
    return std::sqrt(diff_az * diff_az + diff_el * diff_el) < this->sun_avoid_angle_;
}

void TrackingSLR::getSunSectorRotationDirection(
    SunSector &sector, const std::vector<dpslr::astro::SunPosition<long double>> &sun_positions) const
{
    dpslr::astro::PredictorSun<long double> sun_pred(this->predictor_.getGeodeticLocation());
    dpslr::astro::SunPosition<long double> sun_start(
        sun_pred.fastPredict(dpslr::timing::mjdtToJ2000Datetime(sector.mjdt_entry), false));
    dpslr::astro::SunPosition<long double> sun_end(
        sun_pred.fastPredict(dpslr::timing::mjdtToJ2000Datetime(sector.mjdt_exit), false));

    PredictorSLR::PredictionResult obj_start;
    PredictorSLR::PredictionResult obj_end;

    this->predictor_.predict(sector.mjdt_entry, obj_start);
    this->predictor_.predict(sector.mjdt_exit, obj_end);

    long double mjdt = sector.mjdt_entry + kTimeDelta / dpslr::timing::common::kSecsInDay;
    bool valid_cw = true;
    bool valid_ccw = true;

    for (const auto& pos : sun_positions)
    {
        long double time_perc = (mjdt - sector.mjdt_entry) / (sector.mjdt_exit - sector.mjdt_entry);

        long double entry_angle = std::atan2(obj_start.instant_data->el - pos.elevation,
                                             obj_start.instant_data->az - pos.azimuth);

        long double exit_angle = std::atan2(obj_end.instant_data->el - pos.elevation,
                                            obj_end.instant_data->az - pos.azimuth);

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

        long double elev_cw = pos.elevation + this->sun_avoid_angle_ * std::sin(cw_angle);
        long double elev_ccw = pos.elevation + this->sun_avoid_angle_ * std::sin(ccw_angle);


        if (elev_cw > 90.L || elev_cw < this->min_elev_)
            valid_cw = false;

        if (elev_ccw > 90.L || elev_ccw < this->min_elev_)
            valid_ccw = false;

    }

    if (!valid_cw)
        sector.cw = false;
    else if (!valid_ccw)
        sector.cw = true;
    else
    {
        long double entry_angle = std::atan2(obj_start.instant_data->el - sun_start.elevation,
                                             obj_start.instant_data->az - sun_start.azimuth);

        long double exit_angle = std::atan2(obj_end.instant_data->el - sun_end.elevation,
                                            obj_end.instant_data->az - sun_end.azimuth);

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

        if (cw_angle < ccw_angle)
            sector.cw = true;
    }

    // TODO: what happens if the two ways are not valid?

}




}}} // END NAMESPACES
// =====================================================================================================================
