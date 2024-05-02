/***********************************************************************************************************************
 *   LibDegorasSLR (Degoras Project SLR Library).                                                                      *
 *                                                                                                                     *
 *   A modern and efficient C++ base library for Satellite Laser Ranging (SLR) software and real-time hardware         *
 *   related developments. Developed as a free software under the context of Degoras Project for the Spanish Navy      *
 *   Observatory SLR station (SFEL) in San Fernando and, of course, for any other station that wants to use it!        *
 *                                                                                                                     *
 *   Copyright (C) 2024 Degoras Project Team                                                                           *
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >                             *
 *                      < Jesús Relinque Madroñal >                                                                    *
 *                                                                                                                     *
 *   This file is part of LibDegorasSLR.                                                                               *
 *                                                                                                                     *
 *   Licensed under the European Union Public License (EUPL), Version 1.2 or subsequent versions of the EUPL license   *
 *   as soon they will be approved by the European Commission (IDABC).                                                 *
 *                                                                                                                     *
 *   This project is free software: you can redistribute it and/or modify it under the terms of the EUPL license as    *
 *   published by the IDABC, either Version 1.2 or, at your option, any later version.                                 *
 *                                                                                                                     *
 *   This project is distributed in the hope that it will be useful. Unless required by applicable law or agreed to in *
 *   writing, it is distributed on an "AS IS" basis, WITHOUT ANY WARRANTY OR CONDITIONS OF ANY KIND; without even the  *
 *   implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the EUPL license to check specific   *
 *   language governing permissions and limitations and more details.                                                  *
 *                                                                                                                     *
 *   You should use this project in compliance with the EUPL license. You should have received a copy of the license   *
 *   along with this project. If not, see the license at < https://eupl.eu/ >.                                         *
 **********************************************************************************************************************/

/** ********************************************************************************************************************
 * @file astro_types.cpp
 * @brief
 * @author Degoras Project Team
 * @copyright EUPL License
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
#include <cmath>
// =====================================================================================================================

// PROJECT INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Astronomical/types/alt_az_pos.h"
#include "LibDegorasSLR/Mathematics/utils/math_utils.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
namespace types{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace math::units;
using namespace math::units::literals;
// ---------------------------------------------------------------------------------------------------------------------

AltAzPos::AltAzPos():
    az(0._deg),
    el(0._deg)
{}

AltAzPos::AltAzPos(const Degrees &az, const Degrees &el):
    az(az),
    el(el)
{
    // Normalize the azimuth and elevation.
    this->normalize();
}

void AltAzPos::normalize()
{
    // Normalize azimuth
    Degrees az = std::fmod(this->az, 360._deg);
    if (math::isFloatingMinorThanZero(az))
        az += 360._deg;

    // Reduce elevation into the -180 to 180 degree range
    Degrees el = std::fmod(this->el, 360._deg);
    if (el > 180._deg)
        el -= 360._deg;
    else if (el < -180._deg)
        el += 360._deg;

    // Normalize elevation within the -90 to 90 degree range
    if (el > 90._deg)
        el = 180._deg - el;
    else if (el < -90._deg)
        el = -180._deg - el;

    // Store new values.
    this->az = az;
    this->el = el;
}


std::string AltAzPos::toJsonStr() const
{
    std::ostringstream json;
    json << "{"
         << "\"az\": " << this->az.toString() << ", "
         << "\"el\": " << this->el.toString()
         << "}";
    return json.str();
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
