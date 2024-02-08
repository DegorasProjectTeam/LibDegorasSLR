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
 * @file spaceobject_utils.h
 * @author Degoras Project Team.
 * @brief This file contains several helper functions for work with space objects.
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include <string>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
// =====================================================================================================================

// ========== DPSLR NAMESPACES =========================================================================================
namespace dpslr{
namespace spobj{
// =====================================================================================================================

// Satellite identifiers conversion helper functions.
// ---------------------------------------------------------------------------------------------------------------------


//BUG: ILRS CONVERSION BUG!!!!! CHANGE THE CONVERSIONS ACORIDNG TO THE DOCUMENTATION.

/**
 * @brief Converts a COSPAR to a SHORT COSPAR (1986-061A -> 86061A).
 * @param[in] cospar String that represent a COSPAR.
 * @return A string that represent a SHORT COSPAR.
 */
LIBDPSLR_EXPORT std::string cosparToShortcospar(const std::string& cospar);

/**
 * @brief Converts a SHORT COSPAR to an ILRS ID (86061A -> 8606101). This function is not fully compatible with ILRS.
 * @param[in] short_cospar String that represent a SHORT COSPAR.
 * @return A string that represent an ILRS ID.
 */
LIBDPSLR_EXPORT std::string shortcosparToILRSID(const std::string& short_cospar);

/**
 * @brief Converts a COSPAR to an ILRS ID (1986-061A -> 8606101). This function is not fully compatible with ILRS.
 * @param[in] cospar String that represent a COSPAR.
 * @return A string that represent an ILRS ID.
 */
LIBDPSLR_EXPORT std::string cosparToILRSID(const std::string& cospar);

/**
 * @brief Converts a SHORT COSPAR to a COSPAR (86061A -> 1986-061A).
 * @param[in] short_cospar String that represent a SHORT COSPAR.
 * @return A string that represent a COSPAR.
 */
LIBDPSLR_EXPORT std::string shortcosparToCospar(const std::string& short_cospar);

/**
 * @brief Converts an ILRS ID to a SHORT COSPAR (8606101 -> 86061A). This function is not fully compatible with ILRS.
 * @param[in] ilrsid String that represent an ILRS ID.
 * @return A string that represent a SHORT COSPAR.
 */
LIBDPSLR_EXPORT std::string ilrsidToShortcospar(const std::string& ilrsid);

/**
 * @brief Converts an ILRS ID to a COSPAR (8606101 -> 1986-061A). This function is not fully compatible with ILRS.
 * @param[in] ilrsid String that represent an ILRS ID.
 * @return A string that represent a COSPAR.
 */
LIBDPSLR_EXPORT std::string ilrsidToCospar(const std::string& ilrsid);
// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES.
// =====================================================================================================================
