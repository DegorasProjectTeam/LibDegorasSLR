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

// C++ INCLUDES
// =====================================================================================================================
#include <regex>
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include <LibDegorasSLR/Modules/Testing>
#include <LibDegorasSLR/Modules/Timing>
// =====================================================================================================================

// NAMESPACES
// ---------------------------------------------------------------------------------------------------------------------
using namespace dpslr;
using namespace dpslr::timing;
// ---------------------------------------------------------------------------------------------------------------------

// UNIT TEST DECLARATIONS
// ---------------------------------------------------------------------------------------------------------------------
M_DECLARE_UNIT_TEST(daysFromCivil)
// --
M_DECLARE_UNIT_TEST(timePointToString)
M_DECLARE_UNIT_TEST(timePointToIso8601)
M_DECLARE_UNIT_TEST(currentISO8601Date)
M_DECLARE_UNIT_TEST(millisecondsToISO8601Duration)
M_DECLARE_UNIT_TEST(secondsToISO8601Duration)
M_DECLARE_UNIT_TEST(iso8601DatetimeParserUTC)
// --
M_DECLARE_UNIT_TEST(win32TicksToTimePoint)
// --
M_DECLARE_UNIT_TEST(timePointToJulianDatetime)
M_DECLARE_UNIT_TEST(julianDatetimeToTimePoint)
// --
M_DECLARE_UNIT_TEST(timePointToJulianDate_fract)
M_DECLARE_UNIT_TEST(timePointToJulianDate)



M_DECLARE_UNIT_TEST(julianDateToTimePoint)

// ---------------------------------------------------------------------------------------------------------------------

// UNIT TESTS IMPLEMENTATIONS
// ---------------------------------------------------------------------------------------------------------------------

// -- Helper functions.

M_DEFINE_UNIT_TEST(daysFromCivil)
{
    // Test cases with inputs: year, month, day, and expected output: days since 1970-01-01.
    std::vector<std::tuple<int, unsigned, unsigned, long long>> cases =
    {
        {1970, 1, 1, 0}, // Unix epoch start
        {1969, 12, 31, -1}, // Day before Unix epoch
        {2020, 5, 29, 18411}, // Sample date provided
        {1601, 1, 1, -134774}, // Start of Win32 epoch, expected negative days
        {2000, 1, 1, 10957}, // Y2K
        {2010, 1, 1, 14610}, // Start of 2010
        {1980, 1, 1, 3652}, // Start of 1980
        {2058, 11, 20, 32465}, // Future date from the example
    };

    for (const auto& [year, month, day, expected] : cases)
    {
        long long result = timing::daysFromCivil(year, month, day);
        M_EXPECTED_EQ(expected, result)
    }
}

// -- Time strings functions.

M_DEFINE_UNIT_TEST(timePointToString)
{
    // Inputs.
    std::chrono::nanoseconds in_1(167253349123456789);
    std::chrono::seconds in_2(1707131464);
    std::chrono::seconds in_3(0);

    // Expected outputs.
    std::string out_1 = "1975-04-20T19:15:49";
    std::string out_2 = "1975-04-20T19:15:49.123";
    std::string out_3 = "1975-04-20T19:15:49.123456";
    std::string out_4 = "1975-04-20T19:15:49.123456789";
    std::string out_5 = "20240205111104";
    std::string out_6 = "1970-01-01T00:00:00";
    std::string out_7 = "January 01, 1970 - 00:00";
    std::string out_8 = "1970-01-01T00:00:00";
    // Trailing.
    std::string out_9 = "January 01, 1970 - 00:00.000";
    std::string out_10 = "20240205111104.000000";
    std::string out_11 = "1970-01-01T00:00:00.000000000";

    // Results.
    std::string res_1, res_2, res_3, res_4, res_5, res_6, res_7, res_8, res_9, res_10, res_11;

    // Time points.
    timing::HRTimePointStd tp_1(in_1);
    timing::HRTimePointStd tp_2(in_2);
    timing::HRTimePointStd tp_3(in_3);

    // Conversions.
    res_1 = timing::timePointToString(tp_1, "%Y-%m-%dT%H:%M:%S", TimeResolution::SECONDS);
    res_2 = timing::timePointToString(tp_1, "%Y-%m-%dT%H:%M:%S", TimeResolution::MILLISECONDS);
    res_3 = timing::timePointToString(tp_1, "%Y-%m-%dT%H:%M:%S", TimeResolution::MICROSECONDS);
    res_4 = timing::timePointToString(tp_1, "%Y-%m-%dT%H:%M:%S", TimeResolution::NANOSECONDS);
    res_5 = timing::timePointToString(tp_2, "%Y%m%d%H%M%S", TimeResolution::MILLISECONDS);
    res_6 = timing::timePointToString(tp_3, "%Y-%m-%dT%H:%M:%S", TimeResolution::SECONDS);
    res_7 = timing::timePointToString(tp_3, "%B %d, %Y - %H:%M", TimeResolution::SECONDS);
    res_8 = timing::timePointToString(tp_3, "%Y-%m-%dT%H:%M:%S", TimeResolution::NANOSECONDS);
    // Trailing
    res_9 = timing::timePointToString(tp_3, "%B %d, %Y - %H:%M", TimeResolution::MILLISECONDS, true, false);
    res_10 = timing::timePointToString(tp_2, "%Y%m%d%H%M%S", TimeResolution::MICROSECONDS, true, false);
    res_11 = timing::timePointToString(tp_3, "%Y-%m-%dT%H:%M:%S", TimeResolution::NANOSECONDS, true, false);

    // Checks.
    M_EXPECTED_EQ(out_1, res_1)
    M_EXPECTED_EQ(out_2, res_2)
    M_EXPECTED_EQ(out_3, res_3)
    M_EXPECTED_EQ(out_4, res_4)
    M_EXPECTED_EQ(out_5, res_5)
    M_EXPECTED_EQ(out_6, res_6)
    M_EXPECTED_EQ(out_7, res_7)
    M_EXPECTED_EQ(out_8, res_8)
    M_EXPECTED_EQ(out_9, res_9)
    M_EXPECTED_EQ(out_10, res_10)
    M_EXPECTED_EQ(out_11, res_11)
}

M_DEFINE_UNIT_TEST(timePointToIso8601)
{
    // Inputs.
    std::chrono::nanoseconds in_1(167253349123456789);
    std::chrono::seconds in_2(1707141363);

    // Expected outputs.
    std::string out_1 = "1975-04-20T19:15:49.123Z";
    std::string out_2 = "1975-04-20T19:15:49.123456789Z";
    std::string out_3 = "2024-02-05T13:56:03Z";

    // Results.
    std::string res_1, res_2, res_3, res_4;

    // Time points.
    timing::HRTimePointStd tp_1(in_1);
    timing::HRTimePointStd tp_2(in_2);

    // Conversion to ISO 8601 format.
    res_1 = timing::timePointToIso8601(tp_1, TimeResolution::MILLISECONDS, true);
    res_2 = timing::timePointToIso8601(tp_1, TimeResolution::NANOSECONDS, true);
    res_3 = timing::timePointToIso8601(tp_2, TimeResolution::MILLISECONDS, true);
    res_4 = timing::timePointToIso8601(tp_2, TimeResolution::SECONDS, false);

    // Custom check.
    // Custom check function to verify a string does not end with 'Z'
    std::function<bool(const std::string&)> checkDoesNotEndWithZ =
        [](const std::string& str1)
        {
            return !str1.empty() && str1.back() != 'Z';
        };

    // Checks.
    M_EXPECTED_EQ(res_1, out_1)
    M_EXPECTED_EQ(res_2, out_2)
    M_EXPECTED_EQ(res_3, out_3)
    M_CUSTOM_CHECK(checkDoesNotEndWithZ, res_4)
}

M_DEFINE_UNIT_TEST(currentISO8601Date)
{
    // Call currentISO8601Date with different resolutions and UTC settings
    std::string current_ms_utc = timing::currentISO8601Date(TimeResolution::MILLISECONDS, true);
    std::string current_ns_utc = timing::currentISO8601Date(TimeResolution::NANOSECONDS, true);
    std::string current_s_local = timing::currentISO8601Date(TimeResolution::SECONDS, false);

    // Obtain the current time.
    auto now = std::chrono::high_resolution_clock::now();
    std::string formatted_now = timing::timePointToIso8601(now, TimeResolution::MILLISECONDS, true);
    std::string current_now = timing::currentISO8601Date(TimeResolution::MILLISECONDS, true);

    // Custom check for ISO 8601 format correctness
    std::function<bool(const std::string&)> checkISO8601FormatUTC =
        [](const std::string& dateTime) -> bool
    {
        // Example check for UTC with milliseconds resolution: "YYYY-MM-DDTHH:MM:SS.sssZ"
        return dateTime.length() >= 20 &&  dateTime.find('Z') != std::string::npos;
    };

    std::function<bool(const std::string&)> checkISO8601FormatLocal =
        [](const std::string& dateTime) -> bool
    {
        // Example check for local without 'Z': "YYYY-MM-DDTHH:MM:SS" or with added resolution
        return dateTime.length() >= 19 && dateTime.find('Z') == std::string::npos;
    };

    // Validate formats
    M_CUSTOM_CHECK(checkISO8601FormatUTC, current_ms_utc)
    M_CUSTOM_CHECK(checkISO8601FormatLocal, current_s_local)
    M_EXPECTED_EQ(formatted_now.substr(0, 21), current_now.substr(0, 21)) // Approximation.
}

M_DEFINE_UNIT_TEST(millisecondsToISO8601Duration)
{
    // Test cases with expected inputs and outputs
    std::vector<std::pair<long long, std::string>> test_cases =
    {
        {0, "PT0H0M0S"},
        {1000, "PT0H0M1S"},
        {61000, "PT0H1M1S"},
        {3661000, "PT1H1M1S"},
        {45000, "PT0H0M45S"},
        {123456789, "PT34H17M36.789S"},
        {86400000, "PT24H0M0S"}, // 24 hours
        {90061000, "PT25H1M1S"},
        {590325164, "PT163H58M45.164S"},
        {10500, "PT0H0M10.5S"},
        {100, "PT0H0M0.1S"}
    };

    // Custom check for format correctness: PTxxHxxMxx.SSSS
    std::function<bool(const std::string&)> checkISO8601DurationFormat =
        [](const std::string& duration) -> bool
    {
        std::regex durationRegex(R"(^PT(\d+H)?(\d+M)?(\d+(\.\d+)?S)$)");
        return std::regex_match(duration, durationRegex);
    };

    // Do the checks.
    for (const auto& [input, output] : test_cases)
    {
        std::chrono::milliseconds msecs(input);
        std::string result = timing::millisecondsToISO8601Duration(msecs);
        M_EXPECTED_EQ(result, output)
        M_CUSTOM_CHECK(checkISO8601DurationFormat, result)
    }
}

M_DEFINE_UNIT_TEST(secondsToISO8601Duration)
{
    // Test cases with expected inputs and outputs
    std::vector<std::pair<long long, std::string>> test_cases =
        {
            {0, "PT0H0M0S"},
            {1, "PT0H0M1S"},
            {61, "PT0H1M1S"},
            {3661, "PT1H1M1S"},
            {45, "PT0H0M45S"},
            {123456, "PT34H17M36S"},
            {86400, "PT24H0M0S"},
            {90061, "PT25H1M1S"},
            {590325, "PT163H58M45S"}
        };

    // Custom check for format correctness: PTxxHxxMxxS
    std::function<bool(const std::string&)> checkISO8601DurationFormat =
        [](const std::string& duration) -> bool
    {
        std::regex durationRegex(R"(^PT(\d+H)?(\d+M)?(\d+S)$)");
        return std::regex_match(duration, durationRegex);
    };

    // Do the checks.
    for (const auto& [input, expectedOutput] : test_cases)
    {
        std::chrono::seconds secs(input);
        std::string result = timing::secondsToISO8601Duration(secs);
        M_EXPECTED_EQ(result, expectedOutput)
        M_CUSTOM_CHECK(checkISO8601DurationFormat, result)
    }
}

M_DEFINE_UNIT_TEST(iso8601DatetimeParserUTC)
{
    // Exception result.
    std::string exception_str = "[LibDegorasSLR,Timing,iso8601DatetimeParserUTC] Invalid argument:";

    // Base time.
    std::chrono::seconds secs_1(167253349);
    std::chrono::seconds secs_2(1707164728);

    // Test valid ISO 8601 datetime strings using extended format.
    std::vector<std::pair<std::string, std::chrono::nanoseconds>> valid_cases_extended =
    {
        {"1975-04-20T19:15:49Z", std::chrono::nanoseconds(secs_1)},
        {"1975-04-20T19:15:49.123Z", std::chrono::nanoseconds(secs_1 + std::chrono::milliseconds(123))},
        {"1975-04-20T19:15:49.123456Z", std::chrono::nanoseconds(secs_1 + std::chrono::microseconds(123456))},
        {"1975-04-20T19:15:49.123456789Z",  std::chrono::nanoseconds(secs_1 + std::chrono::nanoseconds(123456789))}
    };

    // Test valid ISO 8601 datetime strings using basic format.
    std::vector<std::pair<std::string, std::chrono::nanoseconds>> valid_cases_basic =
        {
            {"20240205T202528Z", std::chrono::nanoseconds(secs_2)},
            {"20240205T202528.123Z", std::chrono::nanoseconds(secs_2 + std::chrono::milliseconds(123))},
            {"20240205T202528.123456Z", std::chrono::nanoseconds(secs_2 + std::chrono::microseconds(123456))},
            {"20240205T202528.123456789Z",  std::chrono::nanoseconds(secs_2 + std::chrono::nanoseconds(123456789))}
        };

    // Test invalid ISO 8601 datetime strings
    std::vector<std::string> invalid_cases =
        {
            "1975-04-20T19:15:49.1-02:00", // Local time
            "1975-04-20 19:15:49Z",        // Incorrect separator between date and time
            "19750420T19:15:49Z",          // Using both formats
            "1975-04-20T19:15Z",           // Missing seconds
            "1975-20 19:15:49Z",           // Missing month
            "1975-20 19:15:49",            // Missing Z
            "20240205T202528-123456789Z",  // Bad format
            "",                            // Empty
            "This is not a date"           // Completely invalid format
        };

    // Do the checks.
    for (const auto& [input, expected] : valid_cases_extended)
    {
        HRTimePointStd parsed = timing::iso8601DatetimeParserUTC(input);
        M_EXPECTED_EQ(parsed.time_since_epoch(), expected)
    }

    // Do the checks.
    for (const auto& [input, expected] : valid_cases_basic)
    {
        HRTimePointStd parsed = timing::iso8601DatetimeParserUTC(input);
        M_EXPECTED_EQ(parsed.time_since_epoch(), expected)
    }

    // Do the checks.
    for (const auto& input : invalid_cases)
    {
        try
        {
            timing::iso8601DatetimeParserUTC(input);
            M_FORCE_FAIL()
        }
        catch (const std::invalid_argument& e)
        {
            std::string exception_msg = e.what();
            M_EXPECTED_EQ(exception_msg.substr(0, 65), exception_str)
        }
    }
}

// -- Time conversion functions.

M_DEFINE_UNIT_TEST(win32TicksToTimePoint)
{
    // Exception result.
    std::string exception_str =
        "[LibDegorasSLR,Timing,win32TicksToTimePoint] The ticks represent a time before the Unix epoch.";

    // Valid cases.
    std::vector<std::pair<timing::Windows32Ticks, std::string>> valid_cases =
        {
            // Windows ticks - Result.
            {116444736000000000ULL, "1970-01-01T00:00:00Z"},   // Unix epoch.
            {125911584000000000ULL, "2000-01-01T00:00:00Z"},   // 2000 era.
            {129067776000000000ULL, "2010-01-01T00:00:00Z"},   // 2000 era.
            {132223104000000000ULL, "2020-01-01T00:00:00Z"},   // 2000 era.
            {117093590311632896ULL, "1972-01-21T23:43:51.1632896Z"},    // FILETIME era 1.
            {121597189939003392ULL, "1986-04-30T11:43:13.9003392Z"},    // FILETIME era 2.
            {126100789566373888ULL, "2000-08-06T23:42:36.6373888Z"},    // FILETIME era 3.
            {130604389193744384ULL, "2014-11-14T11:41:59.3744384Z"},    // FILETIME era 4.
            {135107988821114880ULL, "2029-02-20T23:41:22.111488Z"},     // FILETIME era 5.
            {139611588448485376ULL, "2043-05-31T11:40:44.8485376Z"},    // FILETIME era 6.
        };

    // Invalid cases.
    std::vector<timing::Windows32Ticks> invalid_cases = {0ULL, 123456789123ULL, 116444735999999999ULL};

    // Do the exception checks.
    for (const auto& input : invalid_cases)
    {
        try
        {
            timing::win32TicksToTimePoint(input);
            M_FORCE_FAIL()
        }
        catch (const std::invalid_argument& e)
        {
            std::string exception_msg = e.what();
            M_EXPECTED_EQ(exception_msg, exception_str)
        }
    }

    // Do the valid checks.
    for (const auto& [input, expected] : valid_cases)
    {
        auto result_tp = win32TicksToTimePoint(input);
        std::string result_str = timing::timePointToIso8601(result_tp, TimeResolution::NANOSECONDS);
        M_EXPECTED_EQ(expected, result_str)
    }
}

// --

M_DEFINE_UNIT_TEST(timePointToJulianDatetime)
{
    auto epoch_start = std::chrono::system_clock::from_time_t(0); // 1970-01-01 00:00:00 UTC
    auto one_day_ns = std::chrono::hours(24);
    auto example_1 = std::chrono::nanoseconds(1677589965123456789LL);
    auto example_2 = std::chrono::nanoseconds(4105803825987654321LL);
    auto example_3 = std::chrono::nanoseconds(1707386592000123000LL);

    // Setup test cases
    std::vector<std::pair<timing::HRTimePointStd, JDateTime>> test_cases =
        {
            // Pair of time since epoch and expected Julian Datetime
            {epoch_start, 2440587.5L}, // Unix epoch start
            {epoch_start + one_day_ns, 2440588.5L}, // One day after Unix epoch
            {epoch_start + example_1, 2460004.05052226223L}, // Example nanoseconds after Unix epoch
            {epoch_start + example_2, 2488108.37761559785L}, // Far away
            {epoch_start + example_3, 2460348.91888889031L}
        };

    // Do the checks.
    for (const auto& [input, output] : test_cases)
    {
        timing::JDateTime jdt = timePointToJulianDatetime(input);
        timing::HRTimePointStd jdt_tp = julianDatetimeToTimePoint(jdt);
        std::string str_result = timing::timePointToIso8601(jdt_tp, TimeResolution::MILLISECONDS);
        std::string str_expected = timing::timePointToIso8601(input, TimeResolution::MILLISECONDS);

        // Microseconds preccision (in day fraction sense).
        M_EXPECTED_EQ(str_result, str_expected)
    }
}

M_DEFINE_UNIT_TEST(julianDatetimeToTimePoint)
{
    // Exception result.
    std::string exception_str =
        "[LibDegorasSLR,Timing,julianDatetimeToTimePoint] The jdt represent a time before the Unix epoch.";

    // Valid cases.
    std::vector<std::pair<timing::JDateTime, timing::HRTimePointStd>> valid_cases =
        {
            {2440587.5L, HRClock::from_time_t(0)},
            {2440588.5L, HRClock::from_time_t(86400LL)},
            {2460349.0092144L, HRClock::from_time_t(1707394396LL) + std::chrono::milliseconds(124)},
            {2496964.259213947L, HRClock::from_time_t(4870951996LL) + std::chrono::milliseconds(85)}
        };

    // Invalid cases.
    std::vector<timing::JDateTime> invalid_cases = {2440586.5L, 2040588.5L, 1840890.12345L};

    // Do the exception checks.
    for (const auto& input : invalid_cases)
    {
        try
        {
            timing::julianDatetimeToTimePoint(input);
            M_FORCE_FAIL()
        }
        catch (const std::invalid_argument& e)
        {
            std::string exception_msg = e.what();
            M_EXPECTED_EQ(exception_msg, exception_str)
        }
    }

    // Do the valid checks.
    for (const auto& [input, expected] : valid_cases)
    {
        // Up to ms resolution.
        timing::HRTimePointStd result_tp = julianDatetimeToTimePoint(input);
        std::string str_result = timing::timePointToIso8601(result_tp, TimeResolution::MILLISECONDS);
        std::string str_expected = timing::timePointToIso8601(expected, TimeResolution::MILLISECONDS);
        M_EXPECTED_EQ(str_expected, str_result)
    }
}

// --

M_DEFINE_UNIT_TEST(timePointToJulianDate_fract)
{
    auto epoch_start = std::chrono::system_clock::from_time_t(0);         // 1970-01-01T00:00:00Z
    auto one_day_ns = std::chrono::hours(24);                             // 1970-01-02T00:00:00Z
    auto first_half = std::chrono::hours(48) + std::chrono::hours(8);     // 1970-01-03T08:00:00Z
    auto second_half = std::chrono::hours(48) + std::chrono::hours(18);   // 1970-01-03T18:00:00Z
    auto example_1 = std::chrono::nanoseconds(1677589965123456789);       // 2023-02-28T13:12:45.123456789Z
    auto example_2 = std::chrono::nanoseconds(4105803825987654321);       // 2100-02-08T21:03:45.987654321Z
    auto example_3 = std::chrono::nanoseconds(1707386592000123000);       // 2024-02-08T10:03:12.000123Z

    // Setup test cases
    std::vector<std::tuple<timing::HRTimePointStd, JDate, DayFraction, std::string>> test_cases =
        {
            // Pair of time since epoch and expected Julian Datetime
            {epoch_start, 2440587, 0.5L, "1970-01-01T00:00:00Z"},
            {epoch_start + one_day_ns, 2440588, 0.5L, "1970-01-02T00:00:00Z"},
            {epoch_start + first_half, 2440589, 0.83333333333333337L, "1970-01-03T08:00:00Z"},
            {epoch_start + second_half, 2440590, 0.25L, "1970-01-03T18:00:00Z"},
            {epoch_start + example_1, 2460004, 0.05052226223135416L, "2023-02-28T13:12:45.123456789Z"},
            {epoch_start + example_2, 2488108, 0.37761559785093751L, "2100-02-08T21:03:45.987654321Z"},
            {epoch_start + example_3, 2460348, 0.91888889031249998L, "2024-02-08T10:03:12.000123Z"}
        };

    // Do the checks.
    for (const auto& [input, output_jdate, output_frac, string] : test_cases)
    {
        // Containers.
        DayFraction fraction;
        JDate jd;
        // Conversion and reverse conversion.
        timePointToJulianDate(input, jd, fraction);


        std::string out_str = timePointToIso8601(input, TimeResolution::NANOSECONDS);


        // Nanoseconds preccision (in day fraction sense, approx 17 decimals).
        M_EXPECTED_EQ(jd, output_jdate)
        M_EXPECTED_EQ(out_str, string)
        M_EXPECTED_EQ_F(fraction, output_frac, 0.00000000000000001L)
    }
}

M_DEFINE_UNIT_TEST(timePointToJulianDate)
{
    auto epoch_start = std::chrono::system_clock::from_time_t(0);         // 1970-01-01T00:00:00Z
    auto one_day_ns = std::chrono::hours(24);                             // 1970-01-02T00:00:00Z
    auto first_half = std::chrono::hours(48) + std::chrono::hours(8);     // 1970-01-03T08:00:00Z
    auto second_half = std::chrono::hours(48) + std::chrono::hours(18);   // 1970-01-03T18:00:00Z
    auto example_1 = std::chrono::nanoseconds(1677589965123456789);       // 2023-02-28T13:12:45.123456789Z
    auto example_2 = std::chrono::nanoseconds(4105803825987654321);       // 2100-02-08T21:03:45.987654321Z
    auto example_3 = std::chrono::nanoseconds(1707386592000123000);       // 2024-02-08T10:03:12.000123Z

    // Setup test cases
    std::vector<std::tuple<timing::HRTimePointStd, JDate, std::string>> test_cases =
        {
            // Pair of time since epoch and expected Julian Datetime
            {epoch_start, 2440587, "1970-01-01"},
            {epoch_start + one_day_ns, 2440588, "1970-01-02"},
            {epoch_start + first_half, 2440589, "1970-01-03"},
            {epoch_start + second_half, 2440590, "1970-01-03"},
            {epoch_start + example_1, 2460004, "2023-02-28"},
            {epoch_start + example_2, 2488108, "2100-02-08"},
            {epoch_start + example_3, 2460348, "2024-02-08"}
        };

    // Do the checks.
    for (const auto& [input, output_jdate, string] : test_cases)
    {
        JDate jd = timePointToJulianDate(input);

        M_EXPECTED_EQ(jd, output_jdate)
    }
}










// ---------------------------------------------------------------------------------------------------------------------

// UNIT TESTS EXECUTION
// ---------------------------------------------------------------------------------------------------------------------

// Start the Unit Test Session.
M_START_UNIT_TEST_SESSION("LibDegorasSLR Timing Session")

// Configuration.
M_FORCE_SHOW_RESULTS(false)

// Register the tests.
M_REGISTER_UNIT_TEST(Timing-time_utils, helpers, daysFromCivil)
// --
M_REGISTER_UNIT_TEST(Timing-time_utils, strings, timePointToString)
M_REGISTER_UNIT_TEST(Timing-time_utils, strings, timePointToIso8601)
M_REGISTER_UNIT_TEST(Timing-time_utils, strings, currentISO8601Date)
M_REGISTER_UNIT_TEST(Timing-time_utils, strings, millisecondsToISO8601Duration)
M_REGISTER_UNIT_TEST(Timing-time_utils, strings, secondsToISO8601Duration)
M_REGISTER_UNIT_TEST(Timing-time_utils, strings, iso8601DatetimeParserUTC)
// --
M_REGISTER_UNIT_TEST(Timing-time_utils, conversions, win32TicksToTimePoint)
// --
M_REGISTER_UNIT_TEST(Timing-time_utils, conversions, timePointToJulianDatetime)
M_REGISTER_UNIT_TEST(Timing-time_utils, conversions, julianDatetimeToTimePoint)
// --
M_REGISTER_UNIT_TEST(Timing-time_utils, conversions, timePointToJulianDate_fract)
M_REGISTER_UNIT_TEST(Timing-time_utils, conversions, timePointToJulianDate)



// Run unit tests.
M_RUN_UNIT_TESTS()

// Finish the session.
M_FINISH_UNIT_TEST_SESSION()

// ---------------------------------------------------------------------------------------------------------------------

// =====================================================================================================================
