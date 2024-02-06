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
 * @version 2402.1
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
using namespace dpslr::timing::common;
// =====================================================================================================================

// TIME STRING FUNCTIONS
//======================================================================================================================

/**
 * Converts a time point to a string representation with specified format and resolution.
 *
 * This function formats a given time point according to the specified strftime-compatible
 * format string. The function allows specifying the time resolution for the fractional part
 * of the seconds.It supports displaying the time in either UTC or local time.
 *
 * @warning This function relies on std::gmtime and std::localtime, which are based on the
 * time_t type. Consequently, it may not correctly handle dates before January 1, 1970, on
 * all systems due to the limitations of time_t and these functions. Users requiring support
 * for dates before the Unix epoch should consider alternative solutions or verify behavior
 * on their target platforms.
 *
 * @param tp The time point to format.
 * @param format The strftime format string for the date and time part.
 * @param resolution The resolution (seconds, milliseconds, microseconds, nanoseconds) for the time part.
 * @param utc Boolean flag to use UTC (true) or local time (false) for formatting.
 * @return A string representation of the time point according to the specified format and resolution.
 *
 * @throws std::runtime_error If there's an error during formatting.
 */
LIBDPSLR_EXPORT std::string timePointToString(const HRTimePointStd& tp,
                                              const std::string& format = "%Y-%m-%dT%H:%M:%S",
                                              TimeResolution resolution = TimeResolution::MILLISECONDS,
                                              bool utc = true);

/**
 * Converts a high-resolution time point to an ISO 8601 formatted string.
 *
 * This function formats a given time point into a string following the ISO 8601 standard. The function
 * allows specifying the time resolution for the fractional part of the seconds. If the `utc`
 * flag is set to true, the formatted string is suffixed with 'Z' to indicate that the time is in
 * Coordinated Universal Time (UTC). If `utc` is false, the function does not qualify the time zone,
 * and the resulting string represents local time, unqualified by a specific time zone.
 *
 * @param tp The high-resolution time point to format.
 * @param resolution The desired resolution for the fractional part of the seconds in the output.
 * @param utc A boolean flag indicating whether the output should be in UTC (true) or
 *            local time (false). Default is true, appending 'Z' to the formatted string.
 * @return A std::string containing the formatted date and time in ISO 8601 format.
 *
 * @warning If `utc` is false, the resulting string represents local time without specifying the
 * time zone. This means that the time zone qualification (e.g., 'Z' for UTC) is omitted, and the time is assumed
 * to be undefined local.
 *
 * @throws std::runtime_error If there's an error during formatting.
 */
LIBDPSLR_EXPORT std::string timePointToIso8601(const HRTimePointStd& tp,
                                               TimeResolution resolution = TimeResolution::MILLISECONDS,
                                               bool utc = true);

/**
 * @brief Generates the current date and time as a string formatted according to ISO 8601.
 *
 * This function captures the current system time and formats it into a standardized
 * ISO 8601 date and time string. The function allows specifying the time resolution for the
 * fractional part of the seconds. If the `utc` flag is set to true, the formatted string is
 * suffixed with 'Z' to indicate that the time is in Coordinated Universal Time (UTC). If `utc`
 * is false, the function does not qualify the time zone, and the resulting string represents
 * local time, unqualified by a specific time zone.
 *
 * @param resolution The desired resolution for the fractional part of the seconds in the output.
 * @param utc A boolean flag indicating whether the output should be in UTC (true) or
 *            local time (false). Default is true, appending 'Z' to the formatted string.
 * @return A std::string containing the formatted date and time in ISO 8601 format.
 *
 * @warning The precision of the system clock and the resolution parameter may affect the
 * accuracy and granularity of the output. Additionally, when using local time (utc=false),
 * the output string does not include timezone information, and users must be aware of
 * the local timezone context.
 */
LIBDPSLR_EXPORT std::string currentISO8601Date(TimeResolution resolution = TimeResolution::MILLISECONDS,
                                               bool utc = true);

LIBDPSLR_EXPORT std::string millisecondsToISO8601Duration(const std::chrono::milliseconds& msecs);

//======================================================================================================================

// ISO 8601 RELATED FUNCTIONS
//======================================================================================================================

/**
 * @brief Parses an ISO 8601 UTC datetime string and converts it to a HRTimePointStd.
 *
 * This function parses a UTC datetime string formatted according to ISO 8601, supporting both extended
 * and basic formats, including up to nanosecond precision, and converts it into a HRTimePointStd time point.
 * The input string must conform to the ISO 8601 formats:
 * - Extended format: "YYYY-MM-DDTHH:MM:SS.sssZ"
 * - Basic format: "YYYYMMDDTHHMMSS.sssZ"
 * In both formats, the fractional seconds (".sss") are optional and can represent milliseconds up to nanoseconds.
 *
 * @param datetime The ISO 8601 UTC datetime string to be parsed, in either extended or basic format.
 * @return HRTimePointStd A time point representing the specified datetime.
 *
 * @throws std::invalid_argument If the input string does not match the ISO 8601 format.
 *
 * @warning This function uses regular expressions, which may not be fully supported or performant on some older
 * compiler versions. Ensure compatibility with your compiler's regex implementation when using this function.
 */
LIBDPSLR_EXPORT HRTimePointStd iso8601DatetimeParserUTC(const std::string& datetime);


LIBDPSLR_EXPORT std::chrono::seconds iso8601DurationParser(const std::string& duration);


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
LIBDPSLR_EXPORT HRTimePointStd mjdtToTp(MJDateTime mjdt);

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
LIBDPSLR_EXPORT void timePointToModifiedJulianDate(const HRTimePointStd &tp, MJDate &mjd,
                                                   unsigned int& second_day, long double& second_fract);

LIBDPSLR_EXPORT void timePointToModifiedJulianDate(const HRTimePointStd &tp, MJDate &mjd, SoD& second_day_fract);

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
LIBDPSLR_EXPORT MJDateTime timePointToModifiedJulianDatetime(const HRTimePointStd &tp);

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
LIBDPSLR_EXPORT long double mjdAndSecsToMjdt(MJDate mjd, SoD seconds);

/**
 * @brief Convert a Modified Julian datetime to a Modified Julian Date and seconds (with decimals).
 * @param mjdt    The Modified Julian datetime in days with decimals.
 * @param mjd     The Modified Julian Date in days.
 * @param seconds The number of seconds with decimals.
 */
LIBDPSLR_EXPORT void MjdtToMjdAndSecs(MJDateTime mjdt, MJDate &mjd, SoD &seconds);

/**
 * @brief Convert a MJD with second of day to a J2000 datetime
 * @param mjd     The Modified Julian Date in days.
 * @param seconds The number of seconds with decimals.
 * @return The J2000 Datetime in days.
 * @warning Using this function can make your timestamp inaccurate. Use only to
 *          work with times where nanoseconds are not important.
 */
LIBDPSLR_EXPORT long double mjdToJ2000Datetime(MJDate mjd, SoD seconds);

/**
 * @brief Convert a modified julian datetime to a J2000 datetime
 * @param mjdt    The Modified Julian datetime in days.
 * @return The J2000 Datetime in days.
 * @warning Using this function can make your timestamp inaccurate. Use only to
 *          work with times where nanoseconds are not important.
 */
LIBDPSLR_EXPORT long double mjdtToJ2000Datetime(MJDateTime mjdt);

/**
 * @brief Checks if a timestamp given by pair MJDate, SoD, is within a time window.
 * @param mjd, The Modified Julian date in days to check if it is inside a time window.
 * @param sod, The Second of Day in seconds to check if it is inside a time window.
 * @param mjd_start, The Modified Julian date in days of time window start.
 * @param sod_start, The Second of Day in seconds of time window start.
 * @param mjd_end, The Modified Julian date in days of time window end.
 * @param sod_end, The Second of Day in seconds of time window start.
 * @return true if time is inside window, false otherwise.
 */
bool mjdInsideTimeWindow(MJDate mjd, SoD sod, MJDate mjd_start, SoD sod_start, MJDate mjd_end, SoD sod_end);

// C++ time point to TLE date (year and fractional day).
LIBDPSLR_EXPORT void timePointToTLEDate(const HRTimePointStd& tp, int& cent_year, long double& day_with_fract);

/**
 * @brief Calculates the number of days since the civil date of 1970-01-01.
 *
 * This function computes the number of days from the Gregorian calendar date
 * specified by the year (y), month (m), and day (d) parameters to the epoch date
 * of 1970-01-01. Negative return values indicate dates prior to 1970-01-01. The
 * input date must be in the civil (Gregorian) calendar.
 *
 * @param y The year of the date, can be any integer representing the year.
 * @param m The month of the date, must be in the range [1, 12].
 * @param d The day of the month, must be in the valid range for the given month and year.
 *
 * @return The number of days since 1970-01-01. Negative values indicate dates before 1970-01-01.
 *
 * @warning The function uses static_assert to ensure that the size of unsigned and int types
 * meets the minimum requirements for the calculation.
 */
LIBDPSLR_EXPORT int daysFromCivil(int y, unsigned m, unsigned d);


//======================================================================================================================














}} // END NAMESPACES.
// =====================================================================================================================
