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
 * @file consolidated_types.h
 * @author Degoras Project Team.
 * @brief This file contains several types that are common in the ILRS standard consolidated formats.
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include <string>
#include <array>
#include <vector>
#include <map>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDPSLR/libdpslr_global.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace ilrs{
namespace common{
// =====================================================================================================================

// FORWARD DECLARATIONS
// -----------------------------------------------------------------------------------------------------------------
struct LIBDPSLR_EXPORT ConsolidatedRecord;
// -----------------------------------------------------------------------------------------------------------------

// RECORD IDENTIFIERS
// -----------------------------------------------------------------------------------------------------------------
/// Header record identifier strings.
static constexpr std::array<const char*, 5> HeaderIdStr{"H1","H2","H3","H4","H5"};
/// Configuration record identifier strigs.
static constexpr std::array<const char*, 8> CfgIdStr{"C0","C1","C2","C3","C4","C5","C6","C7"};
/// Data record identifier strings.
static constexpr std::array<const char*, 12> DataIdStr{"10","11","12","20","21","30","40","41","42","50","60","70"};
/// End records identifier strings.
static constexpr std::array<const char*, 3> EndIdStr{"H8","H9","99"};
/// Other record identifier strings.
static constexpr const char* CommentIdStr{"00"};
// -----------------------------------------------------------------------------------------------------------------

// ENUMERATIONS
// -----------------------------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------

}}} // END NAMESPACES
// =====================================================================================================================
