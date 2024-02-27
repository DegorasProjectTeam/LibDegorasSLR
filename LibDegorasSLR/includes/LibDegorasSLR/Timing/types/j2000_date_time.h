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
 * @file j2000_date_time.h
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
 * @brief Struct for handle J2000 datetimes epochs (date and fraction).
 *
 * The J2000 epoch is a standard astronomical reference epoch used in the field of astronomy and celestial mechanics.
 * It represents the start of the year 2000 in the Gregorian calendar system and is commonly used as a reference point
 * for astronomical calculations.
 *
 * This struct stores J2000 datetime epochs (date, fraction and number of seconds in that day). The use of the day
 * value (´j2d´) and the number of seconds in that day (`sod`) in a separated way will provide a time resolution of
 * picoseconds. The use of the day value (´j2d´) and the decimal fractional part of the day (´fract´) in a separated
 * way will provide a time resolution of nanoseconds (in the sense of fraction of the day). The use of the full
 * datetime value (day and fraction) directly will provide a time resolution of milliseconds.
 */
struct J2000DateTime
{

public:

    /**
     * @brief Default constructor for J2000DateTime. Initializes the object with default values (all to zero).
     */
    J2000DateTime();

    /**
     * @brief Constructor with J2000Date and Second Of Day parameters.
     * @param date J2000Date object representing the date.
     * @param sod Number of seconds in that day.
     */
    J2000DateTime(const J2000Date& date, const SoD& sod);

    J2000DateTime(const J2000DateTime& other) = default;

    J2000DateTime(J2000DateTime&& other) = default;

    J2000Date j2d() const;

    DayFraction fract() const;

    SoD sod() const;

    /**
     * @brief Function to get the J2000 date and fractional part together as a long double.
     * @return J2000 date and fractional part combined, reduced to milliseconds precision.
     * @warning This function reduces the precision of the time up to milliseconds in the sense of day fraction).
     */
    long double j2dt() const;

    // Function to increment time by seconds
    void increment(const Seconds& seconds);

    // Function to decrement time by seconds
    void decrement(const SoD& seconds);

    J2000DateTime& operator=(const J2000DateTime&) = default;

    J2000DateTime& operator=(J2000DateTime&&) = default;

    bool operator==(const J2000DateTime& other) const;

    bool operator<(const J2000DateTime& other) const;

    bool operator<=(const J2000DateTime& other) const;

    bool operator>(const J2000DateTime& other) const;

    bool operator>=(const J2000DateTime& other) const;

    J2000DateTime operator+(const Seconds& seconds) const;

    static std::vector<J2000DateTime> linspaceStep(const J2000DateTime& start,
                                                   const J2000DateTime& end, const Seconds& step);

private:

    // TODO Check for negative days (exception).

    void normalize();

    J2000Date j2d_;      ///< J2000 Date in days (J2000 = JD 2451545.0).
    DayFraction fract_;  ///< Decimal fraction of that day (up to nanoseconds resolution in the sense of day fraction).
    SoD sod_;            ///< Number of seconds in that day (up to picoseconds resolution).
};

// External operators.
// ---------------------------------------------------------------------------------------------------------------------

Seconds operator-(const J2000DateTime& a, const J2000DateTime& b);

Seconds operator+(const J2000DateTime &a, const J2000DateTime &b);

// ---------------------------------------------------------------------------------------------------------------------

// Aliases.
// ---------------------------------------------------------------------------------------------------------------------
using J2000DateTimes = std::vector<J2000DateTime>;
// ---------------------------------------------------------------------------------------------------------------------

//======================================================================================================================

}}} // END NAMESPACES.
// =====================================================================================================================
