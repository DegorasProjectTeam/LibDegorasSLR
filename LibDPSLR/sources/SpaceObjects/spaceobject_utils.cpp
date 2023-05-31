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

/** ********************************************************************************************************************
 * @file slrutils.cpp
 * @see slrutils.h
 * @author DEGORAS PROJECT TEAM
 * @copyright EUPL License
***********************************************************************************************************************/

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include <LibDPSLR/SpaceObject/spaceobject_utils.h>
#include <LibDPSLR/Helpers/string_helpers.h>
#include <LibDPSLR/Helpers/file_helpers.h>
#include <LibDPSLR/Mathematics/math.h>
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace spobj{
// =====================================================================================================================

static constexpr std::array<const char*, 24> CosparAlphabet{
    "A", "B", "C", "D", "E", "F", "G", "H", "J", "K", "L", "M", "N",
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};

int letterToNumber(char letter)
{
    std::string upperLetter = std::string(1, std::toupper(letter));
    auto it = std::find(std::begin(CosparAlphabet), std::end(CosparAlphabet), upperLetter);
    if (it != std::end(CosparAlphabet))
    {
        int index = std::distance(std::begin(CosparAlphabet), it);
        return index + 1; // Add 1 to get the numeric value
    }
    return -1; // Return -1 if letter not found
}

std::string cosparToShortcospar(const std::string& cospar)
{
    std::string short_cospar = cospar.substr(2);
    short_cospar.erase(short_cospar.find('-'),1);
    return helpers::strings::toUpper(short_cospar);
}

std::string shortcosparToILRSID(const std::string& short_cospar)
{
    // Find the letters.
    auto it = std::find_if(short_cospar.begin(), short_cospar.end(), [](unsigned char a){return std::isalpha(a);});

    // Copy the numbers
    std::string ilrsid(short_cospar.begin(), it);

    // Convert letters to number like this: letter_0_value + letter_1_value * 26 + letter_2_value * 26^2...
    // The letter value is its position within the ASCII alphabet. A = 1, B = 2, etc.
    int num = 0;
    int exp = 0;
    for (auto i = short_cospar.end() - 1; i != (it - 1); i--)
    {
        num += (std::toupper(*i) - 'A' + 1) * std::pow(26, exp);
        exp++;
    }

    // Get only two characters from number
    char buffer[3];
    std::snprintf(buffer, 3, "%02d", num);

    // Return the ILRS ID.
    return ilrsid + buffer;
}

std::string cosparToILRSID(const std::string& cospar)
{
    return shortcosparToILRSID(cosparToShortcospar(cospar));
}

std::string shortcosparToCospar(const std::string& short_cospar)
{
    // Variables.
    std::string cospar;
    std::string year_cent_str = short_cospar.substr(0, 2);
    int year_cent = 0;

    try { year_cent = std::stoi(year_cent_str); } catch (...)
    { return ""; }

    // Since first satellite is in 1957, from 57 to 99, represents 1957 to 1999. 00 to 56 represents 2000 to 2056
    if(year_cent >= 57)
        cospar.append("19");
    else
        cospar.append("20");

    // Append the numbers and letters
    cospar.append(year_cent_str + '-' + short_cospar.substr(2));
    return helpers::strings::toUpper(cospar);
}

std::string ilrsidToShortcospar(const std::string& ilrsid)
{
    // The ILRS ID size must be always 7.
    if (ilrsid.size() != 7)
        return {};

    // Convert last two digits to letters. This converssion is the inverse of shortcosparToILRSID
    int num = 0;
    std::string letters;

    try
    {
        num = std::stoi(ilrsid.substr(5));
    }
    catch(...)
    {return {};}

    do
    {
        auto res = dpslr::math::euclidDivLL(num - 1, 26);
        num = res.q;
        letters.push_back('A' + res.r);
    } while(num > 0);

    // Add the letters in correct order to the numbers
    std::string res = ilrsid.substr(0, 5);
    std::reverse_copy(letters.begin(), letters.end(), std::back_inserter(res));

    // Return the short cospar.
    return res;
}

std::string ilrsidToCospar(const std::string& ilrsid)
{
    return shortcosparToCospar(ilrsidToShortcospar(ilrsid));
}

}}// END NAMESPACES.
// =====================================================================================================================
