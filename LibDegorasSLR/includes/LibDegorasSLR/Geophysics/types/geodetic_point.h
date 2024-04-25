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
 * @author Degoras Project Team
 * @brief Definition of the GeodeticPoint template structure.
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <type_traits>
#include <array>
#include <sstream>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Mathematics/units/strong_units.h"
#include "LibDegorasSLR/Mathematics/units/unit_conversions.h"
#include "LibDegorasSLR/Helpers/common_aliases_macros.h"
#include "LibDegorasSLR/Helpers/string_helpers.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace geo{
namespace types{
// =====================================================================================================================

/**
 * @brief A template structure to represent a geodetic point with latitude, longitude, and altitude.
 *
 * The GeodeticPoint struct is templated to allow for latitude and longitude angles to be expressed in either degrees
 * or radians. The altitude is always in meters.
 *
 * @tparam AngleType A type that specifies the unit of measurement for the latitude and longitude
 *         (either math::units::Degrees or math::units::Radians).
 */
template <typename AngleType, typename = typename std::enable_if<
std::is_same_v<AngleType, math::units::Degrees> || std::is_same_v<AngleType, math::units::Radians>>>
struct LIBDPSLR_EXPORT GeodeticPoint
{
    // Default constructor and destructor, copy and movement constructor and operators.
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE_DTOR(GeodeticPoint)

    /**
     * @brief Constructs a GeodeticPoint with specified latitude, longitude, and altitude.
     *
     * @param lat Latitude of the geodetic point in the specified AngleType units (Degrees or Radians).
     * @param lon Longitude of the geodetic point in the specified AngleType units (Degrees or Radians).
     * @param alt Altitude of the geodetic point in meters.
     */
    GeodeticPoint(const AngleType& lat, const AngleType lon, const math::units::Meters& alt) :
        lat(lat), lon(lon), alt(alt)
    {}

    std::string toJsonStr() const
    {
        std::ostringstream json;
        json << "{"
             << "\"lat\": " << this->lat.toString() << ", "
             << "\"lon\": " << this->lon.toString() << ", "
             << "\"alt\": " << this->alt.toString()
             << "}";
        return json.str();
    }

    /**
     * @brief Converts the angles of the geodetic point to a different angle unit (Degrees <-> Radians).
     * @tparam NewAngleType The target angle type for the conversion (either Degrees or Radians).
     * @return A new GeodeticPoint instance with the converted angle units and the same altitude.
     */
    template<typename NewAngleType>
    inline constexpr GeodeticPoint<NewAngleType> convertAngles() const
    {
        // Check if the new angle type is the same as the current angle type.
        if constexpr(std::is_same_v<AngleType, NewAngleType>)
        {
            return *this;
        }
        else if constexpr(std::is_same_v<NewAngleType, math::units::Radians>)
        {
            NewAngleType new_lat = static_cast<math::units::Radians>(math::units::degToRad(this->lat));
            NewAngleType new_lon = static_cast<math::units::Radians>(math::units::degToRad(this->lon));
            return GeodeticPoint<NewAngleType>(new_lat, new_lon, this->alt);
        }
        else if constexpr(std::is_same_v<NewAngleType, math::units::Degrees>)
        {
            NewAngleType new_lat = static_cast<math::units::Degrees>(math::units::radToDegree(this->lat));
            NewAngleType new_lon = static_cast<math::units::Degrees>(math::units::radToDegree(this->lon));
            return GeodeticPoint<NewAngleType>(new_lat, new_lon, this->alt);
        }
    }

    /**
     * @brief Stores the geodetic point's latitude, longitude, and altitude in a specified container type.
     * @tparam Container The type of container to store the geodetic point's data. Defaults to std::array with the
     *         common type of AngleType and math::units::Meters, size 3.
     * @return Container A container holding the latitude, longitude, and altitude of the geodetic point.
     */
    template<typename Container = std::array<std::common_type_t<AngleType, math::units::Meters>, 3>>
    inline constexpr Container store() const {return Container{this->lat, this->lon, this->alt};}

    // Members.
    AngleType lat;               ///< Latitude of the gedoetic point (degrees or radians).
    AngleType lon;               ///< Longitude of the gedoetic point (degrees or radians).
    math::units::Meters alt;     ///< Altitude (elevation) of the geodetic point (meters).
};

/// Alias for degrees GeodeticPoint specialization.
using GeodeticPointDeg = GeodeticPoint<math::units::Degrees>;

/// Alias for radians GeodeticPoint specialization.
using GeodeticPointRad = GeodeticPoint<math::units::Radians>;

}}} // END NAMESPACES.
// =====================================================================================================================
