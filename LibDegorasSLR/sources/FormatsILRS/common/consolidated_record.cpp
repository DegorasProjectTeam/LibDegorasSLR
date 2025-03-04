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
 * @file consolidated_record.cpp
 * @author Degoras Project Team.
 * @brief This file contains the implementation of the struct ConsolidatedRecord.
 * @copyright EUPL License
 2305.1
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
#include <sstream>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/FormatsILRS/common/consolidated_record.h"
// =====================================================================================================================

// LIBDPBASE INCLUDES
// =====================================================================================================================
#include <LibDegorasBase/Helpers/container_helpers.h>
#include <LibDegorasBase/Helpers/string_helpers.h>
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace ilrs{
namespace common{
// =====================================================================================================================

std::string ConsolidatedRecord::getIdToken() const {return (tokens.empty() ? "" : tokens[0]);}

std::string ConsolidatedRecord::generateCommentBlock() const
{
    // Base line.
    std::stringstream stream;

    // Generate all the comments.
    for(const auto& comment : this->comment_block)
        stream << CommentIdStr << " " << comment.substr(0, 80) << std::endl;

    // Return the comment block.
    return dpbase::helpers::strings::rmLastLineBreak(stream.str());
}

void ConsolidatedRecord::clearAll()
{
    consolidated_type = ConsolidatedFileType::UNKNOWN_TYPE;
    generic_record_type = -1;
    comment_block.clear();
    tokens.clear();
    line_number = {};
}

}}} // END NAMESPACES
// =====================================================================================================================
