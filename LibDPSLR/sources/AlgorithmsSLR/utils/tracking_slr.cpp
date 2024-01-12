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

TrackingSLR::TrackingSLR(double min_elev, int mjd_start, long double sod_start, PredictorSLR &&predictor) :
    min_elev_(min_elev),
    predictor_(std::move(predictor))
{

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
    this->avoid_sun_ = apply;
}

void TrackingSLR::setSunAvoidAngle(double angle)
{
    this->sun_avoid_angle_ = angle;
}







}}} // END NAMESPACES
// =====================================================================================================================
