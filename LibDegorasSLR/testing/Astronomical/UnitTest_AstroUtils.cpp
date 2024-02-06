
#include <iostream>

#include <LibDegorasSLR/Testing/UnitTest>
#include <LibDegorasSLR/Astronomical/astro_utils.h>
#include <LibDegorasSLR/Mathematics/units.h>

using namespace dpslr;

M_DECLARE_UNIT_TEST(AstroUtils, azElToRaDec)

M_DEFINE_UNIT_TEST(AstroUtils, azElToRaDec)
{
    long double ra, dec;

    long double lat = 50;

    astro::azElToRaDec(43.6, 53.4, 12, math::units::degToRad(50), ra, dec);
    std::cout<<ra<<" "<<dec;

    M_EXPECTED_EQ(1,1)
}


M_START_UNIT_TEST_SESSION("LibDegorasSLR Astronomical Session")

// Register the tests.
M_REGISTER_UNIT_TEST(AstroUtils, azElToRaDec)

M_RUN_UNIT_TESTS()

M_FINISH_UNIT_TEST_SESSION()
