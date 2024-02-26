
// C++ INCLUDES
//======================================================================================================================
#include <cmath>
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

AltAzPosition::AltAzPosition(const Degrees &az, const Degrees &el):
    az(az),
    el(el)
{
    // TODO NORMALIZE AZ EL
}

RA::RA(int hour, int min, double sec) :
    hour(hour),
    min(min),
    sec(sec)
{
    this->ra = hour + min / 60. + sec / 3600.;
}

RA::RA(double ra) :
    ra(ra)
{
    double integer, fract;
    fract = std::modf(ra, &integer);

    this->hour = static_cast<int>(integer);

    this->sec = std::modf(fract * 60, &integer) * 60;

    this->min = static_cast<int>(integer);
}

RA::operator double() const
{
    return this->ra;
}

bool RA::checkRA(int h, int min, double sec)
{
    return h >= 0 && h <= 23 && min >= 0 && min <= 59 && sec >= 0. && sec < 60.;
}

Dec::Dec(int deg, int min, double sec):
    deg(deg),
    min(min),
    sec(sec)
{
    this->dec = deg + min / 60. + sec / 3600.;
}

Dec::Dec(double dec) :
    dec(dec)
{
    double integer, fract;
    fract = std::modf(dec, &integer);

    this->deg = static_cast<int>(integer);

    this->sec = std::modf(fract * 60, &integer) * 60;

    this->min = static_cast<int>(integer);
}

Dec::operator double() const
{
    return this->dec;
}

bool Dec::checkDec(int deg, int min, double sec)
{
    return deg > -90 && deg < 90 && min >= 0 && min <= 59 && sec >= 0. && sec < 60.;
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
