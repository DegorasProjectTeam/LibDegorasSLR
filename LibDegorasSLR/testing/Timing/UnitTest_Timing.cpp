
#include <cassert>
#include <iostream>

#include <LibDegorasSLR/Testing/UnitTest>
#include <LibDegorasSLR/Timing/time_utils.h>
#include <LibDegorasSLR/Mathematics/units.h>

using namespace dpslr;

M_DECLARE_UNIT_TEST(TimeUtils, jdtToGmst)

M_DEFINE_UNIT_TEST(TimeUtils, jdtToGmst)
{
    // Result auxiliar variables.
    const long double res1 = 149.16311L;

    // Result variable
    long double gmst_deg, gmst_rad;
    long double ra, dec;

    long double lat = 50;

    gmst_rad = timing::jdtToGmst(2460130.817593);
    gmst_deg = math::units::radToDegree(gmst_rad);

    M_EXPECTED_EQ(gmst_deg, res1)

    std::cout<<gmst_deg;
}

int main()
{
    M_START_UNIT_TEST_SESSION("LibDegorasSLR Timing Session")

    // Register the tests.
    M_REGISTER_UNIT_TEST(TimeUtils, jdtToGmst)

    M_RUN_UNIT_TESTS()

    return 0;
}
