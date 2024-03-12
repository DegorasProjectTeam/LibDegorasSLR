
// C++ INCLUDES
// =====================================================================================================================
#include <iostream>
#include <sstream>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Testing/unit_test/unit_test_summary.h"
#include "LibDegorasSLR/Helpers/string_helpers.h"
#include "LibDegorasSLR/Timing/time_utils.h"
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
namespace dpslr{
namespace testing{
// =====================================================================================================================

UnitTestSummary::UnitTestSummary()
    :n_pass_(0),
    n_fail_(0)
{}

void UnitTestSummary::setSessionName(const std::string &name)
{
    this->session_ = name;
}

void UnitTestSummary::addLog(const UnitTestLog &log)
{
    this->test_logs_.insert(std::pair<std::string, UnitTestLog>(log.getModuleName(), log));
    log.getResult() ? this->n_pass_++ : this->n_fail_++;
}

void UnitTestSummary::clear()
{
    this->test_logs_.clear();
}

void UnitTestSummary::makeSummary(bool force_show) const
{
    // Auxiliar containers.
    std::stringstream final_stringstream;
    std::vector<std::string> keys = helpers::containers::getMapKeys(this->test_logs_);
    std::stringstream title;
    std::stringstream border;
    std::string s_name = "= Session:  " + this->session_;
    std::string date = "= Datetime: " + timing::currentISO8601Date();
    std::string keys_str = "= Modules:  " + helpers::strings::join(keys, " | ");
    std::string filename = this->session_;
    std::string date_file = timing::timePointToString(std::chrono::high_resolution_clock::now(),
                                                      "%Y%m%d_%H%M%S", timing::types::TimeResolution::SECONDS);
    filename = helpers::strings::replaceStr(filename, " ", "-");
    filename = helpers::strings::replaceStr(filename, ":", "");
    filename = helpers::strings::replaceStr(filename, "_", "-");
    filename = helpers::strings::toLower(filename) + "_";
    filename += date_file + ".utsum";
    std::string file = "= File:     " + filename;
    std::string sep1 = helpers::strings::fillStr("=", 120) + "\n";
    std::string sep2 = helpers::strings::fillStr("-", 120) + "\n";

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
    title << "=                                          COMPLETE UNIT TEST SESSION SUMMARY"
             "                                          =\n";
    title << sep1;
    title << s_name << std::string(119 - s_name.length(), ' ') << "=\n";
    title << date << std::string(119 - date.length(), ' ') << "=\n";
    title << keys_str << std::string(119 - keys_str.length(), ' ') << "=\n";
    title << file << std::string(119 - file.length(), ' ') << "=\n";
    title << test.str() << std::string(154 - test.str().length(), ' ') << "=\n";
    title << pass.str() << std::string(154 - pass.str().length(), ' ') << "=\n";
    title << fail.str() << std::string(154 - fail.str().length(), ' ') << "=\n";
    title << all_pass.str() << std::string(152 - fail.str().length(), ' ') << "=\n";
    title << sep1;
    title << "\n";

    border.fill('=');
    border.width(80);
    border << "\n";

    // Show the title.
    final_stringstream<<title.str();

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
        sep_mod << "-- TESTS RESULTS FOR MODULE: " << c_module << "\n";
        sep_mod << sep2;

        final_stringstream << sep_mod.str();

        // Process all elements with the same key.
        for (auto range_it = range.first; range_it != range.second; ++range_it)
        {
            final_stringstream << range_it->second.makeLog(force_show) << std::endl;
        }

        // Finish the section.
        final_stringstream << "\033[38;2;255;128;0m";
        final_stringstream << sep2 << "\n";

        // Move the iterator to the next unique key
        it = range.second;
    }

    // Finish and restore the default color.
    final_stringstream << sep1;
    final_stringstream << "\x1b[0m";

    std::cerr<<final_stringstream.str();
}

}} // END NAMESPACES.
// =====================================================================================================================
