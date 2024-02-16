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
 * @file string_helpers.h
 * @brief This file contains several helper tools related with strings.
 * @author Degoras Project Team
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

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace helpers{
namespace strings{
// =====================================================================================================================

LIBDPSLR_EXPORT std::string hola();

// Transform a string to upper case.
LIBDPSLR_EXPORT std::string toUpper(const std::string& str);

// Transform a string to lower case.
LIBDPSLR_EXPORT std::string toLower(const std::string& str);

// String left trim.
LIBDPSLR_EXPORT std::string ltrim(const std::string& str);

// String right trim.
LIBDPSLR_EXPORT std::string rtrim(const std::string& str);

// String trim.
LIBDPSLR_EXPORT std::string trim(const std::string& str);

/**
 * @brief Takes a string and returns a new string with the last line break removed.
 * @param str The input string.
 * @return The modified string without the last line break.
 * @note If the input string does not contain any line breaks, the original string is returned.
 */
LIBDPSLR_EXPORT std::string rmLastLineBreak(const std::string& str);

/**
 * @brief Generate Lorem Ipsum text.
 * @param paragraphs The number of paragraphs to generate.
 * @param sentences The number of sentences per paragraph.
 * @param words The number of words per sentence.
 * @return The generated Lorem Ipsum text.
 */
LIBDPSLR_EXPORT std::string generateLoremIpsum(unsigned paragraphs, unsigned sentences, unsigned words);

/**
 * @brief Replaces occurrences of a substring in a string with a specified replacement string.
 * @param str The string to modify.
 * @param target The substring to search for and replace.
 * @param replacement The string to replace occurrences of the target substring with.
 * @return A new string with the replacements.
 */
LIBDPSLR_EXPORT std::string replaceStr(const std::string& str, const std::string& target, const std::string& repl);

/**
 * @brief Creates a string with a specified width filled with a given character.
 * @param fillChar The character used to fill the string.
 * @param width The desired width of the string.
 * @return The resulting string with the specified width and filled characters.
 */
LIBDPSLR_EXPORT std::string fillStr(const std::string& fillChar, size_t width);

// Custom split.
template <class Container>
void split (Container& result, const std::string& s, const std::string& delimiters, bool empties = true);

// Custom split 2.
template <class Container>
Container split (const std::string& s, const std::string& delimiters, bool empties = true );

/**
 * @brief Concatenates elements from a container into a single string, separated by the specified delimiter.
 * @tparam Container The type of the container.
 * @param strings The container of strings to join.
 * @param delimiters The delimiter string to insert between elements (default: " ").
 * @return The concatenated string.
 */
template <typename Container>
std::string join(const Container& strings, const std::string& del = " ");

// Custom number to fixed string conversion.
template<typename T>
std::string numberToFixstr(T x, unsigned int prec);

// Custom number to fixed string conversion.
template<typename T>
std::string numberToStr(T x, unsigned int prec, unsigned int dec_places, bool fixed = true);

template<typename T>
std::string numberToMaxDecStr(const T& x);

}}} // END NAMESPACES
// =====================================================================================================================

// TEMPLATES INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Helpers/string_helpers.tpp"
// =====================================================================================================================
