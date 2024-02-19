
// C++ INCLUDES
//======================================================================================================================
// =====================================================================================================================

// PROJECT INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Astronomical/common/astro_types.h"
// =====================================================================================================================

// AMELAS NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
namespace types{

AltAzPosition::AltAzPosition():
    az(0.0L),
    el(0.0L)
{}

AltAzPosition::AltAzPosition(long double az, long double el):
    az(az),
    el(el)
{
    // TODO NORMALIZE AZ EL
}


/*
size_t AltAzPos::serialize(zmqutils::utils::BinarySerializer &serializer) const
{
    return serializer.write(az, el);
}

void AltAzPos::deserialize(zmqutils::utils::BinarySerializer &serializer)
{
    serializer.read(az, el);
}

size_t AltAzPos::serializedSize() const
{
    return (2*sizeof(uint64_t) + sizeof(double)*2);
}

AltAzPos::~AltAzPos(){}

*/

// =====================================================================================================================

}}} // END NAMESPACES
// =====================================================================================================================
