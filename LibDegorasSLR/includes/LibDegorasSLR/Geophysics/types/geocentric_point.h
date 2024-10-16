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
#include "LibDegorasBase/Helpers/common_aliases_macros.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace geo{
namespace types{
// =====================================================================================================================

/**
 */
struct LIBDPSLR_EXPORT GeocentricPoint
{
    // Default constructor and destructor, copy and movement constructor and operators.
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE_DTOR(GeocentricPoint)

    GeocentricPoint(const dpbase::math::units::Meters& x, const dpbase::math::units::Meters& y, const dpbase::math::units::Meters& z) :
        x(x), y(y), z(z)
    {}

    GeocentricPoint(std::array<dpbase::math::units::Meters,3> a) :
        x(a[0]), y(a[1]), z(a[2])
    {}

    GeocentricPoint(dpbase::math::types::Vector3D<dpbase::math::units::Meters> v) :
        x(v.getX()), y(v.getY()), z(v.getZ())
    {}

    dpbase::math::types::Vector3D<dpbase::math::units::Meters> toVector3D() const
    {
        return dpbase::math::types::Vector3D<dpbase::math::units::Meters>(x,y,z);
    }

    std::vector<dpbase::math::units::Meters> toStdVector() const
    {
        return this->toVector3D().toVector();
    }

    std::string toJsonStr() const
    {
        std::ostringstream json;
        json << "{"
             << "\"x\": " << this->x.toString() << ", "
             << "\"y\": " << this->y.toString() << ", "
             << "\"z\": " << this->z.toString()
             << "}";
        return json.str();
    }

    template<typename Container = std::array<dpbase::math::units::Meters, 3>>
    inline constexpr Container store() const {return Container{this->x,this->y,this->z};}

    dpbase::math::units::Meters x;
    dpbase::math::units::Meters y;
    dpbase::math::units::Meters z;
};

}}} // END NAMESPACES.
// =====================================================================================================================
