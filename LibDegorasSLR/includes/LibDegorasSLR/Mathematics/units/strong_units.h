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

// LIBRARY INCLUDES
// =====================================================================================================================
#include"LibDegorasSLR/Mathematics/types/numeric_strong_type.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace math{
namespace units{
// =====================================================================================================================

// ALIASES FOR STRONG TYPE UNITS
// ---------------------------------------------------------------------------------------------------------------------

/// Alias for strong type long double representing seconds.
M_DEFINE_STRONG_TYPE_WITH_LITERAL(Seconds, long double, s)

/// Alias for strong type unsigned representing seconds.
M_DEFINE_STRONG_TYPE(SecondsU, unsigned)

/// Alias for strong type long double representing milliseconds.
M_DEFINE_STRONG_TYPE_WITH_LITERAL(Milliseconds, long double, ms)

/// Alias for strong type unsigned representing milliseconds.
M_DEFINE_STRONG_TYPE(MillisecondsU, unsigned)

/// Alias for strong type long double representing microseconds.
M_DEFINE_STRONG_TYPE_WITH_LITERAL(Microseconds, long double, us)

/// Alias for strong type unsigned representing microseconds.
M_DEFINE_STRONG_TYPE(MicrosecondsU, unsigned)

/// Alias for strong type long double representing nanoseconds.
M_DEFINE_STRONG_TYPE_WITH_LITERAL(Nanoseconds, long double, ns)

/// Alias for strong type unsigned representing nanoseconds.
M_DEFINE_STRONG_TYPE(NanosecondsU, unsigned)

/// Alias for strong type long double representing picoseconds.
M_DEFINE_STRONG_TYPE_WITH_LITERAL(Picoseconds, long double, ps)

/// Alias for strong type unsigned representing picoseconds.
M_DEFINE_STRONG_TYPE(PicosecondsU, unsigned)

/// Alias for strong type long double representing meters.
M_DEFINE_STRONG_TYPE_WITH_LITERAL(Meters, long double, m)

/// Alias for strong type long double representing degrees.
M_DEFINE_STRONG_TYPE_WITH_LITERAL(Degrees, long double, deg)

/// Alias for strong type unsigned representing degrees.
M_DEFINE_STRONG_TYPE(DegreesU, unsigned)

/// Alias for strong type long double representing radians.
M_DEFINE_STRONG_TYPE_WITH_LITERAL(Radians, long double, rad)

/// Alias for strong type long double representing meters/second.
using MetersSecond = types::NumericStrongType<long double, struct MetersSecondTag>;

}}} // END NAMESPACES
// =====================================================================================================================

