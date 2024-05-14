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
#include <future>
#include <iostream>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Testing/unit_test/unit_test.h"
#include "LibDegorasSLR/Helpers/string_helpers.h"
#include "LibDegorasSLR/Timing/utils/time_utils.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace testing{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace timing::types;
// ---------------------------------------------------------------------------------------------------------------------

UnitTest &UnitTest::instance()
{
    static UnitTest uTest;
    return uTest;
}

void UnitTest::setSessionName(std::string &&session)
{
    this->session_ = std::move(session);
    this->summary_.setSessionName(this->session_);
}

void UnitTest::registerTest(std::string &&module, std::string &&submodule, UnitTestBase* test)
{
    std::pair<std::string, UnitTestBase*> submodule_pair = std::make_pair(std::move(submodule), test);
    std::pair<std::string, std::pair<std::string, UnitTestBase*>> multi_pair =
        std::make_pair(std::move(module), std::move(submodule_pair));
    this->test_dict_.insert(std::move(multi_pair));
}

void UnitTest::clear()
{
    this->test_dict_.clear();
    this->summary_.clear();
}

bool UnitTest::runTests()
{
    // Final result.
    bool final_res = true;

    // Separator.
    std::string sep = helpers::strings::fillStr("=", 120) + "\n";

    // Log.
    std::cout<<"\033[38;2;255;128;0m"<<sep<<"=                                              ";
    std::cout<<"EXECUTING UNIT TEST SESSION                                             =\n";
    std::cout<<"\033[38;2;255;128;0m"<<sep;
    auto now_t = HRTimePointStd::clock::now();
    std::string now_str = timing::timePointToIso8601(now_t, TimeResolution::MILLISECONDS, true, false);
    std::cout<<"\033[38;2;255;128;0m"<<"["<<now_str<<"]"<<"\033[038;2;0;210;0m";
    std::cout<<" Starting the session: "<<this->session_<<"\033[38;2;255;128;0m"<<std::endl;

    // Iterate over the multimap in order of keys
    for (auto it = this->test_dict_.begin(); it != this->test_dict_.end();)
    {
        // Auxiliar containers.
        std::string c_module = it->first;
        auto range = this->test_dict_.equal_range(c_module);

        // Process all elements with the same key
        for (auto range_it = range.first; range_it != range.second; ++range_it)
        {
            // Auxiliar containers.
            std::vector<std::pair<unsigned, bool>> results;
            std::string submodule = range_it->second.first;
            UnitTestBase* test = range_it->second.second;
            std::string det_ex;
            long long elapsed = 0;
            bool result;

            auto now_t = HRTimePointStd::clock::now();
            std::string now_str = timing::timePointToIso8601(now_t, TimeResolution::MILLISECONDS, true, false);

            // Log.
            std::cout<<"\033[38;2;255;128;0m"<<"["<<now_str<<"] "<<"\033[038;2;0;140;255m"
                      <<c_module << " | " << submodule << " | " << test->test_name_<<std::endl;

            // Async execution.
            auto future = std::async(std::launch::async, [test, &result, this]()
            {
                // Configure the base test.
                test->setForceStreamData(this->force_show_results_);
                // Start time.
                auto start = std::chrono::steady_clock::now();
                // Run the test.
                test->runTest();
                // End time.
                auto stop = std::chrono::steady_clock::now();
                // Get the elapsed time.
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
                // Store the result.
                result = test->result_;
                return static_cast<long long>(duration.count());
            });

            // Wait for the asynchronous task to complete.
            future.wait();

            // Get the elapsed time.
            try{elapsed = future.get();}
            catch (const std::exception& e)
            {
                result = false;
                det_ex = e.what();
            }

            // Update final result.
            if(final_res && !result)
                final_res = false;

            // Instantiate the test log and store.
            UnitTestLog t_log(c_module, submodule, test->test_name_, det_ex, result,
                              now_t, elapsed, test->check_results_);
            this->summary_.addLog(t_log);
        }

        // Move the iterator to the next unique key
        it = range.second;
    }

    // Log.
    now_t = HRTimePointStd::clock::now();
    now_str = timing::timePointToIso8601(now_t, TimeResolution::MILLISECONDS, true, false);
    std::cout<<"\033[38;2;255;128;0m"<<"["<<now_str<<"]";
    std::cout<<"\033[038;2;0;210;0m"<<" All registerted unit tests executed!"<<std::endl;
    std::cout<<"\033[38;2;255;128;0m"<<sep<<std::endl;

    // Make the summary.
    this->summary_.makeSummary(this->force_show_results_);

    // Return the final result.
    return final_res;
}

void UnitTest::setForceShowResults(bool enable)
{
    this->force_show_results_ = enable;
}

UnitTest::UnitTest() :
    force_show_results_(false)
{}

}} // END NAMESPACES.
// =====================================================================================================================
