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
#include <LibDPSLR/AlgorithmsSLR/utils/tracking_slr.h>
// =====================================================================================================================

// LIBDPSLR NAMESPACES
//======================================================================================================================
namespace dpslr{
namespace algoslr{
namespace utils{

TrackingSLR::TrackingSLR(double min_elev, int mjd_start, long double sod_start, PredictorSLR &&predictor,
                         bool avoid_sun, double sun_avoid_angle) :
    min_elev_(min_elev),
    avoid_sun_(avoid_sun),
    sun_avoid_angle_(sun_avoid_angle),
    predictor_(std::move(predictor))
{
    this->analyzeTrack(mjd_start, sod_start);

    if (this->avoid_sun_)
    {
        this->analyzeSunOverlapping();
    }
    else
    {
        this->sun_overlap_ = false;
    }
}

bool TrackingSLR::isValid() const
{
    return this->valid_pass_;
}

double TrackingSLR::minElev() const
{
    return this->min_elev_;
}

void TrackingSLR::getTrackingStart(int &mjd, long double &sod) const
{
    mjd = this->mjd_start_;
    sod = this->sod_start_;
}

void TrackingSLR::getTrackingEnd(int &mjd, long double &sod) const
{
    mjd = this->mjd_end_;
    sod = this->sod_end_;
}

bool TrackingSLR::getSunAvoidApplied() const
{
    return this->avoid_sun_;
}

bool TrackingSLR::getSunOverlapping() const
{
    return this->sun_overlap_;
}

double TrackingSLR::getSunAvoidAngle() const
{
    return this->sun_avoid_angle_;
}

void TrackingSLR::setSunAvoidApplied(bool apply)
{
    // If sun avoid is switched on, then we must analyze sun collision
    if (apply != this->avoid_sun_)
    {
        this->avoid_sun_ = apply;

        if (this->avoid_sun_)
            this->analyzeSunOverlapping();
    }
}

void TrackingSLR::setSunAvoidAngle(double angle)
{
    this->sun_avoid_angle_ = angle;
}

void TrackingSLR::analyzeTrack(int mjd_start, long double sod_start)
{
    // If predictor was not properly initializated, then we cannot find a valid tracking.
    if (!this->predictor_.isReady())
    {
        this->valid_pass_ = false;
        return;
    }

    int mjd = mjd_start;
    long double sod = sod_start;
    PredictorSLR::PredictionResult result;
    PredictorSLR::PredictionResult previous_result;
    PredictorSLR::PredictionError error_code;
    bool start_found = false;

    // Now we must find the start of the pass. If there is a pass ongoing, that is, elevation is above minimum,
    // then we will find the start by moving backward, otherwise, we will find the start by moving forward.
    error_code = this->predictor_.predict(mjd, sod, result);

    if (error_code != PredictorSLR::PredictionError::NO_ERROR)
    {
        this->valid_pass_ = false;
        return;
    }

    bool look_backward = result.instant_data->el > this->min_elev_;

    do
    {
        previous_result = std::move(result);
        result = {};

        // Advance to next time position.
        // TODO: maybe time skip should be configurable. Now is at 500 ms.
        if (look_backward)
        {
            sod -= 0.5L;
            if (sod < 0.L)
            {
                mjd--;
                sod += 86400.L;
            }
        }
        else
        {
            sod += 0.5L;
            if (sod > 86400.L)
            {
                mjd++;
                sod -= 86400.L;
            }
        }

        error_code = this->predictor_.predict(mjd, sod, result);

        if (error_code != PredictorSLR::PredictionError::NO_ERROR)
        {
            this->valid_pass_ = false;
            return;
        }

        // Look if we reached the tracking start point, i.e, the elevation passes through minimum elevation point
        // If we are going backward, then the first point is before minimum
        // If we are going forward, then the first point is after the minimum
        if (look_backward && result.instant_data->el < this->min_elev_)
        {
            start_found = true;
            this->mjd_start_ = previous_result.instant_data->mjd;
            this->sod_start_ = previous_result.instant_data->sod;

        }
        else if (!look_backward && result.instant_data->el > this->min_elev_)
        {
            start_found = true;
            this->mjd_start_ = result.instant_data->mjd;
            this->sod_start_ = result.instant_data->sod;
        }

    } while (!start_found);


    bool end_found = false;
    mjd = this->mjd_start_;
    sod = this->sod_start_;

    do
    {
        // Advance to next time position.
        // TODO: maybe time skip should be configurable. Now is at 500 ms.
        sod += 0.5L;
        if (sod > 86400.L)
        {
            mjd++;
            sod -= 86400.L;
        }

        error_code = this->predictor_.predict(mjd, sod, result);

        // If X is out of predictor bounds (the predictor range ends in the middle of a pass) or we reach the end of
        // the pass (the elevation is below the minimum), store the end time
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


    this->valid_pass_ = true;

}

void TrackingSLR::analyzeSunOverlapping()
{
    // What happens if:
    // - In the way from home to tracking start, there is a sun collision: treat as an absolute movement.
    // - The tracking start is inside sun sector: move start to first valid position or create an alternative way?
    int mjd = this->mjd_start_;
    long double sod = this->sod_start_;
    PredictorSLR::PredictionResult result;
    PredictorSLR::PredictionError error_code;

    while (sod < this->sod_end_ || mjd < this->mjd_end_)
    {

        error_code = this->predictor_.predict(mjd, sod, result);

        if (error_code != PredictorSLR::PredictionError::NO_ERROR)
        {
            // TODO: error in predict? exception? disable sun avoid algorithm?
        }



        // Advance to next time position.
        // TODO: maybe time skip should be configurable. Now is at 500 ms.
        sod += 0.5L;
        if (sod > 86400.L)
        {
            mjd++;
            sod -= 86400.L;
        }

    }
}







}}} // END NAMESPACES
// =====================================================================================================================
