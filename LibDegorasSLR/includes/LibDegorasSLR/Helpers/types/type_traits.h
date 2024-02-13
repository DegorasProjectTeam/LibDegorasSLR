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
 * @file type_traits.h
 * @brief
 * @author Degoras Project Team
 * @copyright EUPL License
 * @version
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <type_traits>
#include <vector>
#include <array>
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace helpers{
namespace types{
// =====================================================================================================================

template<typename T>
struct is_numeric : std::integral_constant<bool, std::is_integral<T>::value || std::is_floating_point<T>::value> {};

// Type trait to check if a type is a NumericStrongType with a floating point underlying type
template<typename T>
struct is_strong_float : std::false_type {};

// Custom type trait to check if a type is a NumericStrongType with an integral underlying type
template<typename T>
struct is_strong_integral : std::false_type {};

template <typename T>
struct is_container : std::false_type {};

template <typename... Args>
struct is_container<std::vector<Args...>> : std::true_type {};

template <typename T, size_t N>
struct is_container<std::array<T, N>> : std::true_type {};

// Helper to convert value to string if it supports streaming to an ostringstream.
template<typename T, typename = void>
struct is_streamable : std::false_type {};

template<typename T>
struct is_streamable<T, std::void_t<decltype(std::declval<std::ostringstream&>()
                                             << std::declval<T>())>> : std::true_type {};
template<typename T>
struct underlying_type {using type = T;};

template<typename T>
using underlying_type_t = typename underlying_type<T>::type;

template <typename T>
struct TypeSigns
{
    using SignedT = typename std::make_signed<T>::type;
    using UnsignedT = typename std::make_unsigned<T>::type;
};

}}} // END NAMESPACES.
// =====================================================================================================================
