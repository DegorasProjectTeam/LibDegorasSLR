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
#include <omp.h>
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Timing/types/datetime.tpp"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace timing{
namespace types{
// =====================================================================================================================

/**
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
using J2000DateTime = DateTime<J2000Date>;
using J2000DateTimes = DateTimes<J2000Date>;

using JDateTime = DateTime<JDate>;
using JDateTimes = DateTimes<JDate>;

using MJDateTime = DateTime<MJDate>;
using MJDateTimes = DateTimes<MJDate>;

using RJDateTime = DateTime<RJDate>;
using RJDateTimes = DateTimes<RJDate>;


}}} // END NAMESPACES.
// =====================================================================================================================
