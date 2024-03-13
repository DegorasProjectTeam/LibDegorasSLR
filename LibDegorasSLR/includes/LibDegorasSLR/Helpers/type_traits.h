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
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <type_traits>
#include <vector>
#include <array>
#include <sstream>
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Mathematics/types/numeric_strong_type.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace helpers{
namespace traits{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Type trait to extract the underlying type of a given type.
 *
 * This trait is primarily intended to be specialized for custom types that have an underlying type, such as strong
 * type wrappers or enums. By default, it provides the type itself as the underlying type.
 *
 * @tparam T The type for check.
 */
template<typename T, typename = void>
struct underlying_type {using type = T;};

/// Type trait underlying_type specialization for NumericStrongType type.
template<typename T, class Tag>
struct underlying_type<math::types::NumericStrongType<T, Tag>> {using type = T;};

/// Helper type alias for underlying_type<T>::type direct access.
template<typename T>
using underlying_type_t = typename underlying_type<T>::type;

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Type trait to check if a type is a NumericStrongType type.
 * @tparam T The type for check.
 */
template<typename T, typename = void>
struct is_numeric_strong_type : std::false_type {};

/// True case for is_numeric_strong_type type trait.
template<typename T, class Tag>
struct is_numeric_strong_type<math::types::NumericStrongType<T, Tag>> : std::true_type {};

/// Variable template for direct access to is_numeric_strong_type value.
template<typename T>
inline constexpr bool is_numeric_strong_type_v = is_numeric_strong_type<T>::value;

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Type trait to check if a type is a NumericStrongType type with a floating point underlying type.
 * @tparam T The type for check.
 */
template<typename T, typename = void>
struct is_strong_floating : std::false_type {};

template<typename T, typename Tag>
struct is_strong_floating<math::types::NumericStrongType<T, Tag>> : std::is_floating_point<T> {};

/// Variable template for direct access to is_strong_floating value.
template<typename T>
inline constexpr bool is_strong_floating_v = is_strong_floating<T>::value;

// ---------------------------------------------------------------------------------------------------------------------

/// Type trait to check if a type is a NumericStrongType with an integral underlying type.
template<typename T>
struct is_strong_integral : std::false_type {};


template<typename T, typename Tag>
struct is_strong_integral<math::types::NumericStrongType<T, Tag>> : std::is_integral<T> {};

/// Variable template for direct access to is_strong_integral value.
template<typename T>
inline constexpr bool is_strong_integral_v = is_strong_integral<T>::value;

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Trait to check if a type is either a standard floating-point type or a strong floating-point type.
 * @tparam T The type to check.
*/
template<typename T>
struct is_floating : std::integral_constant<bool, std::is_floating_point_v<T> || is_strong_floating_v<T>>{};

/// Variable template for direct access to is_floating value.
template<typename T>
inline constexpr bool is_floating_v = is_floating<T>::value;

// ---------------------------------------------------------------------------------------------------------------------

/// Type trait to check if a type is integral, floating, strong integral or strong floating.
template<typename T>
struct is_numeric : std::integral_constant<bool,
    std::is_integral_v<T> || is_strong_integral_v<T> ||
    std::is_floating_point_v<T> || is_strong_floating_v<T>>{};

/// Variable template for direct access to is_numeric value.
template<typename T>
inline constexpr bool is_numeric_v = is_numeric<T>::value;

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Type trait to check if both specified types are floating-point types.
 *
 * This trait checks if both T1 and T2 are either standard floating-point types or are recognized as floating-point
 * types through the is_strong_floating_v trait. This can be used to enable template functions or classes only when
 * both types are floating-point types.
 *
 * @tparam T1 First type to check.
 * @tparam T2 Second type to check.
 */
template<typename T1, typename T2,  typename = void>
struct both_floating : std::false_type {};

/// Specialization when both T1 and T2 are either standard or strong floating-point types.
template<typename T1, typename T2>
struct both_floating<T1, T2, std::enable_if_t<
                (std::is_floating_point_v<T1> || traits::is_strong_floating_v<T1>) &&
                (std::is_floating_point_v<T2> || traits::is_strong_floating_v<T2>)>> : std::true_type {};

// Helper variable template for easier use.
template<typename T1, typename T2>
inline constexpr bool both_floating_v = both_floating<T1, T2>::value;

/// Trait to check if both types are either floating-point or integral (including strong types).
template<typename T, typename U, typename = void>
struct same_arithmetic_category : std::false_type {};

/// Trait specialization when both are floating-point or strong floating-point.
template<typename T, typename U>
struct same_arithmetic_category<T, U, std::enable_if_t<
                (std::is_floating_point_v<T> || is_strong_floating_v<T>) &&
                (std::is_floating_point_v<U> || is_strong_floating_v<U>)>> : std::true_type {};

/// Trait specialization when both are integral or strong integral.
template<typename T, typename U>
struct same_arithmetic_category<T, U, std::enable_if_t<
                (std::is_integral_v<T> || is_strong_integral_v<T>) &&
                (std::is_integral_v<U> || is_strong_integral_v<U>)>> : std::true_type {};

/// Variable template for direct access to same_arithmetic_category value.
template<typename T, typename U>
inline constexpr bool same_arithmetic_category_v = same_arithmetic_category<T, U>::value;




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

/// Variable template for direct access to is_streamable value.
template<typename T>
inline constexpr bool is_streamable_v = is_streamable<T>::value;


}}} // END NAMESPACES.
// =====================================================================================================================
