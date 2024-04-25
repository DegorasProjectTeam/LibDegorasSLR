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
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <algorithm>
#include <type_traits>
#include <limits>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Helpers/string_helpers.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace math{
namespace types{
// =====================================================================================================================

// Strong type template
template<typename T, class Tag>
class NumericStrongType
{
    static_assert(std::is_arithmetic_v<T>,
                  "[LibDegorasBase,Helpers,NumericStrongType] Can only be used with numeric types.");
    T value_;

public:

    constexpr NumericStrongType(){this->value_ = static_cast<T>(0);}

    constexpr NumericStrongType(const T& value) : value_(value) {}

    constexpr NumericStrongType(const NumericStrongType&) = default;

    constexpr NumericStrongType(NumericStrongType&&) = default;

    constexpr NumericStrongType& operator=(const NumericStrongType& other)
    {
        if (this != &other)
            this->value_ = other.value_;
        return *this;
    }

    constexpr NumericStrongType& operator=(NumericStrongType&& other) noexcept
    {
        if (this != &other)
            this->value_ = std::move(other.value_);
        return *this;
    }

    // Overloading += operator
    constexpr NumericStrongType& operator+=(const T& rhs)
    {
        this->value_ += rhs;
        return *this;
    }

    // Overloading -= operator
    constexpr NumericStrongType& operator-=(const T& rhs)
    {
        this->value_ -= rhs;
        return *this;
    }

    // Overloading += operator for NumericStrongType
    constexpr NumericStrongType& operator+=(const NumericStrongType& rhs)
    {
        this->value_ += rhs.value_;
        return *this;
    }

    // Overloading -= operator for NumericStrongType
    constexpr NumericStrongType& operator-=(const NumericStrongType& rhs)
    {
        this->value_ -= rhs.value_;
        return *this;
    }

    // Prefix increment operator
    constexpr NumericStrongType& operator++()
    {
        ++this->value_;
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
    constexpr NumericStrongType& operator--()
    {
        --this->value_;
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
    constexpr operator T() const { return this->value_; }

    // Getter for the value_.
    constexpr T get() const { return this->value_; }

    std::string toString() const
    {
        return helpers::strings::numberToMaxDecStr(this->value_);
    }
};

}}} // END NAMESPACES.
// =====================================================================================================================

// STD NAMESPACES
// =====================================================================================================================
namespace std {
// =====================================================================================================================

// Specializing std::numeric_limits for NumericStrongType
template<typename T, class Tag>
class numeric_limits<dpslr::math::types::NumericStrongType<T, Tag>>
{
public:
    static constexpr bool is_specialized = std::numeric_limits<T>::is_specialized;
    static constexpr auto max_digits10 = std::numeric_limits<T>::max_digits10;
    static constexpr auto digits10 = std::numeric_limits<T>::digits10;
    static constexpr T min() noexcept { return std::numeric_limits<T>::min(); }
    static constexpr T max() noexcept { return std::numeric_limits<T>::max(); }
    static constexpr T epsilon() noexcept { return std::numeric_limits<T>::epsilon(); }
};

} // END NAMESPACES.
// =====================================================================================================================

// Macros
// =====================================================================================================================

// Macro to define custom strong types with literals.
#define M_DEFINE_STRONG_TYPE(TypeName, NumericType) \
using TypeName = dpslr::math::types::NumericStrongType<NumericType, struct UnitName##Tag>; \

// Macro to define units and create literal operator
#define M_DEFINE_STRONG_TYPE_WITH_LITERAL(TypeName, NumericType, TypeLiteral) \
M_DEFINE_STRONG_TYPE(TypeName, NumericType) \
namespace literals { \
inline TypeName operator ""_##TypeLiteral(NumericType value) {return TypeName(value);} } \

// =====================================================================================================================
