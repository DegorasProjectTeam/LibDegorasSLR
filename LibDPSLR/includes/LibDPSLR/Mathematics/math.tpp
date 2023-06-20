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
 * @file math.tpp
 * @brief This file contains the template implementation of the mathematical functions.
 * @author Degoras Project Team
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

#pragma once

// C++ INCLUDES
//======================================================================================================================
#include <vector>
#include <numeric>
#include <cmath>
#include <functional>
#include <algorithm>
#include <omp.h>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDPSLR/Mathematics/containers/matrix.h"
#include "LibDPSLR/Helpers/string_helpers.h"
// =====================================================================================================================

// ========== DPSLR NAMESPACES =========================================================================================
namespace dpslr {
namespace math_private  {
// =====================================================================================================================

template <typename T>
std::enable_if_t<std::is_floating_point<T>::value, int>
compareFloating(T a, T b, T epsilon)
{
    T aux = a - b;
    return std::abs(aux) < epsilon ? 0 : std::signbit(aux) ? -1 : 1;
}

template<typename T>
double truncToDouble(T x, unsigned int prec, unsigned int dec_places)
{
    return std::stod(dpslr::helpers::strings::numberToStr(x, prec, dec_places));
}

template<typename T>
double roundToDouble(T x, unsigned int dec_places)
{
    const long double multiplier = std::pow(10.0, dec_places);
    return std::round(x * multiplier) / multiplier;
}

template <typename T>
T normalizeVal(T x, T x_min, T x_max)
{
    return std::fmod(((std::fmod((x - x_min),(x_max - x_min)))+(x_max - x_min)), (x_max - x_min)) + x_min;
}

template <typename T>
void euclid3DRotMat(int axis, T angle, dpslr::math::Matrix<T> &matrix)
{
    double s, c;
    unsigned int caxis = static_cast<unsigned int>(axis - 1);
    matrix.fill(3,3,0);
    s= std::sin(angle);
    c= std::cos(angle);
    matrix[0][0]=c;
    matrix[1][1]=c;
    matrix[2][2]=c;
    matrix[0][1]=-s;
    matrix[1][2]=-s;
    matrix[2][0]=-s;
    matrix[1][0]=s;
    matrix[2][1]=s;
    matrix[0][2]=s;
    for (unsigned i=0; i<3; i++)
    {
      matrix[i][caxis] = 0.0;
      matrix[caxis][i] = 0.0;
    }
    matrix[caxis][caxis]= 1.0;
}

}} // END NAMESPACES.
// =====================================================================================================================
