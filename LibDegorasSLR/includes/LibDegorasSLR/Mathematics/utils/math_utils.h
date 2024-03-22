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
 * @file math_utils.h
 * @brief This file contains the declarations of the mathematical utilities functions.
 * @author Degoras Project Team
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <cmath>
#include <omp.h>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Mathematics/types/euclidean_div_result.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr {
namespace math {
// =====================================================================================================================

template<typename T>
T pow2(const T& x)
{
    return x*x;
}

template<typename T>
T pow3(const T& x)
{
    return x*x*x;
}

/**
 * @brief Truncates a number with a given number of decimal places.
 * @param x, the number to be truncated.
 * @param prec, the total number of digits of the number to be truncated, including whole number and fractional part.
 * @param dec_places, the maximum size of the fractional part.
 * @return
 */
template<typename T>
double truncToDouble(T x, unsigned prec, unsigned dec_places);

/**
 * @brief Rounds a number with a given numer of decimal places.
 * @param x, the number to be rounded.
 * @param dec_places, the decimal places taken into account to round the number.
 * @return the number rounded as double.
 */
template<typename T>
double roundToDouble(T x, unsigned dec_places);

/**
 * @brief Normalize a value within a specified range.
 * @param x The value to be normalized.
 * @param x_min The minimum value of the range.
 * @param x_max The maximum value of the range.
 * @return The normalized value within the specified range.
 */
template <typename T>
T normalizeVal(T x, T x_min, T x_max);

/**
 * @brief Euclidean division for integral types.
 *
 *        This function calculates the quotient and remainder of the Euclidean division for two integral
 *        values. It is neccesary because the std function not calculates the result in the mathematical
 *        sense, the remainder can be negative.
 *
 * @tparam T The integral type for the division.
 * @param a, The dividend.
 * @param b, The divisor.
 * @return An instance of LldivResult<T> containing the quotient and remainder.
 */
template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
types::EuclideanDivResult<T> euclidDivLL(T a, T b)
{
    types::EuclideanDivResult<T> res;
    T r = a % b;
    r =  r >= 0 ? r : r + std::abs(b);
    T q = (a - r) / b;
    res.r = static_cast<typename types::EuclideanDivResult<T>::UnsignedT>(r);
    res.q = q;
    return res;
}

/**
 * @brief Compare floating points arguments a and b.
 * @param a, first operand
 * @param b, second operand
 * @param epsilon, the difference between a floating point number and the consecutive one.
 * @return 1 if a > b, 0 if a == b, -1 a < b.
 */
template <typename T>
std::enable_if_t<helpers::traits::is_floating_v<T>, int>
compareFloating(T a, T b, T epsilon = std::numeric_limits<T>::epsilon())
{
    T aux = a - b;
    return std::abs(aux) < epsilon ? 0 : std::signbit(aux) ? -1 : 1;
}

template <typename T>
std::enable_if_t<helpers::traits::is_floating_v<T>, bool>
isFloatingZeroOrMinor(T a, T epsilon = std::numeric_limits<T>::epsilon())
{
    return (compareFloating(a, static_cast<T>(0.0L), epsilon) <= 0);
}

template <typename T>
std::enable_if_t<helpers::traits::is_floating_v<T>, bool>
isFloatingMinorThanZero(T a, T epsilon = std::numeric_limits<T>::epsilon())
{
    return (compareFloating(a, static_cast<T>(0.0L), epsilon) < 0);
}

template <typename T>
std::enable_if_t<helpers::traits::is_floating_v<T>, std::vector<T>>
linspaceStep(const T& start, const T& end, const T& step)
{
    std::vector<T> result;

    if (step == 0)
        return result;

    // Calculate the number of values based on the step size
    size_t num = static_cast<size_t>(std::ceil((end - start) / step)) + 1;

    result.resize(num);

    #pragma omp parallel for
    for (size_t i = 0; i < num; ++i)
    {
        result[i] = start + step * static_cast<T>(i);
    }

    return result;
}


}} // END NAMESPACES
// =====================================================================================================================

// TEMPLATES INCLUDES
// =====================================================================================================================
#include <LibDegorasSLR/Mathematics/utils/math_utils.tpp>
// =====================================================================================================================
