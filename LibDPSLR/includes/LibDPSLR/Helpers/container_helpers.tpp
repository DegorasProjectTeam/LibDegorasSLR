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
 * @file container_helpers.tpp
 * @brief This file contains the template implementation of several helper tools related with containers.
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
#include <limits>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <iterator>
#include <cmath>
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================namespace dpslr{
namespace dpslr{
namespace helpers{
namespace containers{
// =====================================================================================================================

template <class Container>
void split (Container& result, const std::string& s, const std::string& delimiters, bool empties)
{
    result.clear();
    size_t current;
    size_t next = std::numeric_limits<size_t>::max();
    do
    {
        if (!empties)
        {
            // Before getting substring, discard empty groups
            next = s.find_first_not_of( delimiters, next + 1 );
            // If end has not been reached, get next substring
            if (next != std::string::npos)
            {
                current = next;
                next = s.find_first_of( delimiters, current );
                result.push_back( s.substr( current, next - current ) );
            }
        }
        else
        {
            // Get next substring group
            current = next + 1;
            next = s.find_first_of( delimiters, current );
            result.push_back( s.substr( current, next - current ) );
        }
    }
    while (next != std::string::npos);
}

template <class Container>
Container split (const std::string& s, const std::string& delimiters, bool empties)
{
    Container result;
    split(result, s, delimiters, empties);
    return result;
}

template <class Container>
void insert(Container& a, const Container& b)
{
    a.insert(a.end(), b.begin(), b.end());
}

template <typename Container, typename T>
bool contains(const Container& container, T elem)
{
    const auto it = std::find(container.begin(), container.end(), elem);
    return (it != container.end());
}

template <typename K, typename T>
bool contains(const std::map<K,T> &map, K key)
{
    const auto it = map.find(key);
    return (it != map.end());
}

template <typename Container, typename T>
bool find(const Container& container, const T  & elem, int& pos)
{
    bool result = false;
    auto it = std::find(container.begin(), container.end(), elem);
    if (it != container.end())
    {
        pos = std::distance(container.begin(), it);
        result = true;
    }
    return result;
}

template<typename T>
std::vector<T> extract(const std::vector<T>& data, const std::vector<size_t>& indexes)
{
    // Auxiliar container.
    std::vector<T> result;
    // Extract the data.
    for(const std::size_t& index : indexes)
        result.push_back(data[index]);
    // Return the result.
    return result;
}

template<typename T>
std::vector<T> dataBetween(const std::vector<T>& v, T lower, T upper)
{
    auto l = std::lower_bound(v.begin(), v.end(), lower);
    auto u = std::upper_bound(v.begin(), v.end(), upper);
    return std::vector<T>(l, u);
}

template <class T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v)
{
    out << '[';
    if (!v.empty())
        std::copy (v.begin(), v.end(), std::ostream_iterator<T>(out, ","));
    out << "\b]";
    return out;
}

}}}// END NAMESPACES.
// =====================================================================================================================
