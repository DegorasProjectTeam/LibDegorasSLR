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
 * @file tle.cpp
 * @brief This file contains the implementation of the class TLE.
 * @author Degoras Project Team
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

// C++ INCLUDES
//======================================================================================================================
#include <vector>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include <LibDegorasSLR/Astronomical/tle.h>
#include <LibDegorasSLR/Helpers/string_helpers.h>
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
// =====================================================================================================================

bool TLE::parseLines(const std::string &tle)
{
    bool result = false;
    std::vector<std::string> lines;
    helpers::strings::split(lines, tle, "\n", false);
    if (3 == lines.size())
    {
        if ('1' == lines[1][0] && '2' == lines[2][0])
        {
            this->title = lines[0];
            this->first_line = lines[1];
            this->second_line = lines[2];
            this->norad_ = lines[1].substr(2, 5);
            result = true;
        }
    }
    return result;
}

bool TLE::isValid() const
{
    return !this->title.empty();
}

std::string TLE::getLines() const
{
    return this->title + '\n' + this->first_line + '\n' + this->second_line;
}

const std::string &TLE::getTitle() const
{
    return this->title;
}

const std::string &TLE::getFirstLine() const
{
    return this->first_line;
}

const std::string &TLE::getSecondLine() const
{
    return this->second_line;
}

const std::string &TLE::getNorad() const
{
    return this->norad_;
}

}} // END NAMESPACES
// =====================================================================================================================
