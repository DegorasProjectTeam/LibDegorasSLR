/***********************************************************************************************************************
 *   LibDegorasSLR (Degoras Project SLR Library).                                                                      *
 *                                                                                                                     *
 *   A modern and efficient C++ base library for Satellite Laser Ranging (SLR) software and real-time hardware         *
 *   related developments. Developed as a free software under the context of Degoras Project for the Spanish Navy      *
 *   Observatory SLR station (SFEL) in San Fernando and, of course, for any other station that wants to use it!        *
 *                                                                                                                     *
 *   Copyright (C) 2024 Degoras Project Team                                                                           *
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >                             *
 *                      < Jesús Relinque Madroñal >                                                                    *
 *                                                                                                                     *
 *   This file is part of LibDegorasSLR.                                                                               *
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
 * @file string_helpers.tpp
 * @brief This file contains the template functions related with the string helper tools.
 * @author Degoras Project Team
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include <iostream>
#include <string>
#include <limits>
#include <sstream>
#include <iomanip>
#include <vector>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace helpers{
namespace strings{
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

template<typename T>
std::string numberToFixStr(T x, unsigned int prec)
{
    std::ostringstream strout ;
    strout << std::showpoint << std::setprecision(prec) << x ;
    std::string str = strout.str() ;
    size_t end = str.find_last_not_of( '0' ) + 1 ;
    str.erase(end);
    if(str.back() == '.')
        str.pop_back();
    return str;
}

template<typename T>
std::string numberToStr(T x, unsigned int prec, unsigned int dec_places, bool fixed)
{
    std::ostringstream strout;
    std::vector<std::string> aux;
    std::string dplac;
    if(fixed)
        strout<<std::fixed;
    strout << std::showpoint << std::setprecision(prec) << x ;
    std::string str = strout.str() ;
    split(aux, str, ".", true);
    if(aux.size()==2 && aux[1].size() > dec_places)
    {
        dplac = aux[1].substr(0, dec_places);
        return aux[0]+"."+dplac;
    }
    else
    {
        return str;
    }
}

template<typename T>
std::string numberToMaxDecStr(const T& x)
{
    // Container.
    std::ostringstream strout;
    // Set precision to maximum possible for the type
    strout << std::showpoint << std::setprecision(std::numeric_limits<T>::digits10) << x;
    // Convert to string
    std::string str = strout.str();
    // Remove trailing zeros and potential trailing decimal point
    size_t end = str.find_last_not_of('0') + 1;
    if (str[end - 1] == '.')
        --end;
    str.erase(end);
    // Return the sring.
    return str;
}

template <typename Container>
std::string join(const Container& strings, const std::string& delimiter)
{
    std::ostringstream oss;
    auto it = strings.begin();
    if (it != strings.end()) {
        oss << *it;
        ++it;
    }
    for (; it != strings.end(); ++it) {
        oss << delimiter << *it;
    }
    return oss.str();
}


}}}// END NAMESPACES.
// =====================================================================================================================
