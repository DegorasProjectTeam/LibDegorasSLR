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
 * @file time_types.cpp
 * @brief
 * @author Degoras Project Team
 * @copyright EUPL License
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
#include <omp.h>
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Timing/types/j2000_date_time.h"
#include "LibDegorasSLR/Mathematics/math.h"
#include "LibDegorasSLR/Timing/time_constants.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace timing{
namespace types{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using helpers::types::NumericStrongType;
using math::units::Seconds;
// ---------------------------------------------------------------------------------------------------------------------

J2000DateTime::J2000DateTime() :
    j2d_(J2000Date()),
    fract_(DayFraction()),
    sod_(SoD())
{}

J2000Date J2000DateTime::j2d() const
{
    return this->j2d_;
}

DayFraction J2000DateTime::fract() const
{
    return this->fract_;
}

SoD J2000DateTime::sod() const
{
    return this->sod_;
}

long double J2000DateTime::j2dt() const
{
    return static_cast<long double>(this->j2d_) + this->fract_;
}

void J2000DateTime::increment(const Seconds &seconds)
{
    sod_ += seconds;
    normalize();
}

void J2000DateTime::decrement(const SoD &seconds)
{
    sod_ -= seconds;
    normalize();
}

J2000DateTime::J2000DateTime(const J2000Date &date, const SoD &sod) :
    j2d_(date),
    sod_(sod)
{
    // Normalize.
    this->normalize();
}

std::vector<J2000DateTime> J2000DateTime::linspaceStep(const J2000DateTime &start, const J2000DateTime &end,
                                                       const Seconds &step)
{
    std::vector<J2000DateTime> result;

    if (math::compareFloating(step, Seconds()) <= 0)
        return result;

    size_t num = static_cast<size_t>(std::ceil((end - start) / step));

    result.resize(num);

    #pragma omp parallel for
    for (size_t i = 0; i < num; i++)
    {
        J2000DateTime value = start + step * static_cast<long double>(i);
        result[i] = value;
    }

    return result;
}

bool J2000DateTime::operator==(const J2000DateTime &other) const
{
    return this->j2d_ == other.j2d() && math::compareFloating(this->sod_, other.sod()) == 0;
}

bool J2000DateTime::operator<(const J2000DateTime& other) const
{
    return (this->j2d() < other.j2d()) ||
           (this->j2d() == other.j2d() && math::compareFloating(this->sod(), other.sod()) < 0);
}

bool J2000DateTime::operator<=(const J2000DateTime &other) const
{
    return (this->j2d() < other.j2d()) ||
           (this->j2d() == other.j2d() && math::compareFloating(this->sod(), other.sod()) <= 0);
}

bool J2000DateTime::operator>(const J2000DateTime &other) const
{
    return (this->j2d() > other.j2d()) ||
           (this->j2d() == other.j2d() && math::compareFloating(this->sod(), other.sod()) > 0);
}

bool J2000DateTime::operator>=(const J2000DateTime &other) const
{
    return (this->j2d() > other.j2d()) ||
           (this->j2d() == other.j2d() && math::compareFloating(this->sod(), other.sod()) >= 0);
}

J2000DateTime J2000DateTime::operator+(const Seconds &seconds) const
{
    J2000DateTime result = *this;
    result.sod_ += seconds;
    result.normalize();
    return result;
}

void J2000DateTime::normalize()
{
    // Normalize the second of day input (decrement).
    while(math::compareFloating(this->sod_, SoD(timing::kSecsPerDayL)) < 0)
    {
        this->sod_ += timing::kSecsPerDayL;
        this->j2d_--;
    }

    // Normalize the second of day input (increment).
    while(math::compareFloating(this->sod_, SoD(timing::kSecsPerDayL)) >= 0)
    {
        this->sod_ -= timing::kSecsPerDayL;
        this->j2d_++;
    }

    // Calculate the fractional part of the day
    this->fract_ = this->sod_ / timing::kSecsPerDayL;
}

// =====================================================================================================================

Seconds operator-(const J2000DateTime &a, const J2000DateTime &b)
{
    Seconds result;
    result = (a.j2d()-b.j2d()) * Seconds(timing::kSecsPerDayL) + (a.sod() - b.sod());
    return result;
}

Seconds operator+(const J2000DateTime &a, const J2000DateTime &b)
{
    Seconds result;
    result = (a.j2d()+b.j2d()) * Seconds(timing::kSecsPerDayL) + (a.sod() + b.sod());
    return result;
}

// =====================================================================================================================

}}} // END NAMESPACES.
// =====================================================================================================================
