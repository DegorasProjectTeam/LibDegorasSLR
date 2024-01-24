/***********************************************************************************************************************
 *   LibDPSLR (Degoras Project SLR Library): A libre base library for SLR related developments.                        *                                      *
 *                                                                                                                     *
 *   Copyright (C) 2023 Degoras Project Team                                                                           *
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >                             *
 *                      < Jesús Relinque Madroñal >                                                                    *
 *                                                                                                                     *
 *   This file is part of LibDPSLR.                                                                                    *
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
 * @file geo_types.h
 * @author Degoras Project Team.
 * @brief This file contains the declaration of structs related with the geo module.
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <type_traits>
#include <array>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include <LibDegorasSLR/Mathematics/units.h>
#include <LibDegorasSLR/Mathematics/containers/vector3d.h>
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace geo{
namespace common{
// =====================================================================================================================



/**
 * GeocentricCoords is defined as <x,y,z> tuple
 */
template <typename T = double, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
struct GeocentricPoint
{
    using DistType = dpslr::math::units::Distance<T>;

    DistType x;
    DistType y;
    DistType z;

    GeocentricPoint(T x = T(), T y = T(), T z = T(), typename DistType::Unit unit = DistType::Unit::METRES) :
        x(x, unit), y(y, unit), z(z, unit) {}

    GeocentricPoint(std::array<T,3> a, typename DistType::Unit unit = DistType::Unit::METRES) :
        x(a[0], unit), y(a[1], unit), z(a[2], unit) {}

    template<typename Container = std::array<long double, 3>>
    inline constexpr Container store() const {return Container{x,y,z};}

    math::Vector3D<T> toVector3D() const {return math::Vector3D<T>(x,y,z);}
};

/**
 * GeodeticCoords is defined as <lat, lon, alt> tuple
 */
template <typename T = double, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
struct GeodeticPoint
{
    using AngleType = math::units::Angle<T>;
    using DistType = math::units::Distance<T>;

    AngleType lat;
    AngleType lon;
    DistType alt;

    GeodeticPoint(T lat = T(), T lon = T(), T alt = T(),
                   typename AngleType::Unit angle_unit = AngleType::Unit::RADIANS,
                   typename DistType::Unit dist_unit = DistType::Unit::METRES) :
        lat(lat, angle_unit), lon(lon, angle_unit), alt(alt, dist_unit)
    {}

    void convert(typename AngleType::Unit angle_unit, typename DistType::Unit dist_unit)
    {
        this->lat.convert(angle_unit);
        this->lon.convert(angle_unit);
        this->alt.convert(dist_unit);
    }

    template<typename Container = std::array<long double, 3>>
    inline constexpr Container store() const {return Container{lat, lon, alt};}
};

}}} // END NAMESPACES.
// =====================================================================================================================
