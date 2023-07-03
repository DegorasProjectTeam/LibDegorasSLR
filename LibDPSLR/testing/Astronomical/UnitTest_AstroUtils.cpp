
#include <cassert>
#include <iostream>

#include <LibDPSLR/Testing/unit_test.h>

MDeclareUnitTest(Module1, Testing_Equal_Pass)
MDeclareUnitTest(Module1, Testing_Equal_Fail)
MDeclareUnitTest(Module1, Test3)
MDeclareUnitTest(Module2, Test1)
MDeclareUnitTest(Module2, Test2)

MDefineUnitTest(Module1, Testing_Equal_Pass)
{
    EXPECTED_EQ(1,1)
}

MDefineUnitTest(Module1, Testing_Equal_Fail)
{
    EXPECTED_EQ(1,5)
}

MDefineUnitTest(Module1, Test3)
{
    EXPECTED_EQ(1,1)
}

MDefineUnitTest(Module2, Test1)
{
    EXPECTED_EQ(1,1)
}

MDefineUnitTest(Module2, Test2)
{
    EXPECTED_EQ(1,1)
}


int main()
{
    MRegisterUnitTest(Module1, Testing_Equal_Pass)
    MRegisterUnitTest(Module1, Testing_Equal_Fail)
    MRegisterUnitTest(Module1, Testing_Equal_Fail)

    MRegisterUnitTest(Module2, Test2)


    std::cout << "Unit Test: Class dpslr::math::Matrix" << std::endl;

    UnitTest::instance().runTests();

    //MRunUnitTests()

    // All assertions passed
    std::cout << "All tests passed!\n";

    return 0;
}
