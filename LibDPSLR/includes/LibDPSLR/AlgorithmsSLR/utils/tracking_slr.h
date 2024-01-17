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
 * @file tracking_slr.h
 * @author Degoras Project Team.
 * @brief This file contains the definition of the TrackingSLR class.
 * @copyright EUPL License
 * @version 2306.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDPSLR/libdpslr_global.h"
#include "LibDPSLR/AlgorithmsSLR/utils/predictor_slr.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace algoslr{
namespace utils{
// =====================================================================================================================

// =====================================================================================================================
using dpslr::algoslr::utils::PredictorSLR;
// =====================================================================================================================

/**
 * @brief The TrackingSLR class
 */
class LIBDPSLR_EXPORT TrackingSLR
{
public:

    TrackingSLR(double min_elev, unsigned int mjd_start, long double sod_start, PredictorSLR&& predictor,
                bool avoid_sun = true, double sun_avoid_angle = 15.);

    bool isValid() const;
    double minElev() const;
    void getTrackingStart(unsigned int &mjd, long double& sod) const;
    void getTrackingEnd(unsigned int &mjd, long double& sod) const;
    bool getSunAvoidApplied() const;
    double getSunAvoidAngle() const;

    void setSunAvoidApplied(bool apply);
    void setSunAvoidAngle(double angle);

private:

    void analyzeTrack(unsigned int mjd_start, long double sod_start);
    void analyzeSunOverlapping(unsigned int mjd, long double sod);

    double min_elev_;

    unsigned int mjd_start_;
    long double sod_start_;
    unsigned int mjd_end_;
    long double sod_end_;

    bool valid_pass_;
    bool avoid_sun_;
    double sun_avoid_angle_;

    PredictorSLR predictor_;


};

}}} // END NAMESPACES
// =====================================================================================================================
