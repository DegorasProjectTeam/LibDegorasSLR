
#include <cassert>
#include <iostream>

#include <LibDPSLR/Testing/UnitTest>

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
    M_START_UNIT_TEST_SESSION("LibDPSLR - TEST SESSION ")

    // Register the tests.
    M_REGISTER_UNIT_TEST(Module1, Testing_Equal_Pass)
    M_REGISTER_UNIT_TEST(Module1, Testing_Equal_Fail)
    M_REGISTER_UNIT_TEST(Module1, Test3)
    M_REGISTER_UNIT_TEST(Module2, Test2)
    M_REGISTER_UNIT_TEST(Module3_Unit, Test_Modul_3)

    M_RUN_UNIT_TESTS()



    return 0;
}
