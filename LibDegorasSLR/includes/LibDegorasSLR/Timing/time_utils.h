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
 * @file time_utils.h
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
#include "LibDegorasSLR/libdpslr_global.h"
#include "LibDegorasSLR/Timing/common/time_types.h"
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

// =====================================================================================================================
using dpslr::timing::common::HRTimePointStd;
using dpslr::timing::common::SoDType;
using dpslr::timing::common::MJDType;
using dpslr::timing::common::MJDtType;

// =====================================================================================================================

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

LIBDPSLR_EXPORT std::chrono::seconds iso8601DurationParser(const std::string& duration);

LIBDPSLR_EXPORT common::HRTimePointStd iso8601DatetimeParser(const std::string& datetime);

//======================================================================================================================

// SIDEREAL RELATED FUNCTIONS
//======================================================================================================================

LIBDPSLR_EXPORT long double jdtToGmst(long double jd);


LIBDPSLR_EXPORT long double jdtToLmst(long double jdt, long double lon);


//======================================================================================================================


// TIMING CONVERSION FUNCTIONS
//======================================================================================================================

/**
 * @brief Converts Win32 ticks to a HRTimePointStd with nanosecond precision.
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

/**
 * @brief Convert a given date and time to a HRTimePointStd.
 *
 * This function converts the provided date and time components to a HRTimePointStd
 * based on the high_resolution_clock clock.
 *
 * @param y   The year.
 * @param m   The month (1-12).
 * @param d   The day of the month.
 * @param h   The hour of the day.
 * @param min The minute of the hour.
 * @param s   The second of the minute.
 * @return The HRTimePointStd representing the specified date and time.
 */
LIBDPSLR_EXPORT HRTimePointStd dateAndTimeToTp(int y, int m, int d, int h=0, int min=0, int s=0);

/**
 * @brief Converts a Modified Julian Datetime to a TimePoint.
 * @param mjdt, Modified Julian Datetime in days. Precission up to ns. Must be within TimePoint era.
 * @return A TimePoint.
 */
LIBDPSLR_EXPORT HRTimePointStd mjdtToTp(MJDtType mjdt);

/**
 * @brief Converts a Julian Datetime to a TimePoint.
 * @param jdt Modified Julian Datetime in days. Precission up to ns. Must be within TimePoint era.
 * @return A TimePoint.
 */
LIBDPSLR_EXPORT HRTimePointStd jdtToTp(long double jdt);


// TLE time (year and fractional point) to C++ time point.
LIBDPSLR_EXPORT HRTimePointStd tleDateToTimePoint(unsigned int cent_year, long double day_with_fract);


LIBDPSLR_EXPORT long double timePointToSecsDay(const HRTimePointStd& tp);



// Transforms hours, mins, seconds and nanoseconds to day nanoseconds.
LIBDPSLR_EXPORT long long hhmmssnsToNsDay(unsigned int hour, unsigned int min, unsigned int sec, unsigned int ns);

// Transforms day nanoseconds to hours, mins, seconds and nanoseconds.
// Returns the days offset as long long.
LIBDPSLR_EXPORT long long nsDayTohhmmssns(long long ns_in, unsigned int& hour_out, unsigned int& min_out,
                                        unsigned int& sec_out, unsigned int& ns_out);

/**
 * @brief Transforms the day of the year to the month and day of the month.
 * @param year The year.
 * @param yday The day of the year.
 * @param month [out] The resulting month.
 * @param mday [out] The resulting day of the month.
 *
 * @warning This function does not work correctly for years like 2100, etc.
 *          It is not designed to handle leap years after the year 2000.
 *          Use with caution for such cases.
 */
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
 * @warning Not works with the 2100, etc year (but we will be dead xD).
 */
LIBDPSLR_EXPORT void grtojd(int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute,
                          unsigned int sec, long long &jd_day, long double &jd_fract);

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
 * @warning Not works with the 2100, etc year (but we will be dead xD).
 */
LIBDPSLR_EXPORT void jdtogr(long long jd_day, long double jd_fract,int &year, unsigned int &month,
                          unsigned int &day, unsigned int &hour, unsigned int &minute, unsigned int &second);

/**
 * @brief Convert a TimePoint to Modified Julian Date
 * @param tp, TimePoint to convert. Input param.
 * @param mjd, Modified Julian day. Output param.
 * @param second_day, Modified Julian second of day. Output param.
 * @param second_fract, Fraction of second. Precission up to ns. Output param.
 */
LIBDPSLR_EXPORT void timePointToModifiedJulianDate(const HRTimePointStd &tp, MJDType &mjd,
                                                   unsigned int& second_day, long double& second_fract);

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
LIBDPSLR_EXPORT MJDtType timePointToModifiedJulianDatetime(const HRTimePointStd &tp);

/**
 * @brief Converts a TimePoint to Reduced Julian Datetime
 * @param tp, TimePoint to convert.
 * @return A floating point value representing Reduced Julian Datetime in days. Precission up to ns.
 */
LIBDPSLR_EXPORT long double timePointToReducedJulianDatetime(const HRTimePointStd &tp);

/**
 * @brief Converts a high-resolution time point to a reduced Julian datetime.
 * @param tp The high-resolution time point.
 * @return The reduced Julian datetime corresponding to the given time point.
 */
LIBDPSLR_EXPORT long double timePointToReducedJulianDatetime(const HRTimePointStd &tp);

/**
 * @brief Convert a Modified Julian Date and seconds (with decimals) to a Modified Julian Datetime.
 * @param mjd     The Modified Julian Date in days.
 * @param seconds The number of seconds with decimals.
 * @return The Modified Julian Datetime.
 * @warning Using this function can make your timestamp inaccurate. Use only to
 *          work with times where nanoseconds are not important.
 */
LIBDPSLR_EXPORT long double mjdAndSecsToMjdt(MJDType mjd, SoDType seconds);

/**
 * @brief Convert a Modified Julian datetime to a Modified Julian Date and seconds (with decimals).
 * @param mjdt    The Modified Julian datetime in days with decimals.
 * @param mjd     The Modified Julian Date in days.
 * @param seconds The number of seconds with decimals.
 */
LIBDPSLR_EXPORT void MjdtToMjdAndSecs(MJDtType mjdt, MJDType &mjd, SoDType &seconds);

/**
 * @brief Convert a MJD with second of day to a J2000 datetime
 * @param mjd     The Modified Julian Date in days.
 * @param seconds The number of seconds with decimals.
 * @return The J2000 Datetime in days.
 * @warning Using this function can make your timestamp inaccurate. Use only to
 *          work with times where nanoseconds are not important.
 */
LIBDPSLR_EXPORT long double mjdToJ2000Datetime(MJDType mjd, SoDType seconds);

/**
 * @brief Convert a modified julian datetime to a J2000 datetime
 * @param mjdt    The Modified Julian datetime in days.
 * @return The J2000 Datetime in days.
 * @warning Using this function can make your timestamp inaccurate. Use only to
 *          work with times where nanoseconds are not important.
 */
LIBDPSLR_EXPORT long double mjdtToJ2000Datetime(MJDtType mjdt);

// SPECIFIC FORMATS FUNCTIONS
//======================================================================================================================

// C++ time point to TLE date (year and fractional day).
LIBDPSLR_EXPORT void timePointToTLEDate(const HRTimePointStd& tp, int& cent_year, long double& day_with_fract);

//======================================================================================================================

}} // END NAMESPACES.
// =====================================================================================================================
