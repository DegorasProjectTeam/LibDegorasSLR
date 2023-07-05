
// C++ INCLUDES
// =====================================================================================================================
#include <future>
#include <iomanip>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include <LibDPSLR/Testing/unit_test.h>
#include <LibDPSLR/Helpers/container_helpers.h>
#include <LibDPSLR/Helpers/string_helpers.h>
// =====================================================================================================================


// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace testing{
// =====================================================================================================================


TestLog::TestLog(const std::string& module, const std::string& test, const std::string& det_ex,
                 bool passed, const timing::HRTimePointStd& tp, long long elapsed) :
    module_(module),
    test_(test),
    det_ex_(det_ex),
    passed_(passed),
    tp_str_(timing::timePointToIso8601(tp, true, false)),
    elapsed_(elapsed)
{}

std::string TestLog::makeLog(const std::string& storage_path) const
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
    stream << std::left << std::setw(dot_w) << std::setfill('.') << "" << aux_str;

    // Add the elapsed time if any.
    if(this->det_ex_.empty())
        stream << " [ET: " << this->elapsed_ << "us]";

    // Check if we have detailed error.
    if(!this->passed_ && !this->det_ex_.empty())
        stream << " [Except: " << this->det_ex_ << "]";

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

/*
TestSummary::TestSummary(const std::vector<TestLog> &):
    total(passed + failed)
{
    _sBorder.fill('=');
    _sBorder.width(80);
    _sBorder << "\n";

    _sPassed << "Tests Passed : " << "\x1b[38;5;40m"
             << passed << "\x1b[0m";
    _sFailed << "Tests Failed : " << "\x1b[38;5;160m"
             << failed << "\x1b[0m";

    _sTotal << "Number of tests: ";

    std::cerr << "\n" << _sBorder.str() <<  _sTotal.str()
              << total << "\n" << _sPassed.str() << "\n" << _sFailed.str()
              << "\n" << _sBorder.str() << std::endl;
}
*/


void UnitTest::runTests()
{
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
            TestBase* test = range_it->second;
            std::string det_ex;
            long long elapsed = 0;
            auto now_t = std::chrono::high_resolution_clock::now();

            // Async execution.
            std::future<long long> future = std::async(std::launch::async, [test]()
            {
                // Start time.
                auto start = std::chrono::high_resolution_clock::now();
                // Run the test.
                test->runTest();
                // End time.
                auto stop = std::chrono::high_resolution_clock::now();
                // Get the elapsed time.
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
                return duration.count();
            });

            // Wait for the asynchronous task to complete.
            future.wait();

            // Get the elapsed time.
            try{elapsed = future.get();}
            catch (const std::exception& e)
            {det_ex = e.what();}

            // Instantiate the test and store.
            TestLog t_log(c_module, test->test_name_, det_ex, test->result_, now_t, elapsed);
            t_log.makeLog();
            this->summary_.addLog(t_log);
        }

        // Move the iterator to the next unique key
        it = range.second;
    }

    // Make the summary.
    this->summary_.makeSummary(true);
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

void TestSummary::makeSummary(bool show, const std::string& storage_path) const
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
                                                      "%Y%m%d_%H%M%S", false, false);
    filename = helpers::strings::replaceStr(filename, " ", "");
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
    std::string all_pass_str = this->n_fail_ == 0 ? "YES" : "NO";

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
    title << all_pass.str() << std::string(133 - fail.str().length(), ' ') << "=\n";
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


            std::cerr << range_it->second.makeLog() << std::endl;
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

}} // END NAMESPACES.
// =====================================================================================================================
