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

/**
 * @brief The Pass struct contains data about a space object pass
 */
struct LIBDPSLR_EXPORT Pass
{
    struct LIBDPSLR_EXPORT Step
    {
        timing::dates::MJDateTime mjd;    ///< Modified julian date of step.
        math::units::Degrees azim;        ///< Azimuth of the step in degrees.
        math::units::Degrees elev;        ///< Elevation of the step in degrees.
        long double azim_rate;            ///< Azimuth rate of step in deg/s
        long double elev_rate;            ///< Elevation rate of step in deg/s
    };

    math::units::Seconds interval;        ///< Interval between two steps in seconds
    math::units::Degrees min_elev;        ///< Minimum elevation for pass.
    std::vector<Step> steps;              ///< Steps of the pass
};

/**
 * @brief This class implements a utility that calculates passes using a SLR predictor.
 */
class LIBDPSLR_EXPORT PassCalculator
{
public:

    enum ResultCode
    {
        NOT_ERROR,
        PREDICTOR_NOT_VALID,
        INTERVAL_OUTSIDE_OF_PREDICTOR,
        SOME_PREDICTIONS_NOT_VALID,
        OTHER_ERROR

    };

    /**
     * @brief PassCalculator constructs the pass calculator.
     * @param predictor the predictor used to calculate the passes.
     * @param min_elev minimum elevation of the pass in degrees. By default is 0, i.e., above the horizon.
     * @param interval interval between two steps of the pass in seconds. By default is 1 second.
     */
    PassCalculator(predictors::PredictorSlrPtr predictor, math::units::Degrees min_elev = 0,
                   math::units::Seconds interval = 1.L);


    /**
     * @brief Setter for minimum elevation.
     * @param min_elev the minimum elevation in degrees.
     */
    void setMinElev(math::units::Degrees min_elev);
    /**
     * @brief Getter for minimum elevation.
     * @return the minimum elevation in degrees.
     */
    math::units::Degrees minElev() const;
    /**
     * @brief Setter for interval.
     * @param interval the interval for interpolation in seconds.
     */
    void setInterval(math::units::Seconds interval);
    /**
     * @brief Getter for interval.
     * @return the interval for interpolation in seconds.
     */
    math::units::Seconds interval() const;

    /**
     * @brief Get passes within the given interval of time.
     * @param mjd_start the modified julian date of interval start.
     * @param mjd_end the modified julian date of interval end.
     * @param passes the returned passes.
     * @return The result of the operation.
     */
    ResultCode getPasses(const timing::dates::MJDateTime &mjd_start,
                          const timing::dates::MJDateTime &mjd_end,
                          std::vector<Pass> &passes) const;


private:
    math::units::Degrees min_elev_;
    math::units::Seconds interval_;
    predictors::PredictorSlrPtr predictor_;
};


}}} // END NAMESPACES.
// =====================================================================================================================
