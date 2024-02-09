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
 * @file time_utils.cpp
 * @brief
 * @author Degoras Project Team
 * @copyright EUPL License
 * @version
***********************************************************************************************************************/

// C++ INCLUDES
//======================================================================================================================
#include <regex>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <stdexcept>
#include <iostream>
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Mathematics/math.h"
#include "LibDegorasSLR/Timing/time_utils.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace timing{
// =====================================================================================================================

// =====================================================================================================================
using dpslr::timing::common::HRTimePointStd;
using dpslr::math::common::pi;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::time_point_cast;
// =====================================================================================================================

// AUXILIAR
// =====================================================================================================================

// =====================================================================================================================

// IMPLEMENTATIONS
// =====================================================================================================================

long long daysFromCivil(int y, unsigned m, unsigned d)
{
    // Check the numeric limits.
    static_assert(std::numeric_limits<unsigned>::digits >= 18,
                  "[LibDegorasSLR,Timing,daysFromCivil] >= 16 bit unsigned integer");
    static_assert(std::numeric_limits<int>::digits >= 20,
                  "[LibDegorasSLR,Timing,daysFromCivil] >= 16 bit signed integer");
    // Calculate the number of days since 1970-01-01.
    y -= m <= 2;
    const int era = (y >= 0 ? y : y-399) / 400;
    const unsigned yoe = static_cast<unsigned>(y - era * 400);   // [0, 399]
    const unsigned doy = (153*(m > 2 ? m-3 : m+9) + 2)/5 + d-1;  // [0, 365]
    const unsigned doe = yoe * 365 + yoe/4 - yoe/100 + doy;      // [0, 146096]
    // Return the result.
    return era * 146097 + static_cast<int>(doe) - 719468;
}

std::string timePointToString(const HRTimePointStd &tp, const std::string& format, TimeResolution resolution,
                              bool utc, bool rm_trailing_zeros)
{
    std::ostringstream ss, frac;
    auto dur = tp.time_since_epoch();
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(dur).count();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count();
    auto sec_ns = secs * 1'000'000'000LL; // Seconds in nanoseconds
    auto remaining_ns = ns - sec_ns; // Remaining nanoseconds after subtracting full seconds

    std::time_t time_t = static_cast<std::time_t>(secs);
    const std::tm* tm = (utc ? std::gmtime(&time_t) : std::localtime(&time_t));
    if (!tm)
    {
        throw std::runtime_error("[LibDegorasSLR,Timing,timePointToString] Error in tm struct.");
    }

    ss << std::put_time(tm, format.c_str());

    if (resolution != TimeResolution::SECONDS)
    {
        long long fraction = 0;
        int width = 0;

        switch (resolution)
        {
        case TimeResolution::MILLISECONDS:
            fraction = remaining_ns / 1'000'000; // Convert nanoseconds to milliseconds
            width = 3;
            break;
        case TimeResolution::MICROSECONDS:
            fraction = remaining_ns / 1'000; // Convert nanoseconds to microseconds
            width = 6;
            break;
        case TimeResolution::NANOSECONDS:
            fraction = remaining_ns;
            width = 9;
            break;
        default:
            break; // This case is handled by the if condition above
        }

        frac << '.' << std::setw(width) << std::setfill('0') << fraction;

        // Remove last 0.
        if(rm_trailing_zeros)
        {
            std::string frac_str = frac.str();
            frac_str.erase(frac_str.find_last_not_of('0') + 1, std::string::npos);
            if (frac_str.back() == '.')
                frac_str.erase(frac_str.length() - 1);
            ss << frac_str;
        }
        else
            ss << frac.str();
    }

    // Return the string.
    return ss.str();
}

std::string timePointToIso8601(const HRTimePointStd& tp, TimeResolution resolution, bool utc, bool rm_trailing_zeros)
{
    std::string result = timePointToString(tp, "%Y-%m-%dT%H:%M:%S", resolution, utc, rm_trailing_zeros);
    if(utc)
        result += 'Z';
    return result;
}

std::string currentISO8601Date(TimeResolution resolution, bool utc, bool rm_trailing_zeros)
{
    auto now = high_resolution_clock::now();
    return timePointToIso8601(now, resolution, utc, rm_trailing_zeros);
}

std::string millisecondsToISO8601Duration(const std::chrono::milliseconds& msecs)
{
    if(msecs.count() == 0)
    {
        return "PT0H0M0S";
    }

    long long hours = msecs.count() / 3600000;
    int minutes = (msecs.count() % 3600000) / 60000;
    double seconds = (msecs.count() % 60000) / 1000.0;

    std::ostringstream os;
    os << "PT" << hours << "H" << minutes << "M";

    // Use modf to separate the seconds into whole and fractional parts
    double fractionalPart;
    double wholeSeconds;
    fractionalPart = std::modf(seconds, &wholeSeconds);

    if (fractionalPart > 0.0) {
        // Temporarily output with fractional seconds, preserving up to 3 decimal places
        std::ostringstream fractionalStream;
        fractionalStream << std::fixed << std::setprecision(3) << seconds;
        std::string fractionalStr = fractionalStream.str();

        // Trim trailing zeros and the decimal point if necessary
        auto pos = fractionalStr.find_last_not_of('0');
        if (pos != std::string::npos) {
            if (fractionalStr[pos] == '.') {
                pos -= 1; // Keep the number integral if the fractional part is effectively zero
            }
            fractionalStr.erase(pos + 1);
        }

        os << fractionalStr << "S";
    } else {
        // Whole seconds only
        os << static_cast<int>(wholeSeconds) << "S";
    }

    return os.str();
}

std::string secondsToISO8601Duration(const std::chrono::seconds& secs)
{
    // Convert seconds to milliseconds
    std::chrono::milliseconds msecs = std::chrono::duration_cast<std::chrono::milliseconds>(secs);
    // Call the original function with the converted duration
    return millisecondsToISO8601Duration(msecs);
}

HRTimePointStd iso8601DatetimeParserUTC(const std::string& datetime)
{
    // Auxiliar variables.
    int y,m,d,h,M, s;
    std::smatch match;

    // Regex.
    std::regex iso8601_regex_extended(R"(^(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})(?:\.(\d+))?Z$)");
    std::regex iso8601_regex_basic(R"(^(\d{4})(\d{2})(\d{2})T(\d{2})(\d{2})(\d{2})(?:\.(\d+))?Z$)");

    // Check the regexes.
    if (!std::regex_search(datetime, match, iso8601_regex_extended))
    {
        if (!std::regex_search(datetime, match, iso8601_regex_basic))
            throw std::invalid_argument("[LibDegorasSLR,Timing,iso8601DatetimeParserUTC] Invalid argument: " + datetime);
    }

    // Get the datetime values.
    y = std::stoi(match[1].str());
    m = std::stoi(match[2].str());
    d = std::stoi(match[3].str());
    h = std::stoi(match[4].str());
    M = std::stoi(match[5].str());
    s = std::stoi(match[6].str());
    std::string fractional_seconds_str = match[7].str();

    // Get the time point.
    auto days_since_epoch = daysFromCivil(y, static_cast<unsigned>(m), static_cast<unsigned>(d));
    HRTimePointStd t = HRClock::time_point(std::chrono::duration<int, std::ratio<86400>>(days_since_epoch));

    // Add the hours, minutes, seconds and milliseconds.
    t += std::chrono::hours(h);
    t += std::chrono::minutes(M);
    t += std::chrono::seconds(s);

    // Process the fractional part.
    if (!fractional_seconds_str.empty())
    {
        long long fractional_seconds = std::stoll(fractional_seconds_str);
        size_t length = fractional_seconds_str.length();

        // Convert fractional seconds to the appropriate duration
        if (length <= 3)
            t += std::chrono::milliseconds(fractional_seconds);
        else if (length <= 6)
            t += std::chrono::microseconds(fractional_seconds);
        else
            t += std::chrono::nanoseconds(fractional_seconds);
    }

    // Return the time point.
    return t;
}

JDateTime timePointToJulianDatetime(const HRTimePointStd &tp)
{
    long long ns_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count();
    long long days_since_epoch_int = ns_since_epoch / (common::kSecsPerDay * common::kNsPerSecond);
    long long ns_in_current_day = ns_since_epoch % static_cast<long long>(common::kSecsPerDay * common::kNsPerSecond);
    long double fractional_day = static_cast<long double>(ns_in_current_day) / (common::kSecsPerDay * common::kNsPerSecond);
    long double jd = static_cast<long double>(days_since_epoch_int) + fractional_day + common::kPosixEpochToJulian;
    return jd;
}

HRTimePointStd julianDatetimeToTimePoint(JDateTime jdt)
{
    // Convert JDT to Unix timestamp (seconds since Unix epoch)
    long double unix_stamp = (jdt + kJulianToPosixEpoch) * kSecsPerDay;
    // Check if the resulting time point is before the Unix epoch
    if (unix_stamp<0)
    {
        throw std::invalid_argument(
            "[LibDegorasSLR,Timing,julianDatetimeToTimePoint] The jdt represent a time before the Unix epoch.");
    }
    duration<long double, std::ratio<common::kSecsPerDay>> unix_days(jdt + common::kJulianToPosixEpoch);
    auto dur = duration_cast<HRTimePointStd::duration>(unix_days);
    return HRTimePointStd(dur);
}

void timePointToJulianDate(const HRTimePointStd& tp, JDate& jd, DayFraction& fraction)
{
    // Calculate total nanoseconds since the Unix epoch.
    auto ns_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count();

    // Calculate total days from the Unix epoch to the Julian date.
    long long days_since_epoch = ns_since_epoch / (common::kNsPerSecond * common::kSecsPerDay);

    // Convert Unix days to Julian Date.
    jd = static_cast<JDate>(days_since_epoch - common::kJulianToPosixEpoch);

    // Calculate the remainder to find the nanoseconds for the current day.
    long long ns_in_current_day = ns_since_epoch % (common::kNsPerSecond * common::kSecsPerDay);

    // Adjust for Julian Date starting from noon. If the time corresponds to the first half of the Julian day,
    // it actually belongs to the previous Julian Date.
    if (ns_in_current_day < common::kNsPerHalfDay)
    {
        ns_in_current_day += common::kNsPerHalfDay;
    }
    else if(ns_in_current_day > common::kNsPerHalfDay)
    {
        jd += 1;
        ns_in_current_day -= common::kNsPerHalfDay;
    }

    // Store the fractional part.
    fraction = ns_in_current_day/static_cast<long double>(common::kNsPerDay);
}

JDate timePointToJulianDate(const HRTimePointStd &tp, SoD& seconds)
{
    DayFraction fraction;
}



JDate timePointToJulianDate(const HRTimePointStd& tp)
{
    // Calculate total nanoseconds since the Unix epoch.
    auto ns_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count();

    // Calculate total days from the Unix epoch to the Julian date.
    long long days_since_epoch = ns_since_epoch / (common::kNsPerSecond * common::kSecsPerDay);

    // Convert Unix days to Julian Date.
    JDate jd = static_cast<JDate>(days_since_epoch - common::kJulianToPosixEpoch);

    // Calculate the remainder to find the nanoseconds for the current day.
    long long ns_in_current_day = ns_since_epoch % (common::kNsPerSecond * common::kSecsPerDay);

    // Adjust for Julian Date starting from noon. If the time corresponds to the first half of the Julian day,
    // it actually belongs to the previous Julian Date.
    if(ns_in_current_day > common::kNsPerHalfDay)
    {
        jd += 1;
    }

    // Return the julian day.
    return jd;
}


HRTimePointStd win32TicksToTimePoint(Windows32Ticks ticks)
{
    const unsigned long long ns = ticks * static_cast<unsigned long long>(common::kNsPerWin32Tick);
    const unsigned long long sec = ns / 1000000000ULL;
    const unsigned long long frc = ns % 1000000000ULL;
    auto offset = common::SecStd(sec + static_cast<unsigned long long>(common::kWin32EpochToPosixEpoch));
    if (offset.count() < 0)
    {
        throw std::invalid_argument(
            "[LibDegorasSLR,Timing,win32TicksToTimePoint] The ticks represent a time before the Unix epoch.");
    }
    auto tp_sec = time_point_cast<common::SecStd>(HRTimePointStd()) +
                  common::SecStd(sec + static_cast<unsigned long long>(common::kWin32EpochToPosixEpoch));
    return tp_sec + common::NsStd(frc);
}







HRTimePointStd julianDateToTimePoint(JDate jd, SoD seconds)
{
    // Calculate total days from Julian Date to Unix epoch (1970-01-01).
    long long days_from_epoch = jd + static_cast<long long>(common::kJulianToPosixEpoch);

    // Now the seconds.
    long long secs_from_epoch = days_from_epoch * common::kSecsPerDay - 43200;
    long long ns_day = static_cast<long long>(seconds*common::kNsPerSecond);
    long long ns_from_epoch = secs_from_epoch*common::kNsPerSecond + ns_day;

    // Check if the resulting time point is before the Unix epoch
    if (ns_from_epoch < 0)
    {
        throw std::invalid_argument(
            "[LibDegorasSLR,Timing,julianDateToTimePoint] The jd represents a time before the Unix epoch.");
    }

    // Return.
    return HRTimePointStd(std::chrono::nanoseconds(ns_from_epoch));
}





HRTimePointStd modifiedJulianDatetimeToTimePoint(common::MJDateTime mjt)
{
    duration<long double, std::ratio<common::kSecsPerDay>> unix_days(
        mjt + common::kModifiedJulianToJulian + common::kJulianToPosixEpoch);
    return HRTimePointStd(duration_cast<HRTimePointStd::duration>(unix_days));
}





long double timePointToSecsDay(const HRTimePointStd &tp)
{
    std::time_t current = HRTimePointStd::clock::to_time_t(tp);
    std::tm* current_date = std::gmtime(&current);
    current_date->tm_hour = 0;
    current_date->tm_min = 0;
    current_date->tm_sec = 0;
    current_date->tm_isdst = 0;
    HRTimePointStd day_start = HRTimePointStd::clock::from_time_t(MKGMTIME(current_date));
    return duration_cast<duration<long double>>(tp - day_start).count();
}

HRTimePointStd tleDateToTimePoint(unsigned int cent_year, long double day_with_fract)
{
    auto now_point = high_resolution_clock::now();
    std::time_t now = high_resolution_clock::to_time_t(now_point);
    std::tm* date = std::gmtime(&now);
    // We set date to last day of the previous year, since
    // day = 1 represents January the 1st, and day = 0 the previous one.
    date->tm_sec = 0;
    date->tm_min = 0;
    date->tm_hour = 0;
    date->tm_mday = 0;
    date->tm_mon = 0;
    // In TLE format, if year is [57,99] it represents 1957-1999, if it is [0,56] it represents 2000-2056.
    date->tm_year = cent_year > 56 ? cent_year : cent_year + 100;
    std::time_t start_cent = MKGMTIME(date);
    auto start_cent_point = high_resolution_clock::from_time_t(start_cent);
    double iday;
    long double fday = std::modf(day_with_fract, &iday);
    duration<int, std::ratio<common::kSecsPerDay>> days_duration(static_cast<int>(iday));
    common::NsStd fract_secs_duration(static_cast<long long int>(fday * 86400000000000.0));
    return time_point_cast<HRTimePointStd::duration>(start_cent_point + days_duration + fract_secs_duration);
}

void timePointToTLEDate(const HRTimePointStd& tp, int& cent_year, long double &day_with_fract)
{
    std::time_t tp_time = std::chrono::system_clock::to_time_t(tp);
    std::tm* date = std::gmtime(&tp_time);
    cent_year = date->tm_year % 100;
    day_with_fract = date->tm_yday + 1;
    date->tm_sec = 0;
    date->tm_min = 0;
    date->tm_hour = 0;
    std::time_t start_day = MKGMTIME(date);
    auto start_day_point = std::chrono::system_clock::from_time_t(start_day);
    std::chrono::duration<double, std::ratio<common::kSecsPerDay>> day_fract(
              std::chrono::duration_cast<std::chrono::duration<double,
                                                         std::ratio<common::kSecsPerDay>>>(tp - start_day_point));
    day_with_fract += day_fract.count();
}



void ydtomd(int year, unsigned int yday, unsigned int& month, unsigned int& mday)
{
    // Set year to offset since 1900
    year -= 1900;

    // Convert to julian date avoiding leap years by multiplying by 4 and dividing by 1461
    // Set year start two months before to avoid leap day
    double jda1900 = static_cast<long>(1461.0 * (year - 1) / 4.0) + 306.0 + yday;
    int tyear = ((4.0*jda1900) - 1.0) / 1461.0;
    int tday = ((4.0*jda1900) + 3.0 - (tyear*1461.0)) / 4.0;

    month = ((5.0*tday) - 3.0) / 153.0;
    mday = ((5.0*tday) + 2.0 - (153.0*(month))) / 5.0;

    // Set year start to the first month
    if (month >= 10)
        month -= 9;
    else
        month += 3;
}

void grtojd(int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute,
            unsigned int second, long long &jd_day, long double &jd_fract)
{
    // Set year to offset since 1900
    year -= 1900;

    // Calculate # days since noon feburary 29, 1900 (julian date=2415078.0)
    if (month <= 2)
        jd_day = static_cast<long long>(1461.0 * (year-1) / 4.0) +
                static_cast<long long>((153.0 * (month+9) + 2.0) / 5.0) + day;
    else
        jd_day = static_cast<long long>(1461.0 * year/4.0) +
                static_cast<long long>((153.0 * (month-3) + 2.0) / 5.0) + day;

    // Add fractional day and the jd for 2/29/1900
    jd_fract = (hour + (minute + second/60.0) / 60.0) / 24.0 + 0.5;
    jd_day += 2415078.0;

    // If jd fract is greater than 1, then add to days and substract to fractional
    long long jd_fract_excess = static_cast<long long>(jd_fract);
    if (jd_fract >= 1.0)
    {
        jd_fract -= jd_fract_excess;
        jd_day += jd_fract_excess;
    }
}

void jdtogr(long long jd_day, long double jd_fract, int &year, unsigned int &month, unsigned int &day,
            unsigned int &hour, unsigned int &minute, unsigned int &second)
{
    // Calculate days and fractional part since 1900
    long double jdfc = jd_fract + 0.5;
    long double jda1900 = jd_day - 2415079.0;

    // Adjust fractional part
    long long jd_fract_excess = static_cast<long long>(jdfc);
    if (jdfc >= 1.0)
    {
        jdfc -= jd_fract_excess;
        jda1900 += jd_fract_excess;
    }

    // Avoid .9999... imprecission
    hour   = jdfc*24.0 + 1.e-10;
    minute = jdfc*1440.0 - hour*60.0 + 1.e-8;
    second = ((jdfc - hour/24.0 - minute/1440.0)*common::kSecsPerDay) + 1.e-8;
    year   = ((4.0*jda1900)-1.0)/1461.0;

    int tday = ((4.0*jda1900) + 3.0 - (year*1461.0))/4.0;
    month = ((5.0*tday) - 3.0) / 153.0;
    day = ((5.0*tday) + 2.0 - (153.0*(month))) / 5.0;

    if (month >= 10)
    {
        month -= 9;
        year++;
    }
    else
    {
        month += 3;
    }

    // Set offset since 1900 to year
    year += 1900;
}

void timePointToModifiedJulianDate(const HRTimePointStd &tp, common::MJDate &mjd, unsigned& second_day,
                                   long double& second_fract)
{
    long double unix_seconds = duration_cast<duration<long double>>(tp.time_since_epoch()).count();
    second_fract = unix_seconds - static_cast<long long>(unix_seconds);
    mjd = static_cast<unsigned>((unix_seconds/common::kSecsPerDay) +
                                common::kPosixEpochToJulian + common::kJulianToModifiedJulian);
    second_day = static_cast<unsigned>(static_cast<long long>(unix_seconds) % common::kSecsPerDay);
}

void timePointToModifiedJulianDate(const HRTimePointStd &tp, MJDate &mjd, SoD& second_day_fract)
{
    unsigned second_day;
    long double second_fract;
    timePointToModifiedJulianDate(tp, mjd, second_day, second_fract);
    second_day_fract += second_day;
}



long double timePointToJ2000Datetime(const HRTimePointStd &tp)
{
    return timePointToJulianDatetime(tp) + common::kJulianToJ2000;
}

common::MJDateTime timePointToModifiedJulianDatetime(const HRTimePointStd &tp)
{
    return timePointToJulianDatetime(tp) + common::kJulianToModifiedJulian;
}

RJDateTime timePointToReducedJulianDatetime(const HRTimePointStd &tp)
{
    return timePointToJulianDatetime(tp) + common::kJulianToReducedJulian;
}





HRTimePointStd dateAndTimeToTp(int y, int m, int d, int h, int min, int s)
{
    tm datetime;
    datetime.tm_year = y - 1900;
    datetime.tm_mon = m - 1;
    datetime.tm_mday = d;
    datetime.tm_hour = h;
    datetime.tm_min = min;
    datetime.tm_sec = s;
    return high_resolution_clock::from_time_t(MKGMTIME(&datetime));
}









void adjMJDAndSecs(MJDate& mjd, SoD& seconds)
{
    if (seconds >= common::kSecsPerDay)
    {
        const int days_add =  static_cast<int>(std::floor(seconds / common::kSecsPerDay));
        mjd += days_add;
        seconds -= days_add*common::kSecsPerDay;
    }
}

long double modifiedJulianDateToModifiedJulianDatetime(common::MJDate mjd, SoD seconds)
{
    if (seconds >= common::kSecsPerDay)
    {
        const int days_add =  static_cast<int>(std::floor(seconds / common::kSecsPerDay));
        mjd += days_add;
        seconds -= days_add*common::kSecsPerDay;
    }

    return mjd + (seconds / static_cast<long double>(common::kSecsPerDay));
}


long double jdtToGmst(long double jdt)
{
    long double t = (jdt - 2451545.0L) / 36525.L;

    long double gmst = -6.2e-6L*t*t*t + 0.093104L*t*t + (876600.0L * 3600.L + 8640184.812866L)*t + 67310.54841L;  // sec

    //360 deg / 86400 s = 1/240, to convert seconds of time to deg, and then convert to rad
    gmst = dpslr::math::normalizeVal(gmst / 240.L * pi / 180.L, 0.L , 2*pi);

    return gmst;
}


long double jdtToLmst(long double jdt, long double lon)
{
    long double gmst = jdtToGmst(jdt);
    long double lmst = dpslr::math::normalizeVal(gmst + lon, 0.L, 2*pi);

    return lmst;
}

long double mjdToJ2000Datetime(MJDate mjd, SoD seconds)
{
    auto mjdt = modifiedJulianDateToModifiedJulianDatetime(mjd, seconds);
    return mjdtToJ2000Datetime(mjdt);
}

long double mjdtToJ2000Datetime(MJDateTime mjdt)
{
    return mjdt + common::kModifiedJulianToJulian + common::kJulianToJ2000;
}

void MjdtToMjdAndSecs(MJDateTime mjdt, common::MJDate &mjd, SoD &seconds)
{
    long double int_part;
    seconds = std::modf(mjdt, &int_part) * common::kSecsPerDay;
    mjd = static_cast<MJDate>(int_part);
}



bool mjdInsideTimeWindow(MJDate mjd, SoD sod, MJDate mjd_start, SoD sod_start, MJDate mjd_end, SoD sod_end)
{
    return mjd >= mjd_start && mjd <= mjd_end &&
           !(mjd == mjd_start && sod < sod_start) && !(mjd == mjd_end && sod > sod_end);
}


long long hhmmssnsToNsDay(unsigned int hour, unsigned int min, unsigned int sec, unsigned int ns)
{
    // Not use exponential to avoid double.
    return hour * 3600000000000LL + min * 60000000000LL + sec * 1000000000LL + ns;
}

long long nsDayTohhmmssns(long long ns_in, unsigned int& hour, unsigned int& min, unsigned int& sec, unsigned int& ns)
{
    // Not use exponential to avoid double.
    auto result = dpslr::math::euclidDivLL(ns_in, 86400000000000ll);
    ns_in -= result.q * 86400000000000ll;
    hour = ns_in / 3600000000000ll;
    ns_in -= hour * 3600000000000ll;
    min = ns_in / 60000000000ll;
    ns_in -= min * 60000000000ll;
    sec = ns_in / 1000000000ll;
    ns = ns_in % 1000000000ll;
    return  result.q;
}


}}// END NAMESPACES.
// =====================================================================================================================
