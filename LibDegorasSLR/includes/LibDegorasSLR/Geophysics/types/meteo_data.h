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
 * @file meteo_data.h
 * @author Degoras Project Team.
 * @brief Contains the definition of MeteoData structure.
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <vector>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include <LibDegorasBase/Timing/dates/datetime_types.h>
// =====================================================================================================================


// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace geo{
namespace types{
// =====================================================================================================================

struct LIBDPSLR_EXPORT MeteoData
{
    MeteoData() :
        temperature(0.0),
        pressure(0.0),
        rel_humidity(0.0)
    {}

    MeteoData(double temp, double press, double hum) :
        temperature(temp),
        pressure(press),
        rel_humidity(hum)
    {}

    // Members.
    double temperature;   ///< Temperature in degrees Celsius.
    double pressure;      ///< Pressure in millibars (mbar).
    double rel_humidity;  ///< Relative humidity in percentage (%).
};

using MeteoDataV = std::vector<MeteoData>;


struct LIBDPSLR_EXPORT MeteoRecord : MeteoData
{
    MeteoRecord() = default;

    MeteoRecord(dpbase::timing::dates::MJDateTime mjdt) : mjdt(std::move(mjdt)) {}
    MeteoRecord(dpbase::timing::dates::MJDateTime mjdt, double temp, double press, double hum) :
        MeteoData(temp, press, hum), mjdt(std::move(mjdt)) {}

    dpbase::timing::dates::MJDateTime mjdt; ///< MJ datetime associated to the meteo data.
};

using MeteoRecordV = std::vector<MeteoRecord>;

}}} // END NAMESPACES.
// =====================================================================================================================
