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
 * @file astro_types.h
 * @brief
 * @author Degoras Project Team
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include <vector>
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Mathematics/units/strong_units.h"
#include "LibDegorasSLR/Helpers/common_aliases_macros.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
namespace types{
// =====================================================================================================================

struct LIBDPSLR_EXPORT RA
{
    RA() :
        hour(0), min(0), sec(0), ra(0)
    {}

    RA(int hour, int min, double sec);
    RA(double ra);

    M_DEFINE_CTOR_COPY_MOVE_OP_COPY_MOVE(RA)

    operator double () const;

    static bool checkRA(int h, int min, double sec);

    int hour;
    int min;
    double sec;
    double ra;
};

struct LIBDPSLR_EXPORT Dec
{
    Dec() = default;
    Dec(int deg, int min, double sec);
    Dec(double dec);

    Dec(const Dec&) = default;
    Dec(Dec&&) = default;
    Dec& operator=(const Dec&) = default;
    Dec& operator=(Dec&&) = default;

    operator double () const;

    static bool checkDec(int deg, int min, double sec);

    int deg;
    int min;
    double sec;
    double dec;
};

struct LIBDPSLR_EXPORT AltAzPos
{
    AltAzPos();

    AltAzPos(const math::units::Degrees& az, const math::units::Degrees& el);

    AltAzPos(const AltAzPos& pos) = default;
    AltAzPos(AltAzPos&& pos) = default;

    AltAzPos& operator =(const AltAzPos& pos) = default;
    AltAzPos& operator=(AltAzPos&&) = default;

    ~AltAzPos() = default;

    //size_t serialize(zmqutils::utils::BinarySerializer& serializer) const final;

    //void deserialize(zmqutils::utils::BinarySerializer& serializer) final;

    //size_t serializedSize() const final

    math::units::Degrees az;     ///< Azimuth of the altazimuth coordinate in degrees.
    math::units::Degrees el;     ///< Altitude (elevation) of the altazimuth coordinate in degrees.
};

/// Alias for altaz corrections.
using AltAzCorrection = AltAzPos;

/// Alias for a vector of AltAzPosition.
using AltAzPosV = std::vector<AltAzPos>;

/// Alias for a vector of AltAzCorrection.
using AltAzCorrectionV = std::vector<AltAzCorrection>;

}}} // END NAMESPACES
// =====================================================================================================================
