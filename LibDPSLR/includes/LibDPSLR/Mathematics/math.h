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
 * @file math.h
 * @brief This file contains the declarations of the mathematical functions.
 * @author Degoras Project Team
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

#pragma once

// ========== INTERNAL INCLUDES ========================================================================================
#include "LibDPSLR/Mathematics/common/math_types.h"
#include "LibDPSLR/Mathematics/math.tpp"
// =====================================================================================================================


// ========== DPSLR NAMESPACES =========================================================================================
namespace dpslr {
namespace math {
// =====================================================================================================================


template<typename T>
T pow2(T x)
{
    return x*x;
}

/**
 * @brief Truncates a number with a given number of decimal places.
 * @param x, the number to be truncated.
 * @param prec, the total number of digits of the number to be truncated, including whole number and fractional part.
 * @param dec_places, the maximum size of the fractional part.
 * @return
 */
template<typename T>
double truncToDouble(T x, unsigned int prec, unsigned int dec_places)
{
    return dpslr::math_private::truncToDouble(x, prec, dec_places);
}

/**
 * @brief Rounds a number with a given numer of decimal places.
 * @param x, the number to be rounded.
 * @param dec_places, the decimal places taken into account to round the number.
 * @return the number rounded as double.
 */
template<typename T>
double roundToDouble(T x, unsigned int dec_places)
{
    return dpslr::math_private::roundToDouble(x, dec_places);
}

/**
 * @brief Normalize a value within a specified range.
 * @param x The value to be normalized.
 * @param x_min The minimum value of the range.
 * @param x_max The maximum value of the range.
 * @return The normalized value within the specified range.
 */
template <typename T>
T normalizeVal(T x, T x_min, T x_max)
{
    return dpslr::math_private::normalizeVal(x, x_min, x_max);
}

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
common::EuclideanDivResult<T> euclidDivLL(T a, T b)
{
    common::EuclideanDivResult<T> res;
    T r = a % b;
    r =  r >= 0 ? r : r + std::abs(b);
    T q = (a - r) / b;
    res.r = static_cast<typename common::EuclideanDivResult<T>::UnsignedT>(r);
    res.q = q;
    return res;
};

/**
 * @brief Compare floating points arguments a and b.
 * @param a, first operand
 * @param b, second operand
 * @param epsilon, the difference between a floating point number and the consecutive one.
 * @return 1 if a > b, 0 if a == b, -1 a < b.
 */
template <typename T>
std::enable_if_t<std::is_floating_point<T>::value, int>
compareFloating(T a, T b, T epsilon = std::numeric_limits<T>::epsilon())
{
    return dpslr::math_private::compareFloating(a, b, epsilon);
}

/**
 * @brief Generate a 3D euclidean rotation matrix. To apply the rotation just multiply by the matrix.
 * @param axis, the axis in which the rotation is applied.
 * @param angle, the angle of rotation applied.
 * @param matrix, the generated rotation matrix
 */
template <typename T>
void euclid3DRotMat(int axis, T angle, Matrix<T> &matrix)
{
    return dpslr::math_private::euclid3DRotMat(axis, angle, matrix);
}

} // END NAMESPACE MATH
// =====================================================================================================================

} // END NAMESPACE DPSLR
// =====================================================================================================================

// =====================================================================================================================
