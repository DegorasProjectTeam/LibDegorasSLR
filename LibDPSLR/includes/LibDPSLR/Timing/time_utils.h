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
 * @file TimeUtils.h
 * @brief
 * @author Degoras Project Team
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include <string>
#include <chrono>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDPSLR/libdpslr_global.h"
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

// CONVENIENT ALIAS AND ENUMERATIONS
//======================================================================================================================

/// High resolution time point to store datetimes (uses Unix Time).
using HRTimePointStd = std::chrono::time_point<std::chrono::high_resolution_clock>;

/// Normal time point to store datetimes (uses Unix Time).
using TimePointStd = std::chrono::time_point<std::chrono::system_clock>;

/// Short way of referring to seconds.
using SecStd = std::chrono::seconds;

/// Short way of referring to milliseconds.
using MsStd = std::chrono::milliseconds;

/// Short way of referring to microseconds.
using UsStd = std::chrono::microseconds;

/// Short way of referring to nanoseconds.
using NsStd = std::chrono::nanoseconds;

enum class TimeResolution
{
    SECONDS,
    MILLISECONDS,
    MICROSECOND,
    NANOSECOND,
    PICOSECOND
};

//======================================================================================================================

// CONSTANTS
//======================================================================================================================
constexpr double kModifiedJulianToJulian = 2400000.5;
constexpr double kJulianToModifiedJulian = -2400000.5;
constexpr double kJulianToReducedJulian = -2400000.0;
constexpr double kJulianToJ2000 = -2451545.0;
constexpr double kJ2000ToJulian = 2451545.0;
constexpr double kPosixEpochToJulian = 2440587.5;
constexpr double kJulianToPosixEpoch = -2440587.5;
constexpr long long kNsPerWin32Tick = 100ULL;
constexpr long long kWin32EpochToPosixEpoch = -11644473600ULL;
//======================================================================================================================

// STRING FUNCTIONS
//======================================================================================================================

LIBDPSLR_EXPORT std::string timePointToString(const HRTimePointStd& tp, const std::string& format = "%Y-%m-%dT%H:%M:%S",
                                              bool add_ms = true, bool add_ns = false, bool utc = true);

LIBDPSLR_EXPORT std::string timePointToIso8601(const HRTimePointStd& tp, bool add_ms = true, bool add_ns = false);

LIBDPSLR_EXPORT std::string currentISO8601Date(bool add_ms = true);

LIBDPSLR_EXPORT std::string currentUTCISODate();

//======================================================================================================================

// ISO 8601 RELATED FUNCTIONS
//======================================================================================================================

std::chrono::seconds iso8601DurationParser(const std::string& duration);

HRTimePointStd iso8601DatetimeParser(const std::string& datetime);

//======================================================================================================================

// TIMING CONVERSION FUNCTIONS
//======================================================================================================================

/**
 * @brief Converts Win32 ticks to a std::chrono::time_point with nanosecond precision.
 *
 * This function converts the given Win32 ticks to a std::chrono::time_point using the high-resolution clock.
 * Win32 ticks represent the number of 100-nanosecond intervals that have elapsed since January 1, 1601 (UTC).
 * The resulting time_point has nanosecond precision and represents the corresponding time in the POSIX epoch
 * (number of seconds that have elapsed since January 1, 1970).
 *
 * @param ticks The number of Win32 ticks.
 *
 * @return A std::chrono::time_point representing the corresponding time.
 *
 * @note The conversion assumes that the provided ticks are based on the Win32 epoch.
 *
 * @par Example:
 *
 * Ticks: 133298362768913494
 * Datetime: 2023-05-29T12:17:56.891349400Z
 *
 * More times to test:
 * https://stackoverflow.com/questions/10849717/what-is-the-significance-of-january-1-1601
 */
LIBDPSLR_EXPORT HRTimePointStd win32TicksToTimePoint(unsigned long long ticks);


LIBDPSLR_EXPORT HRTimePointStd dateTimeToTimePoint(int y, int m, int d, int h=0, int min=0, int s=0);

LIBDPSLR_EXPORT long double timePointToSecsDay(const HRTimePointStd& tp);

// TLE time (year and fractional point) to C++ time point.
LIBDPSLR_EXPORT HRTimePointStd tleDateToTimePoint(unsigned int cent_year, long double day_with_fract);

// C++ time point to TLE date (year and fractional day).
LIBDPSLR_EXPORT void timePointToTLEDate(const HRTimePointStd& tp, int& cent_year, long double& day_with_fract);

// Transforms hours, mins, seconds and nanoseconds to day nanoseconds.
LIBDPSLR_EXPORT long long hhmmssnsToNsDay(unsigned int hour, unsigned int min, unsigned int sec, unsigned int ns);

// Transforms day nanoseconds to hours, mins, seconds and nanoseconds.
// Returns the days offset as long long.
LIBDPSLR_EXPORT long long nsDayTohhmmssns(long long ns_in, unsigned int& hour_out, unsigned int& min_out,
                                        unsigned int& sec_out, unsigned int& ns_out);

// Transforms the day of the year to the month and day of the month.
// Not works with the 2100, etc year (but we will be dead xD)
LIBDPSLR_EXPORT void ydtomd(int year, unsigned int yday, unsigned int& month, unsigned int& mday);

/**
 * @brief Converts a Gregorian Date to Julian.
 *
 * @param year, Gregorian year.
 * @param month, in range [1,12].
 * @param day, in range [1,31].
 * @param hour, in range [0,23].
 * @param minute, in range [0,59].
 * @param second, in range [0,59].
 * @param jd_day, Julian day. Output.
 * @param jd_fract, Julian fraction of day. Output.
 *
 * @note Not works with the 2100, etc year (but we will be dead xD).
 */
LIBDPSLR_EXPORT void grtojd(int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute,
                          unsigned int sec, long long &jd_day, double &jd_fract);

/**
 * @brief Converts a Julian Date to Gregorian
 *
 * @param jd_day, Julian day
 * @param jd_fract, Julian fraction of day
 * @param year, Gregorian year. Output.
 * @param month, in range [1,12]. Output.
 * @param day, in range [1,31]. Output.
 * @param hour, in range [0,23]. Output.
 * @param minute, in range [0,59]. Output.
 * @param second, in range [0,59]. Output.
 *
 * @note Not works with the 2100, etc year (but we will be dead xD).
 */
LIBDPSLR_EXPORT void jdtogr(long long jd_day, double jd_fract,int &year, unsigned int &month,
                          unsigned int &day, unsigned int &hour, unsigned int &minute, unsigned int &second);

/**
 * @brief Convert a TimePoint to Modified Julian Date
 * @param tp, TimePoint to convert. Input param.
 * @param mjd, Modified Julian day. Output param.
 * @param second_day, Modified Julian second of day. Output param.
 * @param second_fract, Fraction of second. Precission up to ns. Output param.
 */
LIBDPSLR_EXPORT void timePointToModifiedJulianDate(const HRTimePointStd &tp, unsigned int &mjd,
                                                 unsigned int& second_day, double& second_fract);

/**
 * @brief Converts a TimePoint to Julian Datetime
 * @param tp, TimePoint to convert.
 * @return A floating point value representing Julian Datetime in days. Precission up to ns.
 */
LIBDPSLR_EXPORT long double timePointToJulianDatetime(const HRTimePointStd &tp);

/**
 * @brief Converts a TimePoint to J2000 Datetime
 * @param tp, TimePoint to convert.
 * @return A floating point value representing J2000 Datetime in days. Precission up to ns.
 */
LIBDPSLR_EXPORT long double timePointToJ2000Datetime(const HRTimePointStd &tp);

/**
 * @brief Converts a TimePoint to Modified Julian Datetime
 * @param tp, TimePoint to convert.
 * @return A floating point value representing Modified Julian Datetime in days. Precission up to ns.
 */
LIBDPSLR_EXPORT long double timePointToModifiedJulianDatetime(const HRTimePointStd &tp);

/**
 * @brief Converts a TimePoint to Reduced Julian Datetime
 * @param tp, TimePoint to convert.
 * @return A floating point value representing Reduced Julian Datetime in days. Precission up to ns.
 */
LIBDPSLR_EXPORT long double timePointToReducedJulianDatetime(const HRTimePointStd &tp);

/**
 * @brief Converts a Modified Julian Datetime to a TimePoint.
 * @param mjt, Modified Julian Datetime in days. Precission up to ns. Must be within TimePoint era.
 * @return A TimePoint.
 */
LIBDPSLR_EXPORT HRTimePointStd modifiedJulianDatetimeToTimePoint(long double mjt);

/**
 * @brief Converts a Julian Datetime to a TimePoint.
 * @param mjt, Modified Julian Datetime in days. Precission up to ns. Must be within TimePoint era.
 * @return A TimePoint.
 */
LIBDPSLR_EXPORT HRTimePointStd julianToTimePoint(long double jt);

//

}} // END NAMESPACES.
// =====================================================================================================================
