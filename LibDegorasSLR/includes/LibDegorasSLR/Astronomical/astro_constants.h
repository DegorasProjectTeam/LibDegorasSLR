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
 * @file astro_constants.h
 * @author Degoras Project Team.
 * @brief This file contains several astronomical constants.
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
// =====================================================================================================================

// CONSTANTS
// =====================================================================================================================
constexpr long long kSecsSolDay = 86400LL;                  ///< Seconds in a solar day.
constexpr long double kSecsInSiderealDay = 86164.090517L;   ///< Seconds in a sidereal day.
constexpr long double kEarthRotSolDay = 6.30038809866574L;  ///< Earth rotational angular velocity (rad/solar day).
constexpr long double kC = 299792458.0L;                    ///< Speed of light (m/s). IERS Convention 2003.
// =====================================================================================================================

// CONSTANTS FOR FUNCTIONS FRON NOVAS (NAVAL OBSERVATORY VECTOR ASTROMETRY SOFTWARE)
// =====================================================================================================================
constexpr long double T0 = 2451545.00000000L;         ///< TDB Julian date of epoch J2000.0.
constexpr long double AU_SEC = 499.0047838061L;       ///< Light-time for one astronomical unit (AU) in seconds (DE-405).
constexpr long double AU = 1.4959787069098932e+11L;   ///< Astronomical unit in meters.  Value is AU_SEC * C.
// =====================================================================================================================

}} // END NAMESPACES
// =====================================================================================================================
