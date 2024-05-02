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
 * @file datetime_types.h
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

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Timing/dates/datetime.h"
#include "LibDegorasSLR/Timing/dates/base_date_types.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace timing{
namespace dates{
// =====================================================================================================================

/**
 * @brief Represents a specific point in time based on the J2000 epoch.
 *
 * The J2000 epoch is a standard astronomical reference epoch used in the field of astronomy and celestial mechanics,
 * representing the moment 12:00 TT (Terrestrial Time) on January 1, 2000. This time mark is crucial as it serves as a
 * reference point for astronomical calculations. This type uses the `DateTime` template instantiated with `J2000Date`
 * to represent moments in time with respect to the J2000 epoch.
 */
using J2000DateTime = DateTime<J2000Date>;

/**
 * @brief Represents a specific point in time based on the Julian Date system.
 *
 * Julian Date is a count of days since the beginning of the Julian Period, starting from 12:00 noon Universal Time
 * on January 1, 4713 BCE (Julian calendar). This time mark is crucial as it serves as a reference point for
 * astronomical calculations. This type uses the `DateTime` template instantiated with `JDate` to represent moments
 * in time within the Julian Date system.
 */
using JDateTime = DateTime<JDate>;

/**
 * @brief Represents a specific point in time based on the Modified Julian Date system.
 *
 * Modified Julian Date (MJD) is a variant of the Julian Date that adjusts the start point to midnight at the
 * beginning of November 17, 1858. The switch to midnight as the reference starting hour simplifies the use of MJD
 * in various astronomical calculations. This type uses the `DateTime` template instantiated with `MJDate` to represent
 * moments in time within the MJD system.
 */
using MJDateTime = DateTime<MJDate>;

/**
 * @brief Represents a specific point in time based on a Reduced Julian Date system.
 *
 * Reduced Julian Date is relative to noon on November 16, 1858. Reduced Julian Date is designed to simplify certain
 * calculations by adjusting the Julian Date's starting point. This type uses the `DateTime` template instantiated
 * with `RJDate` to represent moments in time within the Reduced Julian Date system.
 */
using RJDateTime = DateTime<RJDate>;

/// Alias for a vector of `J2000DateTime` objects.
using J2000DateTimeV = DateTimeV<J2000Date>;

/// Alias for a vector of `JDateTime` objects.
using JDateTimeV = DateTimeV<JDate>;

/// Alias for a vector of `MJDateTime` objects.
using MJDateTimeV = DateTimeV<MJDate>;

/// Alias for a vector of `RJDateTime` objects.
using RJDateTimeV = DateTimeV<RJDate>;

}}} // END NAMESPACES.
// =====================================================================================================================
