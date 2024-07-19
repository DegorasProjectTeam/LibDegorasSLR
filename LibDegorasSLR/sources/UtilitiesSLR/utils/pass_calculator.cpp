/***********************************************************************************************************************
 * Copyright 2023 Degoras Project Team
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they will be approved by the
 * European Commission - subsequent versions of the EUPL (the "Licence");
 *
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the Licence is distributed on
 * an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the Licence for the
 * specific language governing permissions and limitations under the Licence.
 **********************************************************************************************************************/

/** ********************************************************************************************************************
 * @file pass_calculator.cpp
 * @see pass_calculator.h
 * @author DEGORAS PROJECT TEAM
 * @copyright EUPL License
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/UtilitiesSLR/utils/pass_calculator.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace slr{
namespace utils{
// =====================================================================================================================

PassCalculator::PassCalculator(predictors::PredictorSlrPtr predictor, math::units::DegreesU min_elev,
                               math::units::MillisecondsU time_step) :
    min_elev_(min_elev),
    time_step_(time_step),
    predictor_(std::move(predictor))
{

}

void PassCalculator::setMinElev(math::units::DegreesU min_elev)
{
    this->min_elev_ = min_elev;
}

math::units::DegreesU PassCalculator::minElev() const
{
    return this->min_elev_;
}

void PassCalculator::setTimeStep(math::units::MillisecondsU time_step)
{
    this->time_step_ = time_step;
}

math::units::MillisecondsU PassCalculator::getTimeStep() const
{
    return this->time_step_;
}

PassCalculator::ResultCode PassCalculator::getPasses(const timing::dates::MJDateTime &mjd_start,
                                                     const timing::dates::MJDateTime &mjd_end,
                                                     std::vector<SpaceObjectPass> &passes) const
{
    // Clear passes vector
    passes.clear();

    // Check if predictor is valid.
    if (!this->predictor_->isReady())
        return ResultCode::PREDICTOR_NOT_VALID;

    // Check if time interval is inside valid prediction window.
    if (!this->predictor_->isInsideTimeWindow(mjd_start, mjd_end))
        return ResultCode::INTERVAL_OUTSIDE_OF_PREDICTOR;

    // Auxiliary variables.
    bool pass_started = false;
    SpaceObjectPass current_pass;
    current_pass.time_step = this->time_step_;
    current_pass.min_elev = this->min_elev_;

    // Calculate all the predictions.
    auto predictions = this->predictor_->predict(mjd_start, mjd_end, this->time_step_);

    // If a pass is ongoing at start time, set start trimmed flag.
    if (predictions.front().instant_data->altaz_coord.el > this->min_elev_)
    {
        current_pass.start_trimmed = true;
    }

    // Check the predictions to get all the passes within the time window.
    for (auto &&pred : predictions)
    {
        if (0 != pred.error)
        {
            passes.clear();
            return PassCalculator::ResultCode::SOME_PREDICTIONS_NOT_VALID;
        }

        if (pred.instant_data->altaz_coord.el > this->min_elev_)
        {
            long double azim_rate = 0;
            long double elev_rate = 0;

            if (!pass_started)
            {
                pass_started = true;
            }
            else
            {
                azim_rate = std::abs(
                    (pred.instant_data->altaz_coord.az - current_pass.steps.back().altaz_coord.az) /
                    (this->time_step_ / 1000.0L));
                elev_rate = std::abs(
                    (pred.instant_data->altaz_coord.el - current_pass.steps.back().altaz_coord.el) /
                    (this->time_step_ / 1000.0L));
            }
            current_pass.steps.push_back(SpaceObjectPassStep(std::move(pred), azim_rate, elev_rate));
        }
        else if(pass_started)
        {
            pass_started = false;
            passes.push_back(std::move(current_pass));
            current_pass = {};
        }
    }

    // If a pass is ongoing at end time, store the pass and set end trimmed flag.
    if (pass_started)
    {
        current_pass.end_trimmed = true;
        passes.push_back(current_pass);
    }

    return PassCalculator::ResultCode::NOT_ERROR;
}

PassCalculator::ResultCode PassCalculator::getNextPass(const timing::dates::MJDateTime& mjd_start,
                                                       SpaceObjectPass& pass) const
{
    // Clear passes vector
    pass = {};

    // Check if predictor is valid.
    if (!this->predictor_->isReady())
        return ResultCode::PREDICTOR_NOT_VALID;

    // Check if time is inside valid prediction window.
    if (!this->predictor_->isInsideTime(mjd_start))
        return ResultCode::TIME_OUTSIDE_OF_PREDICTOR;

    // Declare the times.
    timing::dates::MJDateTime mjdt_start_window, mjdt_end_window;

    // Get the time window.
    this->predictor_->getAvailableTimeWindow(mjdt_start_window, mjdt_end_window);

    // Calculate all the predictions.
    predictors::PredictionSLR pred;
    auto pred_error = this->predictor_->predict(mjd_start, pred);

    // Check the initial prediction.
    if (0 != pred_error)
        return ResultCode::SOME_PREDICTIONS_NOT_VALID;

    // Auxiliary variables.
    math::units::Seconds time_step_sec = this->time_step_ / 1000.0L;
    bool pass_started = pred.instant_data->altaz_coord.el > this->min_elev_;
    pass.time_step = this->time_step_;
    pass.min_elev = this->min_elev_;
    std::vector<SpaceObjectPassStep>& steps = pass.steps;
    timing::dates::MJDateTime mjdt;

    // TODO ADD IN STEPS THE RATES.

    // Check if the pass already started.
    if(pass_started)
    {
        // Insert start time position, since it is inside pass
        steps.push_back(SpaceObjectPassStep(std::move(pred)));

        // Look for the start of the pass going backwards from start.
        mjdt = mjd_start - time_step_sec;
        do
        {
            pred_error = this->predictor_->predict(mjdt, pred);
            if (0 != pred_error)
            {
                pass = {};
                return ResultCode::SOME_PREDICTIONS_NOT_VALID;
            }
            // Store pass step.
            steps.push_back(SpaceObjectPassStep(std::move(pred)));
            mjdt -= time_step_sec;
        } while (steps.back().slr_pred.instant_data->altaz_coord.el >
                     this->min_elev_ && mjdt >= mjdt_start_window);

        // Delete first element (last due to reverse order), since it is outside of pass in some situations.
        steps.pop_back();

        // Put elements in chronological order.
        std::reverse(steps.begin(), steps.end());

        // Set time to position after start, to look for the end of the pass.
        mjdt = mjd_start + time_step_sec;
    }
    else
    {
        // Look for the start of the pass going forward from start.
        mjdt = mjd_start + time_step_sec;
        do
        {
            pred_error = this->predictor_->predict(mjdt, pred);
            if (0 != pred_error)
            {
                pass = {};
                return ResultCode::SOME_PREDICTIONS_NOT_VALID;
            }
            mjdt += time_step_sec;
        } while (pred.instant_data->altaz_coord.el < this->min_elev_ && mjdt <= mjdt_end_window);

        // If end of predictor time window has not been reached, start was found. Otherwise return with no pass error.
        if (mjdt <= mjdt_end_window)
        {
            // Store pass step.
            steps.push_back(SpaceObjectPassStep(std::move(pred)));
        }
        else
        {
            pass = {};
            return ResultCode::NO_NEXT_PASS_FOUND;
        }
    }

    // Look for the end of the pass.
    do
    {
        pred_error = this->predictor_->predict(mjdt, pred);
        if (0 != pred_error)
        {
            pass = {};
            return ResultCode::SOME_PREDICTIONS_NOT_VALID;
        }
        // Store pass step.
        steps.push_back(SpaceObjectPassStep(std::move(pred)));
        mjdt += time_step_sec;
    } while (steps.back().slr_pred.instant_data->altaz_coord.el >
                 this->min_elev_ && mjdt <= mjdt_end_window);

    // Delete last element, since it is outside of pass in some situations.
    steps.pop_back();

    // Update az and el rate.
    // for (auto it = pass.steps.begin() + 1; it != pass.steps.end(); it++)
    // {
    //     it->azim_rate = std::abs( (it->altaz_coord.az - (it - 1)->altaz_coord.az) / interval_sec );
    //     it->elev_rate = std::abs( (it->altaz_coord.el - (it - 1)->altaz_coord.el) / interval_sec );
    // }

    return PassCalculator::ResultCode::NOT_ERROR;
}

PassCalculator::ResultCode PassCalculator::getNextPass(const timing::dates::MJDateTime& mjd_start,
                                                       unsigned pass_limit_minutes,
                                                       SpaceObjectPass& pass,
                                                       unsigned search_limit_minutes) const
{
    // Clear passes vector
    pass = {};

    // Check if predictor is valid.
    if (!this->predictor_->isReady())
        return ResultCode::PREDICTOR_NOT_VALID;

    // Check if time is inside valid prediction window.
    if (!this->predictor_->isInsideTime(mjd_start))
        return ResultCode::TIME_OUTSIDE_OF_PREDICTOR;

    // Declare the times.
    timing::dates::MJDateTime mjdt_start_window, mjdt_end_window, mjdt_end_search;

    // Get the time window.
    this->predictor_->getAvailableTimeWindow(mjdt_start_window, mjdt_end_window);

    // Apply the search limit if set. Otherwise search limit is predictor time window.
    if (search_limit_minutes > 0)
    {
        mjdt_end_search = mjd_start + (search_limit_minutes * 60);

        // If search limit is above predictor window end, then set the search limit to the window end.
        if (mjdt_end_search > mjdt_end_window)
            mjdt_end_search = mjdt_end_window;
    }
    else
        mjdt_end_search = mjdt_end_window;

    // Calculate all the predictions.
    predictors::PredictionSLR pred;
    auto pred_error = this->predictor_->predict(mjd_start, pred);

    // Check the initial prediction.
    if (0 != pred_error)
        return ResultCode::SOME_PREDICTIONS_NOT_VALID;

    // Auxiliary variables.
    math::units::Seconds time_step_sec = this->time_step_ / 1000.0L;
    bool pass_started = pred.instant_data->altaz_coord.el > this->min_elev_;
    pass.time_step = this->time_step_;
    pass.min_elev = this->min_elev_;
    std::vector<SpaceObjectPassStep>& steps = pass.steps;
    timing::dates::MJDateTime mjdt;

    // TODO ADD IN STEPS THE RATES.
    // If pass is started we will look first for the end until the limit. If the limit

    // Check if the pass already started.
    if(pass_started)
    {

        // Insert start time position, since it is inside pass
        steps.push_back(SpaceObjectPassStep(std::move(pred)));

        // Set time to position after start, to look for the end of the pass.
        mjdt = mjd_start + time_step_sec;

        // Set start trimmed to true, since the pass has been trimmed at the start.
        pass.start_trimmed = true;
    }
    else
    {
        // Look for the start of the pass going forward from start.
        mjdt = mjd_start + time_step_sec;
        do
        {
            pred_error = this->predictor_->predict(mjdt, pred);
            if (0 != pred_error)
            {
                pass = {};
                return ResultCode::SOME_PREDICTIONS_NOT_VALID;
            }
            mjdt += time_step_sec;
        } while (pred.instant_data->altaz_coord.el < this->min_elev_ && mjdt <= mjdt_end_search);

        // If search end time has not been reached, start was found. Otherwise return with no pass error.
        if (mjdt <= mjdt_end_search)
        {
            // Store pass step.
            steps.push_back(SpaceObjectPassStep(std::move(pred)));
        }
        else
        {
            pass = {};
            return ResultCode::NO_NEXT_PASS_FOUND;
        }
    }

    // Update end time. The end time will be the minimum between the available predictor time window and
    // the pass duration limit.
    auto mjdt_end_limit = steps.front().mjdt + (pass_limit_minutes*60);
    if (mjdt_end_limit < mjdt_end_window)
        mjdt_end_window = mjdt_end_limit;

    // Look for the end of the pass. This pass can end when it goes below minimum elevation or when time limit is reached
    do
    {
        pred_error = this->predictor_->predict(mjdt, pred);
        if (0 != pred_error)
        {
            pass = {};
            return ResultCode::SOME_PREDICTIONS_NOT_VALID;
        }
        // Store pass step.
        steps.push_back(SpaceObjectPassStep(std::move(pred)));
        mjdt += time_step_sec;
    } while (steps.back().slr_pred.instant_data->altaz_coord.el > this->min_elev_ && mjdt <= mjdt_end_window );

    // If pass limit or predictor time window end was reached, set end trimmed flag.
    if (mjdt > mjdt_end_window)
        pass.end_trimmed = true;

    // Delete last element if it is outside of pass.
    if (steps.back().altaz_coord.el < this->min_elev_)
        steps.pop_back();

    // Update az and el rate.
    // for (auto it = pass.steps.begin() + 1; it != pass.steps.end(); it++)
    // {
    //     it->azim_rate = std::abs( (it->altaz_coord.az - (it - 1)->altaz_coord.az) / interval_sec );
    //     it->elev_rate = std::abs( (it->altaz_coord.el - (it - 1)->altaz_coord.el) / interval_sec );
    // }

    return PassCalculator::ResultCode::NOT_ERROR;
}

bool PassCalculator::isInsidePass(const timing::dates::MJDateTime &mjd) const
{
    // If predictor is not valid or time is outside of prediction time window, return false.
    if (!this->predictor_->isReady() || !this->predictor_->isInsideTime(mjd))
        return false;

    // Calculate all the predictions.
    predictors::PredictionSLR pred;
    auto pred_error = this->predictor_->predict(mjd, pred);

    // If there was an error at prediction, return false.
    if (0 != pred_error)
        return false;

    // Return true if elevation of position is above minimum. False otherwise.
    return pred.instant_data->altaz_coord.el >= this->min_elev_;
}

}}} // END NAMESPACES.
// =====================================================================================================================
