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
 * @author Degoras Project Team
 * @copyright EUPL License

***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// DEFINITIONS
// =====================================================================================================================
#if defined(__MINGW32__) || defined(_MSC_VER)
#define MKGMTIME _mkgmtime
#else
#define MKGMTIME timegm
#endif
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace timing{
// =====================================================================================================================

// CONSTANTS
// =====================================================================================================================
constexpr long double kModifiedJulianToJulian = 2400000.5L;
constexpr long double kJulianToModifiedJulian = -2400000.5L;
constexpr long double kJulianToReducedJulian = -2400000.0L;
constexpr long double kJulianToJ2000 = -2451545.0L;
constexpr long double kJ2000ToJulian = 2451545.0L;
constexpr long double kPosixEpochToJulian = 2440587.5L;
constexpr long double kJulianToPosixEpoch = -2440587.5L;
constexpr long double kModifiedJulianToPosixEpoch = kModifiedJulianToJulian + kJulianToPosixEpoch;
constexpr long long kNsPerSecond = 1000000000LL;
//
constexpr long long kSecsPerDayLL = 86400LL;
constexpr long double kSecsPerDayL = 86400.0L;
constexpr double kSecsPerDay = 86400.0;
//
constexpr long long kSecsPerHalfDayLL = 43200LL;
constexpr long double kSecsPerHalfDayL = 43200.0L;
//
constexpr long long kNsPerDay = 86400000000000LL;
constexpr long long kNsPerHalfDay = 43200000000000LL;
constexpr long long kNsPerWin32Tick = 100ULL;
constexpr long long kWin32EpochToPosixEpoch = -11644473600LL;
// =====================================================================================================================

}} // END NAMESPACES.
// =====================================================================================================================
