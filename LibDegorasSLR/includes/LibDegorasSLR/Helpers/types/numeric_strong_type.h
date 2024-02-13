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
 * @file numeric_strong_type.h
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
#include <algorithm>
#include <type_traits>
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "type_traits.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace helpers{
namespace types{
// =====================================================================================================================

// Strong type template
template<typename T, class Tag>
class NumericStrongType
{
    static_assert(std::is_arithmetic_v<T>,
                  "[LibDegorasBase,Helpers,NumericStrongType] Can only be used with numeric types.");
    T value;

public:

    NumericStrongType() = default;

    NumericStrongType(T const& value) : value(value) {}

    NumericStrongType(const NumericStrongType&) = default;

    NumericStrongType(NumericStrongType&&) = default;

    NumericStrongType& operator=(const NumericStrongType& other)
    {
        if (this != &other)
        {
            this->value = other.value;
        }
        return *this;
    }

    NumericStrongType& operator=(NumericStrongType&& other) noexcept
    {
        if (this != &other)
        {
            this->value = std::move(other.value);
        }
        return *this;
    }

    // Overloading += operator
    NumericStrongType& operator+=(const T& rhs)
    {
        this->value += rhs;
        return *this;
    }

    // Overloading -= operator
    NumericStrongType& operator-=(const T& rhs)
    {
        this->value -= rhs;
        return *this;
    }

    // Overloading += operator for NumericStrongType
    NumericStrongType& operator+=(const NumericStrongType& rhs)
    {
        this->value += rhs.value;
        return *this;
    }

    // Overloading -= operator for NumericStrongType
    NumericStrongType& operator-=(const NumericStrongType& rhs)
    {
        this->value -= rhs.value;
        return *this;
    }

    // Prefix increment operator
    NumericStrongType& operator++()
    {
        ++this->value;
        return *this;
    }

    // Postfix increment operator
    NumericStrongType operator++(int)
    {
        NumericStrongType tmp(*this);
        operator++();
        return tmp;
    }

    // Prefix decrement operator
    NumericStrongType& operator--()
    {
        --this->value;
        return *this;
    }

    // Postfix decrement operator
    NumericStrongType operator--(int)
    {
        NumericStrongType tmp(*this);
        operator--();
        return tmp;
    }

    // Conversion back to the underlying type.
    operator T() const { return this->value; }

    // Getter for the value.
    T get() const { return this->value; }
};

// Type traits.

template<typename T, typename Tag>
struct is_strong_float<NumericStrongType<T, Tag>> : std::is_floating_point<T> {};

template<typename T, typename Tag>
struct is_strong_integral<NumericStrongType<T, Tag>> : std::is_integral<T> {};

// Trait to check if a type is a NumericStrongType
template<typename T>
struct is_numeric_strong_type : std::false_type {};

template<typename T, class Tag>
struct is_numeric_strong_type<NumericStrongType<T, Tag>> : std::true_type {};

template<typename T, class Tag>
struct underlying_type<NumericStrongType<T, Tag>> {using type = T;};

}}} // END NAMESPACES.
// =====================================================================================================================

// Specializing std::numeric_limits for NumericStrongType
namespace std
{

template<typename T, class Tag>
class numeric_limits<dpslr::helpers::types::NumericStrongType<T, Tag>>
{
public:
    static constexpr bool is_specialized = numeric_limits<T>::is_specialized;

    static constexpr auto max_digits10 = std::numeric_limits<T>::max_digits10;
    static constexpr auto digits10 = std::numeric_limits<T>::digits10;
    static constexpr T min() noexcept { return numeric_limits<T>::min(); }
    static constexpr T max() noexcept { return numeric_limits<T>::max(); }
    static constexpr T epsilon() noexcept { return numeric_limits<T>::epsilon(); }
};

}

// =====================================================================================================================
