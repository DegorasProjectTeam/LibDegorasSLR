
// C++ INCLUDES
// =====================================================================================================================
#include <iomanip>
#include <iostream>
#include <sstream>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Testing/unit_test/unit_test_log.h"
#include "LibDegorasSLR/Timing/time_utils.h"
// =====================================================================================================================


// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
namespace dpslr{
namespace testing{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace timing::types;
// ---------------------------------------------------------------------------------------------------------------------

UnitTestLog::UnitTestLog(const std::string& module,
                         const std::string& submodule,
                         const std::string& test,
                         const std::string& det_ex,
                         bool passed, const HRTimePointStd& tp,
                         long long elapsed,
                         const UnitTestResult& results) :
    module_(module),
    submodule_(submodule),
    test_(test),
    tp_str_(timing::timePointToIso8601(tp, TimeResolution::MILLISECONDS, true, false)),
    det_ex_(det_ex),
    passed_(passed),
    elapsed_(elapsed),
    results_(results)
{}

std::string UnitTestLog::makeLog(bool force_show) const
{
    // Stream.
    std::stringstream stream;

    // Update colors.
    stream << (this->passed_ ? "\033[038;2;0;210;0m" : "\033[170;2;0;038;0m");

    // Log the result.
    stream << "[" << this->tp_str_ << "] ";
    stream << this->module_ << " | " << this->submodule_ << " | " << this->test_ << " ";
    std::string aux_str = formatResult();
    size_t dot_w = 70 - this->test_.size() - this->module_.size() - this->submodule_.size() - aux_str.size();
    stream << std::left << std::setw(static_cast<int>(dot_w)) << std::setfill('.') << "" << aux_str;

    // Add the elapsed time.
    stream << " [" << this->elapsed_ << "us]";

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

std::string UnitTestLog::formatResult() const
{
    std::stringstream result;
    result << std::left << std::setw(4) << std::setfill('.')
           << (this->passed_ ? " PASS" : " FAIL");
    return result.str();
}

const std::string &UnitTestLog::getModuleName() const{return this->module_;}

const std::string &UnitTestLog::getSubmoduleName() const{return this->submodule_;}

bool UnitTestLog::getResult() const{return this->passed_;}


}} // END NAMESPACES.
// =====================================================================================================================
