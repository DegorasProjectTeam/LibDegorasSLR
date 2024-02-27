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
 * @file degoras_time.h
 * @brief
 * @author Degoras Project Team
 * @copyright EUPL License
 * @version
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include <chrono>
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Timing/types/time_types.h"
#include "LibDegorasSLR/Timing/time_utils.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace timing{
// =====================================================================================================================

// The internal time point have nanoseconds precision. The internal storaged time, have picoseconds precisions.
// Dependiendo de que función llames, podrás obtener el tiempo con una precisión u otra.
class LIBDPSLR_EXPORT DegorasTime
{

public:

    // Ns precision
    LIBDPSLR_EXPORT DegorasTime(const HRTimePointStd& tp)
    {
        // TODO
    }

    // Ns precision.
    LIBDPSLR_EXPORT DegorasTime(const std::chrono::nanoseconds& ns)
    {
        // TODO
    }

    // Ns precision.
    LIBDPSLR_EXPORT DegorasTime(const DegorasTime& dego_time)
    {
        this->mjd_ = dego_time.modifiedJulianDate();
        this->sod_ = dego_time.secondsOfDay();
        // TODO tp
    }

    // Ps precision
    LIBDPSLR_EXPORT static DegorasTime fromModifiedJulianDate(const MJDate& date, const SoD& seconds = 0.0L)
    {

    }

    // Ns
    LIBDPSLR_EXPORT static DegorasTime fromSecsSinceUnixEpoch(long long secs)
    {
        return(HRClock::from_time_t(secs));
    }

    // Ns
    LIBDPSLR_EXPORT static DegorasTime fromWin32Ticks(Windows32Ticks ticks)
    {
        return DegorasTime(timing::win32TicksToTimePoint(ticks));
    }

    MJDate modifiedJulianDate() const
    {
        return this->mjd_;
    }

    void modifiedJulianDate(MJDate& date, SoD& sod) const
    {
        date = this->mjd_;
        sod = this->sod_;
    }

    const SoD& secondsOfDay() const
    {
        return this->sod_;
    }

    const HRTimePointStd& highResolutionTimePointSTD() const
    {
        //return timing::modif
    }

private:

    MJDate mjd_;              ///< Modified julian date in days.
    SoD sod_;                 ///< Second of day in that MJD (ps precission -> 12 decimals).

};


}} // END NAMESPACES.
// =====================================================================================================================
