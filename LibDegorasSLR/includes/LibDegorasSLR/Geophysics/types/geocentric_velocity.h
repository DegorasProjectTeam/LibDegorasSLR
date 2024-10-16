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
 * @file geocentric_point.h
 * @author Degoras Project Team.
 * @brief
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <array>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
// =====================================================================================================================

// LIBDPBASE INCLUDES
// =====================================================================================================================
#include "LibDegorasBase/Mathematics/units/strong_units.h"
#include "LibDegorasBase/Mathematics/types/vector3d.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace geo{
namespace types{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using dpbase::math::types::Vector3D;
using dpbase::math::units::MetersSecond;
// ---------------------------------------------------------------------------------------------------------------------

/**
 */
struct LIBDPSLR_EXPORT GeocentricVelocity
{
    GeocentricVelocity(const MetersSecond& x = MetersSecond(), const MetersSecond& y = MetersSecond(),
                       const MetersSecond& z = MetersSecond()) :
        x(x), y(y), z(z)
    {}

    GeocentricVelocity(std::array<MetersSecond,3> a) :
        x(a[0]), y(a[1]), z(a[2])
    {}

    GeocentricVelocity(Vector3D<MetersSecond> v) :
        x(v.getX()), y(v.getY()), z(v.getZ())
    {}

    GeocentricVelocity(const GeocentricVelocity&) = default;
    GeocentricVelocity(GeocentricVelocity&&) = default;

    GeocentricVelocity& operator=(const GeocentricVelocity&) = default;
    GeocentricVelocity& operator=(GeocentricVelocity&&) = default;


    Vector3D<MetersSecond> toVector3D() const {return Vector3D<MetersSecond>(x,y,z);}

    std::vector<MetersSecond> toStdVector() const {return this->toVector3D().toVector();}

    std::string toJsonStr() const {return this->toVector3D().toJsonStr();}

    template<typename Container = std::array<MetersSecond, 3>>
    inline constexpr Container store() const {return Container{x,y,z};}

    MetersSecond x;
    MetersSecond y;
    MetersSecond z;
};

}}} // END NAMESPACES.
// =====================================================================================================================
