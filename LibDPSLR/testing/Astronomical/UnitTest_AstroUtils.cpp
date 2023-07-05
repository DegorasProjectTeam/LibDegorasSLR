
#include <cassert>
#include <iostream>

#include <LibDPSLR/Testing/UnitTest>
#include <LibDPSLR/Astronomical/astro_utils.h>
#include <LibDPSLR/Mathematics/units.h>

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

int main()
{
    M_START_UNIT_TEST_SESSION("LibDPSLR Astronomical Session")

    // Register the tests.
    M_REGISTER_UNIT_TEST(AstroUtils, azElToRaDec)

    M_RUN_UNIT_TESTS()

    return 0;
}
