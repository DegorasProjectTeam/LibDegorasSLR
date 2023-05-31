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
 * @file container_helpers.h
 * @brief This file contains several helper tools related with containers.
 * @author Degoras Project Team.
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
#include "LibDPSLR/libdpslr_global.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace helpers{
namespace containers{
// =====================================================================================================================

// Convenient class to cast strings to bool using exceptions.
// ---------------------------------------------------------------------------------------------------------------------
class LIBDPSLR_EXPORT BoolString : public std::string
{
public:
    BoolString(const std::string &s);
    operator bool() const;
};
// ---------------------------------------------------------------------------------------------------------------------

// Custom split.
template <class Container>
void split (Container& result, const std::string& s, const std::string& delimiters, bool empties = true);

// Custom split 2.
template <class Container>
Container split (const std::string& s, const std::string& delimiters, bool empties = true );

// Custom push back.
// Insert b in a.
template <class Container>
void insert(Container& a, const Container& b);

// Custom string join.
template <typename Container>
std::string join(const Container& strings, const std::string& delimiters = " ");

// Custom contains helper.
template <typename Container, typename T>
bool contains(const Container& container, T elem);

template <typename Container, typename T>
bool find(const Container& container, const T  & elem, int& pos);

// Extract from a vector the input indexes.
template<typename T>
std::vector<T> extract(const std::vector<T>& data, const std::vector<size_t>& indexes);

template<typename T>
std::vector<T> dataBetween(const std::vector<T>& v, T lower, T upper);

template <class T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v);

}}} // END NAMESPACES.
// =====================================================================================================================

// TEMPLATES INCLUDES
// =====================================================================================================================
#include "LibDPSLR/Helpers/container_helpers.tpp"
// =====================================================================================================================
