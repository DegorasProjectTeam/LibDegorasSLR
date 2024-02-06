
// C++ INCLUDES
// =====================================================================================================================
#include <future>
#include <iomanip>
#include <iostream>
#include <sstream>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Testing/unit_test.h"
#include "LibDegorasSLR/Helpers/container_helpers.h"
#include "LibDegorasSLR/Helpers/string_helpers.h"
// =====================================================================================================================


// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace testing{
// =====================================================================================================================


TestLog::TestLog(const std::string& module, const std::string& test, const std::string& det_ex,
                 bool passed, const timing::HRTimePointStd& tp, long long elapsed,
                 const std::vector<std::tuple<unsigned int, bool, std::string> > &results) :
    module_(module),
    test_(test),
    det_ex_(det_ex),
    passed_(passed),
    tp_str_(timing::timePointToIso8601(tp, timing::TimeResolution::MILLISECONDS, true, false)),
    elapsed_(elapsed),
    results_(results)
{}

std::string TestLog::makeLog(bool force_show) const
{
    // Stream.
    std::stringstream stream;

    // Update colors.
    stream << (this->passed_ ? "\033[038;2;0;210;0m" : "\033[170;2;0;038;0m");

    // Log the result.
    stream << "[" << this->tp_str_ << "] ";
    stream << this->module_ << " - " << this->test_ << " ";
    std::string aux_str = formatResult();
    size_t dot_w = 50 - this->test_.size() - this->module_.size() - aux_str.size();
    stream << std::left << std::setw(static_cast<int>(dot_w)) << std::setfill('.') << "" << aux_str;

    // Add the elapsed time.
    stream << " [ET: " << this->elapsed_ << "us]";

    // Check if we have detailed error.
    if(!this->det_ex_.empty())
        stream << " [Except: " << this->det_ex_ << "]";

    // Put all the checks.
    if(!this->passed_ || force_show)
    {
        stream << "\n";
        stream << "   Subtest - Result - Data\n";
        for (const auto& check : this->results_)
        {
            // Extract tuple elements using structured bindings
            const auto& [num, result, message] = check;

            // Calculate padding for check numbers to align PASS/FAIL in the same column.
            // Considering up to 999 subtests, ensuring 3 spaces for subtest numbers and a padding space.
            size_t padd = 3 - std::to_string(num).length();
            stream << (result ? "\033[038;2;0;210;0m" : "\033[170;2;0;038;0m");

            stream << "      " << num << std::string(padd, ' ') << "   -   ";
            if (result)
            {
                stream << "PASS";
                if(force_show && !message.empty())
                    stream << " - " << message;
            }
            else
            {
                stream << "FAIL";
                // Output the message associated with the check, if any.
                if (!message.empty())
                    stream << " - " << message;
            }
            stream << "\n";
        }
    }

    // Restore the default color
    stream << "\x1b[0m";

    // Return the stream.
    return stream.str();
}

std::string TestLog::formatResult() const
{
    std::stringstream result;
    result << std::left << std::setw(4) << std::setfill('.')
           << (this->passed_ ? " PASS" : " FAIL");
    return result.str();
}

const std::string &TestLog::getModuleName() const{return this->module_;}

bool TestLog::getResult() const{return this->passed_;}

bool UnitTest::runTests()
{
    // Final result.
    bool final_res = true;

    // Separator.
    std::string sep = helpers::strings::fillStr("=", 100) + "\n";

    // Log.
    std::cout<<"\033[38;2;255;128;0m"<<sep<<"=                                    ";
    std::cout<<"EXECUTING UNIT TEST SESSION                                   =\n";
    std::cout<<"\033[38;2;255;128;0m"<<sep;
    auto now_t = timing::HRTimePointStd::clock::now();
    std::string now_str = timing::timePointToIso8601(now_t, timing::TimeResolution::MILLISECONDS, true, false);
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
            TestBase* test = range_it->second;
            std::string det_ex;
            long long elapsed = 0;
            bool result;

            auto now_t = timing::HRTimePointStd::clock::now();
            std::string now_str = timing::timePointToIso8601(now_t, timing::TimeResolution::MILLISECONDS, true, false);

            // Log.
            std::cout<<"\033[38;2;255;128;0m"<<"["<<now_str<<"] "<<"\033[038;2;0;140;255m"
                      <<c_module << " | " << test->test_name_<<"..."<<std::endl;

            // Async execution.
            std::future<long long> future =
                std::async(std::launch::async, [test, &result, this]()
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
                               return duration.count();
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
            TestLog t_log(c_module, test->test_name_, det_ex, result, now_t, elapsed, test->check_results_);
            this->summary_.addLog(t_log);
        }

        // Move the iterator to the next unique key
        it = range.second;
    }

    // Log.
    now_t = timing::HRTimePointStd::clock::now();
    now_str = timing::timePointToIso8601(now_t, timing::TimeResolution::MILLISECONDS, true, false);
    std::cout<<"\033[38;2;255;128;0m"<<"["<<now_str<<"]";
    std::cout<<"\033[038;2;0;210;0m"<<" All registerted unit tests executed!"<<std::endl;
    std::cout<<"\033[38;2;255;128;0m"<<sep<<std::endl;

    // Make the summary.
    this->summary_.makeSummary(this->force_show_results_);

    // Return the final result.
    return final_res;
}

TestSummary::TestSummary():n_pass_(0),n_fail_(0){}

void TestSummary::setSessionName(const std::string &name){this->session_ = name;}

void TestSummary::addLog(const TestLog &log)
{
    this->test_logs_.insert(std::pair<std::string, TestLog>(log.getModuleName(), log));
    log.getResult() ? this->n_pass_++ : this->n_fail_++;
}

void TestSummary::clear()
{
    this->test_logs_.clear();
}

void TestSummary::makeSummary(bool force_show) const
{
    // Auxiliar containers.
    std::vector<std::string> keys = helpers::containers::getMapKeys(this->test_logs_);
    std::stringstream title;
    std::stringstream border;
    std::string s_name = "= Session:  " + this->session_;
    std::string date = "= Datetime: " + timing::currentISO8601Date();
    std::string keys_str = "= Modules:  " + helpers::strings::join(keys, " - ");
    std::string filename = this->session_;
    std::string date_file = timing::timePointToString(std::chrono::high_resolution_clock::now(),
                                                      "%Y%m%d_%H%M%S", timing::TimeResolution::SECONDS);
    filename = helpers::strings::replaceStr(filename, " ", "-");
    filename = helpers::strings::replaceStr(filename, ":", "");
    filename = helpers::strings::replaceStr(filename, "_", "-");
    filename = helpers::strings::toLower(filename) + "_";
    filename += date_file + ".utsum";
    std::string file = "= File:     " + filename;
    std::string sep1 = helpers::strings::fillStr("=", 100) + "\n";
    std::string sep2 = helpers::strings::fillStr("-", 100) + "\n";

    std::stringstream test;
    std::stringstream pass;
    std::stringstream fail;
    std::stringstream all_pass;
    std::string all_pass_color = this->n_fail_ == 0 ? "\033[038;2;0;210;000m" : "\033[170;2;0;038;000m";
    std::string all_pass_str = this->n_fail_ == 0 ? "YES" : "NO ";

    test << "= Tests:    " << "\033[038;2;0;140;255m" << std::to_string(this->test_logs_.size()) << "\033[38;2;255;128;0m";
    pass << "= Passed:   " << "\033[038;2;0;210;000m" << std::to_string(this->n_pass_) << "\033[38;2;255;128;0m";
    fail << "= Failed:   " << "\033[170;2;0;038;000m" << std::to_string(this->n_fail_) << "\033[38;2;255;128;0m";
    all_pass << "= All Pass: " << all_pass_color << all_pass_str << "\033[38;2;255;128;0m";

    std::string pass_str = pass.str();

    // Get the keys.
    keys = helpers::containers::getMapKeys(this->test_logs_);

    // Generate the header.
    title << "\033[38;2;255;128;0m";
    title << sep1;
    title << "=                                COMPLETE UNIT TEST SESSION SUMMARY                                =\n";
    title << sep1;
    title << s_name << std::string(99 - s_name.length(), ' ') << "=\n";
    title << date << std::string(99 - date.length(), ' ') << "=\n";
    title << keys_str << std::string(99 - keys_str.length(), ' ') << "=\n";
    title << file << std::string(99 - file.length(), ' ') << "=\n";
    title << test.str() << std::string(134 - test.str().length(), ' ') << "=\n";
    title << pass.str() << std::string(134 - pass.str().length(), ' ') << "=\n";
    title << fail.str() << std::string(134 - fail.str().length(), ' ') << "=\n";
    title << all_pass.str() << std::string(132 - fail.str().length(), ' ') << "=\n";
    title << sep1;
    title << "\n";

    border.fill('=');
    border.width(80);
    border << "\n";

    // Show the title.
    std::cerr<<title.str();

    // Iterate over the multimap in order of keys
    for (auto it = this->test_logs_.begin(); it != this->test_logs_.end();)
    {
        // Auxiliar containers.
        std::string c_module = it->first;
        auto range = this->test_logs_.equal_range(c_module);
        std::stringstream sep_mod;

        // Prepare the separator string.
        sep_mod << "\033[38;2;255;128;0m";
        sep_mod << sep2;
        sep_mod << "  TEST FOR MODULE: " << c_module << "\n";
        sep_mod << sep2;

        std::cerr << sep_mod.str();

        // Process all elements with the same key.
        for (auto range_it = range.first; range_it != range.second; ++range_it)
        {
            std::cerr << range_it->second.makeLog(force_show) << std::endl;
        }

        // Finish the section.
        std::cerr << "\033[38;2;255;128;0m";
        std::cerr << sep2 << "\n";

        // Move the iterator to the next unique key
        it = range.second;
    }

    // Finish and restore the default color.
    std::cerr << sep1;
    std::cerr << "\x1b[0m";
}

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

void UnitTest::addTest(std::pair<std::string, TestBase *> p)
{
    this->test_dict_.insert(p);
}

void UnitTest::clear()
{
    this->test_dict_.clear();
    this->summary_.clear();
}

void UnitTest::setForceShowResults(bool enable)
{
    this->force_show_results_ = enable;
}

UnitTest::UnitTest() :
    force_show_results_(false)
{}

TestBase::TestBase(const std::string &name):
    test_name_(name),
    result_(true),
    force_stream_data_(false),
    current_check_n_(0)
{}

void TestBase::runTest(){}

bool TestBase::forceFail()
{
    this->updateCheckResults(false);
    return false;
}

bool TestBase::forcePass()
{
    this->updateCheckResults(true);
    return false;
}

TestBase::~TestBase(){}

void TestBase::setForceStreamData(bool enable)
{
    this->force_stream_data_ = enable;
}

}} // END NAMESPACES.
// =====================================================================================================================
