/***********************************************************************************************************************
 *   LibDegorasSLR (Degoras Project SLR Library).
 *
 *   A modern C++ libre base library for Satellite Laser Ranging (SLR)
 *
 *   software and real-time hardware related developments.
                       *                                      *
 *     Developed under the context of Degoras Project for the Spanish Navy Observatory SLR
(SFEL) station in San Fernando and, of course, any other station that wants to use it!

                                                                                                                *
 *   Copyright (C) 2023 Degoras Project Team                                                                           *
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >                             *
 *                      < Jesús Relinque Madroñal >                                                                    *
 *                                                                                                                     *
 *   This file is part of LibDegorasSLR.                                                                                    *
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
#include <iostream>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include <LibDegorasSLR/Testing/UnitTest>
#include <LibDegorasSLR/Timing/TimeUtils>
// =====================================================================================================================

// NAMESPACES
using namespace dpslr;
using namespace dpslr::timing;

// UNIT TEST DECLARATIONS
M_DECLARE_UNIT_TEST(Timing, timePointToString)
M_DECLARE_UNIT_TEST(Timing, timePointToIso8601)
M_DECLARE_UNIT_TEST(Timing, currentISO8601Date)
M_DECLARE_UNIT_TEST(Timing, iso8601DatetimeParserUTC)
M_DECLARE_UNIT_TEST(Timing, iso8601DurationParser)

// UNIT TESTS IMPLEMENTATIONS

M_DEFINE_UNIT_TEST(Timing, timePointToString)
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
    std::string out_5 = "20240205111104.000";
    std::string out_6 = "1970-01-01T00:00:00";
    std::string out_7 = "January 01, 1970 - 00:00";

    // Results.
    std::string res_1, res_2, res_3, res_4, res_5, res_6, res_7;

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

    // Checks.
    M_EXPECTED_EQ(out_1, res_1)
    M_EXPECTED_EQ(out_2, res_2)
    M_EXPECTED_EQ(out_3, res_3)
    M_EXPECTED_EQ(out_4, res_4)
    M_EXPECTED_EQ(out_5, res_5)
    M_EXPECTED_EQ(out_6, res_6)
    M_EXPECTED_EQ(out_7, res_7)
}

M_DEFINE_UNIT_TEST(Timing, timePointToIso8601)
{
    // Inputs.
    std::chrono::nanoseconds in_1(167253349123456789);
    std::chrono::seconds in_2(1707141363);

    // Expected outputs.
    std::string out_1 = "1975-04-20T19:15:49.123Z";
    std::string out_2 = "1975-04-20T19:15:49.123456789Z";
    std::string out_3 = "2024-02-05T13:56:03.000Z";

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

M_DEFINE_UNIT_TEST(Timing, currentISO8601Date)
{
    // Call currentISO8601Date with different resolutions and UTC settings
    std::string current_ms_utc = timing::currentISO8601Date(TimeResolution::MILLISECONDS, true);
    std::string current_ns_utc = timing::currentISO8601Date(TimeResolution::NANOSECONDS, true);
    std::string current_s_local = timing::currentISO8601Date(TimeResolution::SECONDS, false);

    // Obtain the current time.
    auto now = std::chrono::system_clock::now();
    std::string formatted_now = timing::timePointToIso8601(now, TimeResolution::MILLISECONDS, true);
    std::string current_now = timing::currentISO8601Date(TimeResolution::MILLISECONDS, true);

    // Custom check for ISO 8601 format correctness
    std::function<bool(const std::string&)> checkISO8601FormatUTC =
        [](const std::string& dateTime) -> bool
    {
        // Example check for UTC with milliseconds resolution: "YYYY-MM-DDTHH:MM:SS.sssZ"
        return dateTime.length() == 24 && dateTime[23] == 'Z';
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
    M_EXPECTED_EQ(formatted_now.substr(0, 22), current_now.substr(0, 22)) // Approximation.
}

M_DEFINE_UNIT_TEST(Timing, iso8601DatetimeParserUTC)
{
    // Exception result.
    std::string exception_str = "[LibDegorasSLR,Timing,iso8601DatetimeParser] Invalid argument:";

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
            "1975-04-20 19:15:49Z",       // Incorrect separator between date and time
            "19750420T19:15:49Z",         // Using both formats
            "1975-04-20T19:15Z",          // Missing seconds
            "1975-20 19:15:49Z",          // Missing month
            "1975-20 19:15:49",           // Missing Z
            "20240205T202528-123456789Z", // Bad format.
            "",                           // Empty.
            "This is not a date"          // Completely invalid format
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
            M_EXPECTED_EQ(exception_msg.substr(0, 62), exception_str)
        }
    }
}

int main()
{
    // Start the Unit Test Session.
    M_START_UNIT_TEST_SESSION("LibDegorasSLR Timing Session")

    // Register the tests.
    M_REGISTER_UNIT_TEST(Timing, timePointToString)
    M_REGISTER_UNIT_TEST(Timing, timePointToIso8601)
    M_REGISTER_UNIT_TEST(Timing, currentISO8601Date)
    M_REGISTER_UNIT_TEST(Timing, iso8601DatetimeParserUTC)

    // Run unit tests.
    M_RUN_UNIT_TESTS()
}
