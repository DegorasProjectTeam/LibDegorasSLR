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

PassCalculator::PassCalculator(predictors::PredictorSlrPtr predictor, math::units::Degrees min_elev,
                               math::units::Seconds interval) :
    min_elev_(min_elev),
    interval_(interval),
    predictor_(std::move(predictor))
{

}

void PassCalculator::setMinElev(math::units::Degrees min_elev)
{
    this->min_elev_ = min_elev;
}

math::units::Degrees PassCalculator::minElev() const
{
    return this->min_elev_;
}

void PassCalculator::setInterval(math::units::Seconds interval)
{
    this->interval_ = interval;
}

math::units::Seconds PassCalculator::interval() const
{
    return this->interval_;
}

PassCalculator::ResultCode PassCalculator::getPasses(const timing::dates::MJDateTime &mjd_start,
                                                     const timing::dates::MJDateTime &mjd_end,
                                                     std::vector<Pass> &passes) const
{
    // Clear passes vector
    passes.clear();

    // Check if predictor is valid.
    if (!this->predictor_->isReady())
        return ResultCode::PREDICTOR_NOT_VALID;

    // Check if time interval is inside valid prediction window.
    if (!this->predictor_->isInsideTimeWindow(mjd_start, mjd_end))
        return ResultCode::INTERVAL_OUTSIDE_OF_PREDICTOR;

    // Calculate all the predictions.
    auto predictions = this->predictor_->predict(mjd_start, mjd_end, this->interval_ * 1000.L);

    // Auxiliary variables.
    bool pass_started = false;
    Pass current_pass;
    current_pass.interval = this->interval_;
    current_pass.min_elev = this->min_elev_;
    Pass::Step current_step;

    // Check the predictions to get all the passes within the time window.
    for (const auto &pred : predictions)
    {
        if (0 != pred.error)
        {
            passes.clear();
            return PassCalculator::ResultCode::SOME_PREDICTIONS_NOT_VALID;
        }

        if (pred.instant_data->altaz_coord.el >= this->min_elev_)
        {
            if (!pass_started)
            {
                pass_started = true;
                current_step.azim_rate = 0;
                current_step.elev_rate = 0;
            }
            else
            {
                current_step.azim_rate = (pred.instant_data->altaz_coord.az - current_pass.steps.back().azim) /
                                         this->interval_;
                current_step.elev_rate = (pred.instant_data->altaz_coord.el - current_pass.steps.back().elev) /
                                         this->interval_;
            }
            current_step.mjd = pred.instant_data->mjdt;
            current_step.azim = pred.instant_data->altaz_coord.az;
            current_step.elev = pred.instant_data->altaz_coord.el;
            current_pass.steps.push_back(std::move(current_step));
            current_step = {};

        }
        else if(pass_started)
        {
            pass_started = false;
            passes.push_back(std::move(current_pass));
            current_pass = {};
        }

    }

    // Close pass if it is cut in the middle.
    if (pass_started)
    {
        passes.push_back(current_pass);
    }

    return PassCalculator::ResultCode::NOT_ERROR;
}

PassCalculator::ResultCode PassCalculator::getNextPass(const timing::dates::MJDateTime &mjd_start, Pass &pass) const
{
    // Clear passes vector
    pass = {};

    // Check if predictor is valid.
    if (!this->predictor_->isReady())
        return ResultCode::PREDICTOR_NOT_VALID;

    // Check if time is inside valid prediction window.
    if (!this->predictor_->isInsideTime(mjd_start))
        return ResultCode::TIME_OUTSIDE_OF_PREDICTOR;

    timing::dates::MJDateTime mjdt_start_window, mjdt_end_window;
    this->predictor_->getTimeWindow(mjdt_start_window, mjdt_end_window);

    // Calculate all the predictions.
    predictors::PredictionSLR pred;
    auto pred_error = this->predictor_->predict(mjd_start, pred);

    if (0 != pred_error)
        return ResultCode::SOME_PREDICTIONS_NOT_VALID;

    // Auxiliary variables.
    bool pass_started = pred.instant_data->altaz_coord.el >= this->min_elev_;
    pass.interval = this->interval_;
    pass.min_elev = this->min_elev_;
    std::vector<Pass::Step>& steps = pass.steps;
    timing::dates::MJDateTime mjdt;

    if (pass_started)
    {
        // Insert start time position, since it is inside pass
        steps.push_back({mjdt, pred.instant_data->altaz_coord.az, pred.instant_data->altaz_coord.el, 0, 0});

        // Look for the start of the pass going backwards from start.
        mjdt = mjd_start - this->interval_;
        do
        {
            pred_error = this->predictor_->predict(mjdt, pred);
            if (0 != pred_error)
            {
                pass = {};
                return ResultCode::SOME_PREDICTIONS_NOT_VALID;
            }
            // Store pass step.
            steps.push_back({mjdt, pred.instant_data->altaz_coord.az, pred.instant_data->altaz_coord.el, 0, 0});
            mjdt -= this->interval_;
        } while (pred.instant_data->altaz_coord.el >= this->min_elev_ && mjdt >= mjdt_start_window);

        // If the whole pass is inside the predictor time window.
        if (mjdt >= mjdt_start_window)
        {
            // Delete last element, since it is outside of pass
            steps.pop_back();
        }

        // Put elements in chronological order.
        std::reverse(steps.begin(), steps.end());

        // Set time to position after start, to look for the end of the pass.
        mjdt = mjd_start + this->interval_;

    }
    else
    {
        // Look for the start of the pass going forward from start.
        mjdt = mjd_start + this->interval_;
        do
        {
            pred_error = this->predictor_->predict(mjdt, pred);
            if (0 != pred_error)
            {
                pass = {};
                return ResultCode::SOME_PREDICTIONS_NOT_VALID;
            }
            mjdt += this->interval_;
        } while (pred.instant_data->altaz_coord.el < this->min_elev_ && mjdt <= mjdt_end_window);

        // If end of predictor time window has not been reached, start was found. Otherwise return with no pass error.
        if (mjdt <= mjdt_end_window)
        {
            // Store pass step.
            steps.push_back({mjdt - this->interval_, pred.instant_data->altaz_coord.az,
                             pred.instant_data->altaz_coord.el, 0, 0});
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
        steps.push_back({mjdt, pred.instant_data->altaz_coord.az, pred.instant_data->altaz_coord.el, 0, 0});
        mjdt += this->interval_;
    } while (pred.instant_data->altaz_coord.el >= this->min_elev_ && mjdt <= mjdt_end_window);


    // If the whole pass is inside the predictor time window.
    if (mjdt <= mjdt_end_window)
    {
        // Delete last element, since it is outside of pass
        steps.pop_back();
    }


    // Update az and el rate.
    for (auto it = pass.steps.begin() + 1; it != pass.steps.end(); it++)
    {
        it->azim_rate = (it->azim - (it - 1)->azim) / this->interval_;
        it->elev_rate = (it->elev - (it - 1)->elev) / this->interval_;
    }

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
