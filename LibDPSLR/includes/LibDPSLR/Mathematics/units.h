/***********************************************************************************************************************
 *   LibDPSLR (Degoras Project SLR Library): A libre base library for SLR related developments.                        *                                      *
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
 * @file container_helpers.h
 * @brief This file contains several units related classes.
 * @author Degoras Project Team.
 * @copyright EUPL License
 * @version 2305.1
 * @todo Delete this and use the new units system in units_todo (for the future).
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include <string>
#include <vector>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDPSLR/Mathematics/math_types.h"
#include "LibDPSLR/Astronomical/astro_types.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace math{
namespace units{
// =====================================================================================================================

// CONSTANTS
// =====================================================================================================================
constexpr double kSecToPs = 1e12;
constexpr double kSecToNs = 1e9;
constexpr double kSecToUs = 1e6;
constexpr double kSecToMs = 1e3;
constexpr double kNmToUm = 0.001;
// =====================================================================================================================

// ========== FUNCTIONS ===============================================================================================

template <typename T>
double rad(T degree)
{
    return (degree*(math::pi/180.0));
}

template <typename T>
double deg(T radian)
{
    return (radian*(180.0/math::pi));
}

// =====================================================================================================================

template <typename T = double>
class Angle
{
public:
    enum class Unit
    {
        RADIANS = 0,
        DEGREES = 1
    };

    Angle(T angle = T(), Unit unit = Unit::RADIANS) : angle_(angle), unit_(unit) {}

    inline constexpr operator T() const {return angle_;}

    T convert(Unit unit)
    {
        if (this->unit_ != unit)
        {
            if (unit == Unit::RADIANS)
            {
                this->angle_ *= math::pi / 180.0;
                this->unit_ = unit;
            }
            else if (unit == Unit::DEGREES)
            {
                this->angle_ *= 180.0 / math::pi;
                this->unit_ = unit;
            }
        }
        return this->angle_;
    }

private:
    T angle_;
    Unit unit_;
};


template <typename T = double>
class Distance
{
public:

    enum class Unit
    {
        METRES = 0,
        LIGHT_PS = 1
    };

    Distance(T dist = T(), Unit unit = Unit::METRES, double ratio = 1.0) :
        dist_(dist), unit_(unit), ratio_(ratio) {};

    inline constexpr operator T() const {return dist_ * ratio_;};

    inline double getRatio() const {return this->ratio_;};

    void convert(Unit unit)
    {
        if (this->unit_ != unit)
        {
            if (unit == Unit::LIGHT_PS)
            {
                this->dist_ *= 1.0/astro::kLightPsToM;
                this->unit_ = unit;
            }

            else if (unit == Unit::METRES)
            {
                this->dist_ *= astro::kLightPsToM;
                this->unit_ = unit;
            }

        }
    };

    void convertRatio(double ratio)
    {
        dist_ *= ratio_ / ratio;
        ratio_ = ratio;
    }

private:
    T dist_;
    Unit unit_;
    double ratio_;
};


}}} // END NAMESPACES
// =====================================================================================================================

