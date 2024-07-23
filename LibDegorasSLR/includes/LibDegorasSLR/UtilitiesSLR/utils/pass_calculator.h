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
 * @file pass_calculator.h
 * @author Degoras Project Team
 * @brief This file contains the declaration of the PassCalculator class.
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <vector>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/UtilitiesSLR/predictors/predictor_slr_base.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace slr{
namespace utils{
// =====================================================================================================================

// TODO COMPLETE ALL THIS INFORMATION

struct LIBDPSLR_EXPORT SpaceObjectPassStep
{
    SpaceObjectPassStep() = default;

    SpaceObjectPassStep(predictors::PredictionSLR&& pred,
                        long double azim_ra = 0,
                        long double elev_ra = 0,
                        long double azim_accel = 0,
                        long double elev_accel = 0) :
        slr_pred(pred),
        mjdt(this->slr_pred.instant_data->mjdt),
        altaz_coord(this->slr_pred.instant_data->altaz_coord),
        azim_rate(azim_ra),
        elev_rate(elev_ra),
        azim_accel(azim_accel),
        elev_accel(elev_accel)
    {}

    predictors::PredictionSLR slr_pred;  ///< Full SLR prediction computed data.
    timing::dates::MJDateTime mjdt;      ///< Modified julian datetime asociated to the step.
    astro::types::AltAzPos altaz_coord;  ///< Fast access to InstantData local computed altazimuth coords in degrees.
    ///@warning If step is too wide, rates could be incorrect.
    long double azim_rate;               ///< Azimuth rate of step in deg/s. It is the velocity change in abs. value.
    long double elev_rate;               ///< Elevation rate of step in deg/s. It is the velocity change in abs. value.
    long double azim_accel;              ///< Azimuth accel. of step in deg/s2.
    long double elev_accel;              ///< Elevation accel. of step in deg/s2.
};

/**
 * @brief The Pass struct contains data about a space object pass
 */
struct LIBDPSLR_EXPORT SpaceObjectPass
{
    bool isEmpty()
    {
        return steps.empty();
    }

    predictors::PredictionSLRV getPredictionsSLR() const
    {
        predictors::PredictionSLRV predictions(steps.size());

        #pragma omp parallel for
        for (size_t i = 0; i < steps.size(); ++i)
        {
            predictions[i] = steps[i].slr_pred;
        }

        return predictions;
    }

    math::units::MillisecondsU time_step;    ///< Interval between two steps in milliseconds.
    math::units::DegreesU min_elev;          ///< Minimum elevation for pass.
    std::vector<SpaceObjectPassStep> steps;  ///< Steps of the pass.

    bool start_trimmed = false;              ///< True if start was trimmed due to time limitation.
    bool end_trimmed = false;                ///< True if end was trimmed due to time limitation.
};

/**
 * @brief This class implements a utility that calculates passes using a SLR predictor.
 */
class LIBDPSLR_EXPORT PassCalculator
{
public:

    /**
     * @brief ResultCode enum represents the possible results of the operations performed by this class.
     */
    enum ResultCode
    {
        NOT_ERROR,                          ///< There was no error.
        PREDICTOR_NOT_VALID,                ///< The predictor is not ready, so it cannot be used.
        INTERVAL_OUTSIDE_OF_PREDICTOR,      ///< Requested interval for pass search is outside of predictor window.
        TIME_OUTSIDE_OF_PREDICTOR,          ///< Requested time for next pass search is outside of predictor window.
        SOME_PREDICTIONS_NOT_VALID,         ///< There was some errors at predictions.
        NO_NEXT_PASS_FOUND,                 ///< There is no next pass after given datetime.
        OTHER_ERROR                         ///< Other errors.

    };

    /**
     * @brief PassCalculator constructs the pass calculator.
     * @param predictor  The predictor used to calculate the passes.
     * @param min_elev   Minimum elevation of the pass in degrees. By default is 0, i.e., above the horizon.
     * @param time_step  Interval between two steps of the pass in milliseconds.
     */
    PassCalculator(predictors::PredictorSlrPtr predictor, math::units::DegreesU min_elev = 0,
                   math::units::MillisecondsU time_step = 1000.0L);


    /**
     * @brief Setter for minimum elevation.
     * @param min_elev the minimum elevation in degrees.
     */
    void setMinElev(math::units::DegreesU min_elev);
    /**
     * @brief Getter for minimum elevation.
     * @return The minimum elevation in degrees.
     */
    math::units::DegreesU minElev() const;
    /**
     * @brief Setter for time_step.
     * @param time_step  The time_step for interpolation in seconds.
     */
    void setTimeStep(math::units::MillisecondsU time_step);
    /**
     * @brief Getter for interval.
     * @return The interval for interpolation in seconds.
     */
    math::units::MillisecondsU getTimeStep() const;

    /**
     * @brief Get passes within the given interval of time.
     * @param mjd_start  The modified julian date of interval start.
     * @param mjd_end    The modified julian date of interval end.
     * @param passes     The returned passes, or empty if no pass was found.
     * @return The result of the operation.
     */
    ResultCode getPasses(const timing::dates::MJDateTime &mjd_start,
                         const timing::dates::MJDateTime &mjd_end,
                         std::vector<SpaceObjectPass> &passes) const;

    /**
     * @brief Get the next pass, starting from mjd_start datetime. If this datetime is already inside a pass, then
     *        this pass will be returned.
     * @param mjd_start  The datetime to start lookig for next pass.
     * @param pass       The data of the pass. This data is not valid if returned code is different from NOT_ERROR.
     * @return The result of the operation. If the result is different from NOT_ERROR, pass data is not valid.
     */
    ResultCode getNextPass(const timing::dates::MJDateTime &mjd_start, SpaceObjectPass &pass) const;


    /**
     * @brief Get the next pass, starting from mjd_start datetime. If this datetime is already inside a pass, then
     *        this pass will be returned. The returned pass duration will always be less or equal to pass_limit_minutes.
     * @param mjd_start  The datetime to start lookig for next pass.
     * @param pass_limit_minutes
     * @param pass       The data of the pass. This data is not valid if returned code is different from NOT_ERROR.
     * @param search_limit_minutes The time search limit duration in minutes. 0 = no limit. This limit is used only
     *                             for looking for the start. After that pass_limit_minutes is used.
     * @return The result of the operation. If the result is different from NOT_ERROR, pass data is not valid.
     */
    ResultCode getNextPass(const timing::dates::MJDateTime &mjd_start, unsigned int pass_limit_minutes,
                           SpaceObjectPass &pass, unsigned search_limit_minutes = 0) const;

    /**
     * @brief Checks is a given time is inside a pass.
     * @param mjd  The MJ datetime to check.
     * @return True if the datetime is inside of a pass, false if there was some error or the datetime is not inside a pass.
     */
    bool isInsidePass(const timing::dates::MJDateTime& mjd) const;

private:

    math::units::DegreesU min_elev_;
    math::units::MillisecondsU time_step_;
    predictors::PredictorSlrPtr predictor_;
};

}}} // END NAMESPACES.
// =====================================================================================================================
