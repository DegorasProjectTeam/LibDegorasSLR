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
 * @file geodetic_point.h
 * @author Degoras Project Team.
 * @brief
 * @copyright EUPL License
 * @version
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
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Mathematics/unit_conversions.h"

#include "LibDegorasSLR/Mathematics/units/strong_units.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace geo{
namespace types{
// =====================================================================================================================

/**
 * GeodeticCoords is defined as <lat, lon, alt> tuple
 */
template <typename T = double, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
struct LIBDPSLR_EXPORT GeodeticPoint
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

/// Alias for long double GeodeticPoint specialization.
using GeodeticPointL = GeodeticPoint<long double>;

}}} // END NAMESPACES.
// =====================================================================================================================
