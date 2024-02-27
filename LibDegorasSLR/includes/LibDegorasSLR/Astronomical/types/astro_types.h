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
 * @file astro_types.h
 * @author Degoras Project Team.
 * @brief This file contains several astronomical definitions.
 * @copyright EUPL License
 * @version 2305.1
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
#include "LibDegorasSLR/Timing/types/time_types.h"
#include "LibDegorasSLR/Mathematics/units/strong_units.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
namespace types{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using timing::types::MJDate;
using timing::types::SoD;
using timing::types::MJDateTime;
using math::units::Degrees;
// ---------------------------------------------------------------------------------------------------------------------


struct LIBDPSLR_EXPORT RA
{
    RA() = default;
    RA(int hour, int min, double sec);
    RA(double ra);

    RA(const RA&) = default;
    RA(RA&&) = default;
    RA& operator=(const RA&) = default;
    RA& operator=(RA&&) = default;

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

struct LIBDPSLR_EXPORT Star
{
    Star() = default;
    Star(const Star&) = default;
    Star(Star&&) = default;
    Star& operator =(const Star&) = default;
    Star& operator =(Star&&) = default;

    RA ra;
    Dec dec;
    std::string star_name;
    std::string catalog_name;
    int catalog_num;
    int id;
    double pm_ra;
    double pm_dec;
    double parallax;
    double rad_vel;
};

struct LIBDPSLR_EXPORT AltAzPos
{
    AltAzPos();

    AltAzPos(const Degrees& az, const Degrees& el);


    AltAzPos(const AltAzPos& pos) = default;
    AltAzPos(AltAzPos&& pos) = default;

    AltAzPos& operator =(const AltAzPos& pos) = default;
    AltAzPos& operator=(AltAzPos&&) = default;

    ~AltAzPos() = default;

    //size_t serialize(zmqutils::utils::BinarySerializer& serializer) const final;

    //void deserialize(zmqutils::utils::BinarySerializer& serializer) final;

    //size_t serializedSize() const final



    Degrees az;     ///< Azimuth of the altazimuth coordinate in degrees.
    Degrees el;     ///< Altitude (elevation) of the altazimuth coordinate in degrees.
};

/// Alias for altaz corrections.
using AltAzCorrection = AltAzPos;

/// Alias for a vector of AltAzPosition.
using AltAzPosV = std::vector<AltAzPos>;

/// Alias for a vector of AltAzCorrection.
using AltAzCorrectionV = std::vector<AltAzCorrection>;



}}} // END NAMESPACES
// =====================================================================================================================
