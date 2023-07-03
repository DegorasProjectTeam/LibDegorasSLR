#pragma once

#include <unistd.h>
#include <iostream>
#include <sstream>
#include <list>
#include <map>
#include <vector>

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDPSLR/libdpslr_global.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace testing{
// =====================================================================================================================


class LIBDPSLR_EXPORT TestLog
{

public:

    TestLog(const std::string& module, const std::string& test, bool passed);

    ~TestLog();

private:

    // Stringstreams.
    std::stringstream stream_;
    std::stringstream failed_;
    std::stringstream passed_;
};


class LIBDPSLR_EXPORT TestSummary
{
    std::stringstream _sBorder;
    std::stringstream _sTotal;
    std::stringstream _sPassed;
    std::stringstream _sFailed;

    int total;

public:

    TestSummary(const int& passed, const int& failed);
    ~TestSummary();
};

class TestBase
{

protected:

    TestBase(const std::string& name):
        test_name_(name),
        result_(true)
    {}

public:

    template<typename T>
    inline bool expectEQ(const T& arg1, const T& arg2)
    {
        bool result = (arg1 == arg2);
        return result;
    }

    template<typename T>
    inline bool expectNE(const T& arg1, const T& arg2)
    {
        bool result = (arg1 != arg2);
        return result;
    }

    virtual void runTest(){};

    std::string test_name_;
    bool result_;
};



class LIBDPSLR_EXPORT UnitTest
{
    UnitTest() {}

protected:

public:

    // Deleting the copy constructor.
    UnitTest(const UnitTest& obj) = delete;

    inline static UnitTest& instance()
    {
        static UnitTest uTest;
        return uTest;
    }

    void runTests();

    template<typename T>
    inline bool expectEQ(const T& arg1, const T& arg2)
    {
        bool result = (arg1 == arg2);
        return result;
    }

    template<typename T>
    inline bool expectNE(const T& arg1, const T& arg2)
    {
        bool result = (arg1 != arg2);
        return result;
    }

    virtual ~UnitTest() {}

    // Members.
    std::multimap<std::string, TestBase*> test_dict_;
    std::list<UnitTest*> test_list_;
    std::string module_name_;
    std::string test_name_;
    bool result_;
};


#define MDeclareUnitTest(Module, TestName)              \
using dpslr::testing::TestBase;                             \
using dpslr::testing::UnitTest;                         \
class Test_##Module##_##TestName : public TestBase      \
{                                                       \
    Test_##Module##_##TestName(): TestBase(#TestName){} \
    public:                                             \
    static Test_##Module##_##TestName* instance()                       \
    {                                                                           \
        static Test_##Module##_##TestName test;                           \
        return &test;                                                     \
    } \
    void runTest() override;                    \
};                                                  \


#define MDefineUnitTest(Module, TestName)       \
void Test_##Module##_##TestName::runTest()  \
 \

#define MRegisterUnitTest(Module, TestName)                                    \
    UnitTest::instance().test_dict_.insert( \
        std::pair<std::string, TestBase*>(#Module, Test_##Module##_##TestName::instance()));


#define EXPECTED_EQ(arg1, arg2)          \
this->result_ &= expectEQ(arg1, arg2);

#define EXPECTED_NE(arg1, arg2)          \
this->result_ &= expectNE(arg1, arg2);

#define MRunUnitTests()                                                                 \
UnitTest::instance.runTests();  \
 \


}} // END NAMESPACES.
// =====================================================================================================================
