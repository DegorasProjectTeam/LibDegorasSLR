
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

UnitTestBase::~UnitTestBase(){}

void UnitTestBase::setForceStreamData(bool enable)
{
    this->force_stream_data_ = enable;
}

}} // END NAMESPACES.
// =====================================================================================================================
