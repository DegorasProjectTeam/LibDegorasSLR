
// C++ INCLUDES
// =====================================================================================================================
#include <future>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include <LibDPSLR/Testing/unit_test.h>
#include <iomanip>
// =====================================================================================================================


// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace testing{
// =====================================================================================================================

std::string formatResult(bool passed)
{
    std::stringstream result;
    result << std::left << std::setw(4) << std::setfill('.')
           << (passed ? "  PASS" : "  FAIL");
    return result.str();
}

TestLog::TestLog(const std::string& module, const std::string& test, bool passed)
{
    std::stringstream stream_unit;

    if(passed)
        stream_unit << "\033[038;2;0;210;0m";
    else
        stream_unit << "\033[170;2;0;038;0m";

    stream_unit << "( " << module << " ) " << test << "  ";

    stream_ << std::left << std::setw(70) << std::setfill('.') << stream_unit.str();
    stream_ << std::right << formatResult(passed) << "\n";
}


TestLog::~TestLog()
{
    std::cerr << stream_.str() << std::endl;
}


TestSummary::TestSummary(const int& passed, const int& failed):
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
}

TestSummary::~TestSummary()
{
    std::cerr << "\n" << _sBorder.str() <<  _sTotal.str()
              << total << "\n" << _sPassed.str() << "\n" << _sFailed.str()
              << "\n" << _sBorder.str() << std::endl;
}

void UnitTest::runTests()
{
    static int passed{0};
    static int failed{0};

    if(this->test_dict_.empty())
    {
        std::cout << "No tests registered!" << std::endl;
        return;
    }

    // Iterate over the multimap in order of keys
    for (auto it = this->test_dict_.begin(); it != this->test_dict_.end();)
    {
        std::string current_module = it->first;
        auto range = this->test_dict_.equal_range(current_module);

        // Process all elements with the same key
        for (auto rangeIt = range.first; rangeIt != range.second; ++rangeIt)
        {
            TestBase* test = rangeIt->second;

            // Async execution.
            std::future<void> future = std::async(std::launch::async, [test]()
            {test->runTest();});

            // Wait for the asynchronous task to complete.
            // TODO Move all the futures and do the wait externally.
            future.wait();

            try
            {
                future.get();
                test->result_ ? ++passed : ++failed;
            } catch (const std::exception& e)
            {
                ++failed;
                std::cerr << "\n" << "In function " << __func__
                          << ": \n" << "Failed with exception: " << e.what() << std::endl;
            }

            TestLog(current_module, test->test_name_, test->result_);
        }

        // Move the iterator to the next unique key
        it = range.second;

        TestSummary(passed, failed);

    }

}

}} // END NAMESPACES.
// =====================================================================================================================
