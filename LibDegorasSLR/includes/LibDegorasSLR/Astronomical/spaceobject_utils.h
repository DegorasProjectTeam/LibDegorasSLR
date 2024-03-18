
/** ********************************************************************************************************************
 * @file spaceobject_utils.h
 * @author Degoras Project Team.
 * @brief This file contains several helper functions for work with space objects.
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include <string>
// =====================================================================================================================

// LIBRARY INCLUDES
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
