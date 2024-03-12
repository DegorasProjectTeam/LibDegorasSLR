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
 * @file
 * @brief
 * @author Degoras Project Team.
 * @copyright EUPL License
 * @todo Use the new units system in units_todo (for the future).
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include"LibDegorasSLR/Mathematics/types/numeric_strong_type.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace math{
namespace units{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using types::NumericStrongType;
// ---------------------------------------------------------------------------------------------------------------------

// ALIASES FOR STRONG TYPE UNITS
// ---------------------------------------------------------------------------------------------------------------------

/// Alias for strong type long double representing seconds.
using Seconds = NumericStrongType<long double, struct SecondsTag>;

/// Alias for strong type unsigned representing seconds.
using SecondsU = NumericStrongType<unsigned, struct SecondsUTag>;

/// Alias for strong type long double representing milliseconds.
using Milliseconds = NumericStrongType<long double, struct MillisecondsTag>;

/// Alias for strong type unsigned representing milliseconds.
using MillisecondsU = NumericStrongType<unsigned, struct MillisecondsUTag>;

/// Alias for strong type long double representing microseconds.
using Microseconds = NumericStrongType<long double, struct MicrosecondsTag>;

/// Alias for strong type unsigned representing microseconds.
using MicrosecondsU = NumericStrongType<unsigned, struct MicrosecondsUTag>;

/// Alias for strong type long double representing nanoseconds.
using Nanoseconds = NumericStrongType<long double, struct NanosecondsTag>;

/// Alias for strong type unsigned representing nanoseconds.
using NanosecondsU = NumericStrongType<unsigned, struct NanosecondsUTag>;

/// Alias for strong type long double representing picoseconds.
using Picoseconds = NumericStrongType<long double, struct PicosecondsTag>;

/// Alias for strong type unsigned representing picoseconds.
using PicosecondsU = NumericStrongType<unsigned, struct PicosecondsUTag>;

/// Alias for strong type long double representing meters.
using Meters = NumericStrongType<long double, struct MetersTag>;

/// Alias for strong type long double representing degrees.
using Degrees = NumericStrongType<long double, struct DegreesTag>;

/// Alias for strong type unsigned representing degrees.
using DegreesU = NumericStrongType<unsigned, struct DegreesUTag>;

/// Alias for strong type long double representing radians.
using Radians = NumericStrongType<long double, struct RadiansTag>;

/// Alias for strong type long double representing meters/second.
using MetersSecond = NumericStrongType<long double, struct MetersSecondTag>;

}}} // END NAMESPACES
// =====================================================================================================================

