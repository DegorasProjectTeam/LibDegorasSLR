
#include <cassert>
#include <iostream>

#include <LibDegorasSLR/Testing/UnitTest>

#include <string>
#include <sstream>
#include <vector>
#include <stdexcept> // For std::invalid_argument

// Function to split a version string into its numerical components
std::vector<unsigned> splitVersionStr(const std::string& version)
{
    std::vector<unsigned> parts;
    std::stringstream versionStream(version);
    std::string part;

    while (std::getline(versionStream, part, '.'))
    {
        try
        {
            unsigned partNum = std::stoul(part);
            parts.push_back(partNum);
        }
        catch (const std::invalid_argument&)
        {
            throw std::invalid_argument(
                "[LibDegorasBase, Strings, splitVersionStr] Non-numeric character found: " + version);
        }
        catch (const std::out_of_range&)
        {
            throw std::out_of_range(
                "[LibDegorasBase, Strings, splitVersionStr] Part of the version is out of range: " + version);
        }
    }

    return parts;
}

int compareVersionsStr(const std::string& version1, const std::string& version2)
{
    // Split versions.
    std::vector<unsigned> parts1 = splitVersionStr(version1);
    std::vector<unsigned> parts2 = splitVersionStr(version2);

    // Compare each part from left to right (major, minor, patch).
    for (size_t i = 0; i < std::max(parts1.size(), parts2.size()); ++i)
    {
        unsigned part1 = i < parts1.size() ? parts1[i] : 0;
        unsigned part2 = i < parts2.size() ? parts2[i] : 0;
        if (part1 < part2) return -1;
        if (part1 > part2) return 1;
    }

    return 0; // Versions are equal
}

int main()
{
    std::string version1 = "2.8.10.1.1";
    std::string version2 = "2.9.10";

    try
    {
        int result = compareVersionsStr(version1, version2);

        if (result < 0)
            std::cout << version1 << " is less than " << version2 << std::endl;
        else if (result > 0)
            std::cout << version1 << " is greater than " << version2 << std::endl;
        else
            std::cout << version1 << " is equal to " << version2 << std::endl;

    }
    catch (const std::exception& e)
    {
        std::cerr << "Error comparing version strings: " << e.what() << std::endl;
    }

    return 0;
}

/*
M_DECLARE_UNIT_TEST(Module1, Testing_Equal_Pass)
M_DECLARE_UNIT_TEST(Module1, Testing_Equal_Fail)
M_DECLARE_UNIT_TEST(Module1, Test3)
M_DECLARE_UNIT_TEST(Module2, Test1)
M_DECLARE_UNIT_TEST(Module2, Test2)
M_DECLARE_UNIT_TEST(Module3_Unit, Test_Modul_3)

M_DEFINE_UNIT_TEST(Module1, Testing_Equal_Pass)
{
    M_EXPECTED_EQ(1,1)
    M_SLEEP_US(23458)
}

M_DEFINE_UNIT_TEST(Module1, Testing_Equal_Fail)
{
    M_EXPECTED_EQ(1,5)
}

M_DEFINE_UNIT_TEST(Module1, Test3)
{
    M_EXPECTED_EQ(1,1)
}

M_DEFINE_UNIT_TEST(Module2, Test1)
{
    M_EXPECTED_EQ(1,1)
}

M_DEFINE_UNIT_TEST(Module2, Test2)
{
    M_EXPECTED_EQ(1,1)
}

M_DEFINE_UNIT_TEST(Module3_Unit, Test_Modul_3)
{
    M_EXPECTED_EQ(5.5, 5.5)
    M_SLEEP_US(125468)
}




int main()
{
    // For this test, some test must fail, becaouse it is for testing the UnitTest
    // framework. Due to this, we don't add this test to the cmake testing.
    M_START_UNIT_TEST_SESSION("LibDegorasSLR - TEST SESSION ")

    // Register the tests.
    M_REGISTER_UNIT_TEST(Module1, Testing_Equal_Pass)
    M_REGISTER_UNIT_TEST(Module1, Testing_Equal_Fail)
    M_REGISTER_UNIT_TEST(Module1, Test3)
    M_REGISTER_UNIT_TEST(Module2, Test2)
    M_REGISTER_UNIT_TEST(Module3_Unit, Test_Modul_3)

    M_RUN_UNIT_TESTS()



    return 0;
}
*/
