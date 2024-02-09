
// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Testing/unit_test_base.h"
// =====================================================================================================================


// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace testing{
// =====================================================================================================================


UnitTestBase::UnitTestBase(const std::string &name):
    test_name_(name),
    result_(true),
    force_stream_data_(false),
    current_check_n_(0)
{}

void UnitTestBase::runTest(){}

bool UnitTestBase::forceFail()
{
    this->updateCheckResults(false);
    return false;
}

bool UnitTestBase::forcePass()
{
    this->updateCheckResults(true);
    return false;
}

bool UnitTestBase::expectTrue(bool result)
{
    std::cout<<"                           - Expecting true result"<<std::endl;
    this->updateCheckResults(result);
    return result;
}

bool UnitTestBase::expectFalse(bool result)
{
    std::cout<<"                           - Expecting false result"<<std::endl;
    this->updateCheckResults(!result);
    return !result;
}

bool UnitTestBase::expectEQ(const std::string &str1, const std::string &str2)
{
    bool result = (str1 == str2);
    this->updateCheckResults(result, str1, str2);
    return result;
}

bool UnitTestBase::expectEQ(const char *str1, const char *str2)
{
    bool result = (std::string(str1) == std::string(str2));
    this->updateCheckResults(result, str1, str2);
    return result;
}

UnitTestBase::~UnitTestBase(){}

void UnitTestBase::setForceStreamData(bool enable)
{
    this->force_stream_data_ = enable;
}

}} // END NAMESPACES.
// =====================================================================================================================
