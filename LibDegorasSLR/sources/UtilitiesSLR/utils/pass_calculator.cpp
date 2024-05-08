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

    // Check if predictor is valid and has data for the time window.
    if (this->predictor_->isReady())
        return ResultCode::PREDICTOR_NOT_VALID;

    if (this->predictor_->isInsideTimeWindow(mjd_start, mjd_end))
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

}}} // END NAMESPACES.
// =====================================================================================================================
