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
 * @file datetime.tpp
 * @brief
 * @author Degoras Project Team
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <omp.h>
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Helpers/types/numeric_strong_type.h"
#include "LibDegorasSLR/Mathematics/math.h"
#include "LibDegorasSLR/Timing/time_constants.h"
#include "LibDegorasSLR/Timing/types/time_types.h"
#include "LibDegorasSLR/Mathematics/units/strong_units.h"
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

/**
 * @brief Class for handle datetimes epochs (date and fraction). This is a generic structure for handling different
 * datetime calendars. This class stores:
 * - The date in days since calendar origin
 * - The day fraction, that represents the elapsed fraction of the day.
 * - The second of the day.
 * The difference between different template specializations is the DateType used, that defines the origin of
 * the calendar.
 *
 * This class could be used with inheritance to overwrite the normalize method if necessary.
 *
 */
template <typename DateType>
class DateTime
{

public:

    /**
     * @brief Default constructor for DateTime. Initializes the object with default values (all to zero).
     */
    DateTime();

    /**
     * @brief Constructor with Date and Second Of Day parameters.
     * @param date DateType object representing the date.
     * @param sod Number of seconds in that day.
     */
    DateTime(const DateType& date, const SoD& sod);

    /**
     * @brief Constructor from long double value containing the day and fraction of day combined.
     * @param dt, the current datetime in days since origin with day fraction.
     */
    DateTime(long double dt);

    DateTime(const DateTime& other) = default;

    DateTime(DateTime&& other) = default;

    DateTime& operator=(const DateTime&) = default;

    DateTime& operator=(DateTime&&) = default;

    virtual ~DateTime() = default;

    /**
     * @brief Date getter.
     * @return The current date in days since origin.
     */
    DateType date() const;

    /**
     * @brief Day fraction getter.
     * @return The current elapsed day fraction. In the range of [0,1) days.
     */
    DayFraction fract() const;

    /**
     * @brief Second of day getter.
     * @return The current elapsed second of day in seconds.
     */
    SoD sod() const;

    /**
     * @brief Function to get the date and fractional part together as a long double.
     * @return date and fractional part combined. Precision can be reduced.
     * @warning This function reduces the precision of the fraction.
     */
    long double datetime() const;

    /**
     * @brief Function to add some seconds to this datetime.
     * @param seconds, the seconds that will be added to the datetime. If negative, the time is decremented.
     */
    void add(const Seconds& seconds);

    bool operator==(const DateTime& other) const;

    bool operator<(const DateTime& other) const;

    bool operator<=(const DateTime& other) const;

    bool operator>(const DateTime& other) const;

    bool operator>=(const DateTime& other) const;

    DateTime operator+(const Seconds& seconds) const;

    static std::vector<DateTime> linspaceStep(const DateTime& start,
                                              const DateTime& end,
                                              const Seconds& step);

private:

    // TODO Check for negative days (exception).

    virtual void normalize();

    DateType date_;      ///< Date in days since origin.
    DayFraction fract_;  ///< Decimal fraction of that day (up to nanoseconds resolution in the sense of day fraction).
    SoD sod_;            ///< Number of seconds in that day (up to picoseconds resolution).
};

// External operators.
// ---------------------------------------------------------------------------------------------------------------------
template <typename DateType>
Seconds operator-(const DateTime<DateType>& a, const DateTime<DateType>& b);

template <typename DateType>
Seconds operator+(const DateTime<DateType> &a, const DateTime<DateType> &b);

// ---------------------------------------------------------------------------------------------------------------------

// Aliases.
// ---------------------------------------------------------------------------------------------------------------------
template <typename DateType>
using DateTimes = std::vector<DateTime<DateType>>;
// ---------------------------------------------------------------------------------------------------------------------


template <typename DateType>
DateTime<DateType>::DateTime() :
    date_(0),
    fract_(0),
    sod_(0)
{}

template <typename DateType>
DateTime<DateType>::DateTime(const DateType &date, const SoD &sod) :
    date_(date),
    sod_(sod)
{
    // Normalize.
    this->normalize();
}

template<typename DateType>
DateTime<DateType>::DateTime(long double dt)
{
    long double day;
    this->sod_ = std::modf(dt, &day) * timing::kSecsPerDayL;
    this->date_ = day;

    this->normalize();
}

template <typename DateType>
DateType DateTime<DateType>::date() const
{
    return this->date_;
}

template <typename DateType>
DayFraction DateTime<DateType>::fract() const
{
    return this->fract_;
}

template <typename DateType>
SoD DateTime<DateType>::sod() const
{
    return this->sod_;
}

template <typename DateType>
long double DateTime<DateType>::datetime() const
{
    return static_cast<long double>(this->date_) + this->fract_;
}

template <typename DateType>
void DateTime<DateType>::add(const Seconds &seconds)
{
    sod_ += seconds;
    normalize();
}


template <typename DateType>
DateTimes<DateType> DateTime<DateType>::linspaceStep(const DateTime &start,
                                                     const DateTime &end,
                                                     const Seconds &step)
{
    DateTimes<DateType> result;

    if (math::compareFloating(step, Seconds()) <= 0)
        return result;

    size_t num = static_cast<size_t>(std::ceil((end - start) / step));

    result.resize(num);

#pragma omp parallel for
    for (size_t i = 0; i < num; i++)
    {
        DateTime value = start + step * static_cast<long double>(i);
        result[i] = value;
    }

    return result;
}

template <typename DateType>
bool DateTime<DateType>::operator==(const DateTime &other) const
{
    return this->date_ == other.date() && math::compareFloating(this->sod_, other.sod()) == 0;
}

template <typename DateType>
bool DateTime<DateType>::operator<(const DateTime& other) const
{
    return (this->date() < other.date()) ||
           (this->date() == other.date() && math::compareFloating(this->sod(), other.sod()) < 0);
}

template <typename DateType>
bool DateTime<DateType>::operator<=(const DateTime &other) const
{
    return (this->date() < other.date()) ||
           (this->date() == other.date() && math::compareFloating(this->sod(), other.sod()) <= 0);
}

template <typename DateType>
bool DateTime<DateType>::operator>(const DateTime &other) const
{
    return (this->date() > other.date()) ||
           (this->date() == other.date() && math::compareFloating(this->sod(), other.sod()) > 0);
}

template <typename DateType>
bool DateTime<DateType>::operator>=(const DateTime &other) const
{
    return (this->date() > other.date()) ||
           (this->date() == other.date() && math::compareFloating(this->sod(), other.sod()) >= 0);
}

template <typename DateType>
DateTime<DateType> DateTime<DateType>::operator+(const Seconds &seconds) const
{
    DateTime result = *this;
    result.add(seconds);
    return result;
}

template <typename DateType>
void DateTime<DateType>::normalize()
{
    // Normalize the second of day input (decrement).
    while(math::compareFloating(this->sod_, SoD(timing::kSecsPerDayL)) < 0)
    {
        this->sod_ += timing::kSecsPerDayL;
        this->date_--;
    }

    // Normalize the second of day input (increment).
    while(math::compareFloating(this->sod_, SoD(timing::kSecsPerDayL)) >= 0)
    {
        this->sod_ -= timing::kSecsPerDayL;
        this->date_++;
    }

    // Calculate the fractional part of the day
    this->fract_ = this->sod_ / timing::kSecsPerDayL;
}

// =====================================================================================================================

template <typename DateType>
Seconds operator-(const DateTime<DateType> &a, const DateTime<DateType> &b)
{
    Seconds result;
    result = (a.date()-b.date()) * Seconds(timing::kSecsPerDayL) + (a.sod() - b.sod());
    return result;
}

template <typename DateType>
Seconds operator+(const DateTime<DateType> &a, const DateTime<DateType> &b)
{
    Seconds result;
    result = (a.date()+b.date()) * Seconds(timing::kSecsPerDayL) + (a.sod() + b.sod());
    return result;
}
//======================================================================================================================

}}} // END NAMESPACES.
// =====================================================================================================================
