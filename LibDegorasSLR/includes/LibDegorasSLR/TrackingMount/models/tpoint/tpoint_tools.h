/***********************************************************************************************************************
 *   AMELAS_SFELMountController: [...].
 *
 *   Copyright (C) 2023 ROA Team (Royal Institute and Observatory of the Spanish Navy)
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >
 *                      < Jesús Relinque Madroñal >
 *                      AVS AMELAS Team
 *                      <>
 *
 *   This file is part of AMELAS_SFELMountController.
 *
 *   Licensed under [...]
 **********************************************************************************************************************/

/** ********************************************************************************************************************
 * @file tpoint_tools.h
 * @brief This file contains the declaration of elements related to TPoint usage.
 * @author Degoras Project Team
 * @copyright EUPL License
 * @version 2310.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <cstdint>
#include <string>
#include <vector>
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Astronomical/types/alt_az_pos.h"
// =====================================================================================================================

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
namespace models{
// =====================================================================================================================

// CONSTANTS
// =====================================================================================================================
// =====================================================================================================================

// CONVENIENT ALIAS, ENUMERATIONS AND CONSTEXPR
// =====================================================================================================================
enum class TPointCoefficientId : std::int16_t
{
    INVALID_ID = -1,
    IE,
    IA,
    CA,
    AN,
    AW,
    NPAE,
    TF,
    TX,
    H
};

enum class TPointParseError : std::uint16_t
{
    NOT_ERROR,
    CANNOT_OPEN_FILE,
    BAD_PARAMETERS_LINE,
    UNSUPPORTED_ID
};

// =====================================================================================================================


struct LIBDPSLR_EXPORT TPointSingleCoefficient
{
    TPointCoefficientId id;
    double value;
    double sigma;
    std::string name;
};

using TPointParallelCoefficients = std::vector<TPointSingleCoefficient>;
using TPointCoefficients = std::vector<TPointParallelCoefficients>;

astro::types::AltAzPos LIBDPSLR_EXPORT computeCorrectedByTPointPosition(const TPointCoefficients& coefs,
                                                                        const astro::types::AltAzPos& pos);

TPointParseError LIBDPSLR_EXPORT parseTPointModelFile(const std::string &path, TPointCoefficients &coefs);


}}} // END NAMESPACES.
// =====================================================================================================================
