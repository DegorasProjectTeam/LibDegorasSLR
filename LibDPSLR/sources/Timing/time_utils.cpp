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
 * @file TimeUtils.cpp
 * @brief
 * @author Degoras Project Team
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDPSLR/Mathematics/math.h"
#include <LibDPSLR/Timing/time_utils.h>
#include <LibDPSLR/Helpers/string_helpers.h>
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
    duration<int, std::ratio<common::kSecsInDay>> days_duration(static_cast<int>(iday));
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
    std::chrono::duration<double, std::ratio<common::kSecsInDay>> day_fract(
              std::chrono::duration_cast<std::chrono::duration<double,
              std::ratio<common::kSecsInDay>>>(tp - start_day_point));
    day_with_fract += day_fract.count();
}

long long hhmmssnsToNsDay(unsigned int hour, unsigned int min, unsigned int sec, unsigned int ns)
{
    // Not use exponential to avoid double.
    return hour * 3600000000000ll + min * 60000000000ll + sec * 1000000000ll + ns;
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
    second = ((jdfc - hour/24.0 - minute/1440.0)*common::kSecsInDay) + 1.e-8;
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

void timePointToModifiedJulianDate(const HRTimePointStd &tp, long long &mjd, unsigned& second_day,
                                   long double& second_fract)
{
    long double unix_seconds = duration_cast<duration<long double>>(tp.time_since_epoch()).count();
    second_fract = unix_seconds - static_cast<long long>(unix_seconds);
    mjd = static_cast<unsigned>((unix_seconds/common::kSecsInDay) +
                                common::kPosixEpochToJulian + common::kJulianToModifiedJulian);
    second_day = static_cast<unsigned>(static_cast<long long>(unix_seconds) % common::kSecsInDay);
}

long double timePointToJulianDatetime(const HRTimePointStd &tp)
{
    long double unix_seconds = duration_cast<duration<long double>>(tp.time_since_epoch()).count();
    long double jd = (unix_seconds/common::kSecsInDay) + common::kPosixEpochToJulian;
    return jd;
}

long double timePointToJ2000Datetime(const HRTimePointStd &tp)
{
    return timePointToJulianDatetime(tp) + common::kJulianToJ2000;
}

long double timePointToModifiedJulianDatetime(const HRTimePointStd &tp)
{
    return timePointToJulianDatetime(tp) + common::kJulianToModifiedJulian;
}

long double timePointToReducedJulianDatetime(const HRTimePointStd &tp)
{
    return timePointToJulianDatetime(tp) + common::kJulianToReducedJulian;
}

HRTimePointStd mjdtToTp(long double mjt)
{
    duration<long double, std::ratio<common::kSecsInDay>> unix_days(
        mjt + common::kModifiedJulianToJulian + common::kJulianToPosixEpoch);
    return HRTimePointStd(duration_cast<HRTimePointStd::duration>(unix_days));
}

HRTimePointStd jdtToTp(long double jt)
{
    duration<long double, std::ratio<common::kSecsInDay>> unix_days(jt + common::kJulianToPosixEpoch);
    return HRTimePointStd(duration_cast<HRTimePointStd::duration>(unix_days));
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

HRTimePointStd win32TicksToTimePoint(unsigned long long ticks)
{
    const unsigned long long ns = ticks * common::kNsPerWin32Tick;
    const unsigned long long sec = ns / 1000000000ULL;
    const unsigned long long frc = ns % 1000000000ULL;
    auto tp_sec = time_point_cast<common::SecStd>(
                      HRTimePointStd()) + common::SecStd(sec + common::kWin32EpochToPosixEpoch);
    return tp_sec + common::NsStd(frc);
}

std::string timePointToString(const HRTimePointStd &tp, const std::string& format,
                              bool add_ms, bool add_ns, bool utc)
{
    // Stream to hold the formatted string and the return container.
    std::ostringstream ss;
    // Convert the time point to a duration and get the different time fractions.
    HRTimePointStd::duration dur = tp.time_since_epoch();
    const time_t secs = duration_cast<common::SecStd>(dur).count();
    const long long mill = duration_cast<common::MsStd>(dur).count();
    const unsigned long long ns = duration_cast<common::NsStd>(dur).count();
    const unsigned long long s_ns = secs * 1e9;
    const unsigned long long t_ns = (ns - s_ns);
    // Format the duration.
    if (const std::tm *tm = (utc ? std::gmtime(&secs) : std::localtime(&secs)))
    {
        ss << std::put_time(tm, format.c_str());
        if(add_ms && !add_ns)
            ss << '.' << std::setw(3) << std::setfill('0') << (mill - secs * 1e3);
        else if(add_ns)
            ss << '.' << std::setw(9) << std::setfill('0') << t_ns;
    }
    else
    {
        // If error, return an empty string.
        return std::string();
    }
    // Return the container.
    return ss.str();
}

std::string timePointToIso8601(const HRTimePointStd& tp, bool add_ms, bool add_ns)
{
    // Return the ISO 8601 datetime.
    return timePointToString(tp, "%Y-%m-%dT%H:%M:%S", add_ms, add_ns) + 'Z';
}


std::string currentISO8601Date(bool add_ms)
{
    auto now = high_resolution_clock::now();
    return timePointToIso8601(now, add_ms);
}

void adjMJDAndSecs(long long& mjd, long double& seconds)
{
    if (seconds >= common::kSecsInDay)
    {
        const int days_add =  static_cast<int>(std::floor(seconds / common::kSecsInDay));
        mjd += days_add;
        seconds -= days_add*common::kSecsInDay;
    }
}

long double mjdAndSecsToMjdt(long long mjd, long double seconds)
{
    if (seconds >= common::kSecsInDay)
    {
        const int days_add =  static_cast<int>(std::floor(seconds / common::kSecsInDay));
        mjd += days_add;
        seconds -= days_add*common::kSecsInDay;
    }

    long double mjdt = mjd + (seconds / static_cast<long double>(common::kSecsInDay));
    return mjdt;
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

long double mjdToJ2000Datetime(long long mjd, long double seconds)
{
    return mjdtToJ2000Datetime(mjdAndSecsToMjdt(mjd, seconds));
}

long double mjdtToJ2000Datetime(long double mjdt)
{
    return mjdt + common::kModifiedJulianToJulian + common::kJulianToJ2000;
}


}}// END NAMESPACES.
// =====================================================================================================================
