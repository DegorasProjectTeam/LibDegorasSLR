/***********************************************************************************************************************
 * Copyright 2023 Degoras Project Team
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they will be approved by the
 * European Commission - subsequent versions of the EUPL (the "Licence");
 *
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the Licence is distributed on
 * an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the Licence for the
 * specific language governing permissions and limitations under the Licence.
 **********************************************************************************************************************/

#include "LibDPSLR/class_tle.h"
#include "LibDPSLR/helpers.h"

#include <vector>

bool TLE::parseLines(const std::string &tle)
{
    bool result = false;
    std::vector<std::string> lines;
    dpslr::helpers::split(lines, tle, "\n", false);

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
