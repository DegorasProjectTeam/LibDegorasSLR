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
#include "LibDegorasSLR/Mathematics/units/unit_conversions.h"
#include "LibDegorasSLR/Mathematics/units/strong_units.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace geo{
namespace types{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using math::units::Degrees;
using math::units::Radians;
using math::units::Meters;
// ---------------------------------------------------------------------------------------------------------------------

/**
 * GeodeticCoords is defined as <lat, lon, alt> tuple
 */
template <typename AngleType,
         typename = typename std::enable_if<
         std::is_same<AngleType, Degrees>::value ||
         std::is_same<AngleType, Radians>::value>::type>
struct LIBDPSLR_EXPORT GeodeticPoint
{
    AngleType lat;
    AngleType lon;
    Meters alt;

    GeodeticPoint(const AngleType& lat = AngleType(),
                  const AngleType lon = AngleType(),
                  const Meters alt = Meters()) :
        lat(lat),
        lon(lon),
        alt(alt)
    {}

    GeodeticPoint(const GeodeticPoint&) = default;

    GeodeticPoint(GeodeticPoint&&) = default;

    GeodeticPoint& operator =(const GeodeticPoint&) = default;

    GeodeticPoint& operator =(GeodeticPoint&&) = default;


    template<typename NewAngleType>
    inline constexpr GeodeticPoint<NewAngleType> convertAngles() const
    {
        // Check if the new angle type is the same as the current angle type.
        if constexpr(std::is_same_v<AngleType, NewAngleType>)
        {
            return *this;
        }
        else if constexpr(std::is_same_v<NewAngleType, Radians>)
        {
            NewAngleType new_lat = static_cast<Radians>(math::units::degToRad(this->lat));
            NewAngleType new_lon = static_cast<Radians>(math::units::degToRad(this->lon));
            return GeodeticPoint<NewAngleType>(new_lat, new_lon, this->alt);
        }
        else if constexpr(std::is_same_v<NewAngleType, Degrees>)
        {
            NewAngleType new_lat = static_cast<Degrees>(math::units::radToDegree(this->lat));
            NewAngleType new_lon = static_cast<Degrees>(math::units::radToDegree(this->lon));
            return GeodeticPoint<NewAngleType>(new_lat, new_lon, this->alt);
        }
    }

    template<typename Container = std::array<std::common_type_t<AngleType, Meters>, 3>>
    inline constexpr Container store() const {return Container{lat, lon, alt};}
};

/// Alias for degrees GeodeticPoint specialization.
using GeodeticPointDeg = GeodeticPoint<Degrees>;

/// Alias for radians GeodeticPoint specialization.
using GeodeticPointRad = GeodeticPoint<Radians>;

}}} // END NAMESPACES.
// =====================================================================================================================
