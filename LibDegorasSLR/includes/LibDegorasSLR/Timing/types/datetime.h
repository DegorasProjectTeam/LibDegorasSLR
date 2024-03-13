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
 * @file datetime.h
 * @brief
 * @author Degoras Project Team
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Timing/types/base_time_types.h"
#include "LibDegorasSLR/Mathematics/units/strong_units.h"
#include "LibDegorasSLR/Helpers/common_aliases_macros.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace timing{
namespace types{
// =====================================================================================================================

/**
 * @brief Class for handle generic datetimes epochs (date, second of day and decimal fraction).
 *
 * This is a generic class for handling different datetime calendars (epochs).The difference between different template
 * specializations is the DateType used, that defines the origin of the calendar. This class could be used with
 * inheritance to overwrite the normalize method if necessary.
 *
 * Due to the need to handle times with resolutions of up to picoseconds, it is impossible to store these times in a
 * single variable. In this way, the datetimes are decomposed into different parts in order to achieve the desired
 * resolution. The parts are:
 *
 * - The date in days since calendar origin.
 * - The decimal day fraction of the day, that represents the elapsed fraction of the day.
 * - The second of the day.
 *
 * The use of the date value and the number of seconds in that day in a separated way will provide a time resolution of
 * picoseconds. The use of the date value and the decimal fractional part of the day in a separated way will provide a
 * time resolution of nanoseconds (in the sense of fraction of the day). The use of the full datetime value (day and
 * fraction in the same long double variable) will provide a time resolution of milliseconds.
 *
 * @note This class could be used with inheritance to overwrite the normalize method if necessary.
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

    DateTime(const DateType& date, const DayFraction& sod);

    /**
     * @brief Constructor from long double value containing the day and fraction of day combined.
     * @param datetime, the current datetime in days since origin with day fraction.
     */
    DateTime(long double datetime);

    M_DEFINE_CTOR_COPY_MOVE_OP_COPY_MOVE(DateTime)

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
     * @warning This function reduces the precision of the fraction. TODO EXPLAIN MORE
     */
    long double datetime() const;

    /**
     * @brief Function to add some seconds to this datetime.
     * @param seconds, the seconds that will be added to the datetime. If negative, the time is decremented.
     */
    void add(const math::units::Seconds& seconds);

    bool operator==(const DateTime& other) const;

    bool operator<(const DateTime& other) const;

    bool operator<=(const DateTime& other) const;

    bool operator>(const DateTime& other) const;

    bool operator>=(const DateTime& other) const;

    DateTime operator+(const math::units::Seconds& seconds) const;

    static std::vector<DateTime> linspaceStep(const DateTime& start,  const DateTime& end,
                                              const math::units::Seconds& step);

    virtual ~DateTime() = default;

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
math::units::Seconds operator-(const DateTime<DateType>& a, const DateTime<DateType>& b);

template <typename DateType>
math::units::Seconds operator+(const DateTime<DateType> &a, const DateTime<DateType> &b);

// ---------------------------------------------------------------------------------------------------------------------

// Aliases.
// ---------------------------------------------------------------------------------------------------------------------
template <typename DateType> using DateTimeV = std::vector<DateTime<DateType>>;
// ---------------------------------------------------------------------------------------------------------------------


}}} // END NAMESPACES.
// =====================================================================================================================

// TEMPLATES INCLUDES
// =====================================================================================================================
#include <LibDegorasSLR/Timing/types/datetime.inl>
// =====================================================================================================================
