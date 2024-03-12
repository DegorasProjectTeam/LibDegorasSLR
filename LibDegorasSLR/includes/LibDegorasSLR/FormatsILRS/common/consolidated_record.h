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
 * @file consolidated_record.h
 * @author Degoras Project Team.
 * @brief This file contains the definition of the struct ConsolidatedRecord.
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include <string>
#include <vector>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/FormatsILRS/common/consolidated_types.h"
#include "LibDegorasSLR/Helpers/common_aliases_macros.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace ilrs{
namespace common{
// =====================================================================================================================

// --- RECORD BASE STRUCT ----------------------------------------------------------------------------------------------

/**
 * @brief Generic consolidated record in the ILRS standard consolidated formats.
 *
 * This structure represents a generic consolidated record in the ILRS standard consolidated formats like CPF and CRD
 * formats. This structure contains information about the record type, the comment block, tokens, line number, etc.
 */
struct LIBDPSLR_EXPORT ConsolidatedRecord
{

    // Common members for all the records.
    ConsolidatedFileType consolidated_type;     ///< Stores the consolidated type which belongs the record.
    unsigned generic_record_type;               ///< For CRD: CRDRecordsTypeEnum    For CPF: CPFRecordsTypeEnum
    std::vector<std::string> comment_block;     ///< Associated comment bloc (lines "00") for each record.
    std::vector<std::string> tokens;            ///< For reading files or other usages. ["H1", "CRD", 2, etc]
    Optional<unsigned> line_number;             ///< Line number in the file, for error handling when reading files.

    /**
     * @brief Get the ID token of the record.
     * @return The ID token as a string, or an empty string if the tokens vector is empty.
     */
    std::string getIdToken() const;

    /**
     * @brief Generate the comment block associated with the record.
     * @return The generated comment block as a string.
     */
    std::string generateCommentBlock() const;

    /**
     * @brief Clear all member variables of the ConsolidatedRecord structure.
     */
    void clearAll();
};

// ---------------------------------------------------------------------------------------------------------------------

}}} // END NAMESPACES
// =====================================================================================================================
