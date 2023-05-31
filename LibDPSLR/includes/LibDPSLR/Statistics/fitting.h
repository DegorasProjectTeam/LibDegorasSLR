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
 * @file fitting.h
 * @author Degoras Project Team.
 * @brief This file contains the declaration of several functions related with data fitting.
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDPSLR/Statistics/fitting.tpp"
#include "LibDPSLR/Mathematics/matrix.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace stats{
// =====================================================================================================================

/**
 * @brief Calculates the Lagrange interpolation
 * @param x
 * @param Y
 * @param degree
 * @param x_interp
 * @param y_interp
 * @return
 */template <typename T, typename U>
LagrangeResult lagrangeInterp(const std::vector<T>& x, const dpslr::math::Matrix<T>& Y, unsigned int degree,
                              T x_interp, std::vector<U>& y_interp)
{
    return dpslr::stats_private::lagrangeInterp(x, Y, degree, x_interp, y_interp);
}


/**
 * @brief Gets the polynomial fit coefficients for x,y.
 * @param x, a vector with the independent variable values.
 * @param y, a vector with the dependent variable values. It must have the same size as x.
 * @param degree, the degree of the polynomial fit
 * @param w, a vector with the weights applied to each observation. It must be empty or have the same size as x.
 * @param robust, the robust fit method selected
 * @return The coefficients of the polynomial fit for x and x, or empty vector if x and x sizes are not equal.
 *         The order of the coefficients in the returned vector is c[0] + c[1] * x + c[2] * x^2 + ... + c[n] * x^n.
 */
template <typename T, typename Ret = T>
std::vector<Ret> polynomialFit(const std::vector<T>& x, const std::vector<T>& y, unsigned int degree,
                               const std::vector<T>& w = std::vector<T>(),
                               dpslr::stats::PolyFitRobustMethod robust = dpslr::stats::PolyFitRobustMethod::NO_ROBUST)
{
    return dpslr::stats_private::polynomialFit(x, y, degree, w, robust);
}

/**
 * @brief Resolve for @param x the polynomial given by its coefficients in @param coefs.
 * @param coefs, the coefficients of the polynomial: coefs[0] + coefs[1] * x + cofes[2] * x^2 + ... + coefs[n] * x^n
 * @param x, the independent variable for resolving the polynomial.
 * @return The solution of the polynomial: coefs[0] + coefs[1] * x + cofes[2] * x^2 + ... + coefs[n] * x^n.
 *         If @param coefs is empty, returns 0.
 */
template <typename T>
T applyPolynomial(const std::vector<T>& coefs, T x)
{
    return dpslr::stats_private::applyPolynomial(coefs, x);
}


/**
 * @brief Removes polynomial trend from the data in y.
 * @param x, a vector with the independent variable values.
 * @param y, a vector with the dependent variable values. It must have the same size as x.
 * @param degree, the degree of the polynomial fit using for detrend.
 * @return The data with the trend removed.
 * @note When degree = 1, detrend removes the linear trend.
 */
template <typename T, typename Ret = T>
std::vector<Ret> detrend(const std::vector<T>& x, const std::vector<T>& y, unsigned int degree)
{
    return dpslr::stats_private::detrend(x, y, degree);
}

/**
 * @brief Removes polynomial trend from the data in y, using xinterp, yinterp to form the polynomial fit
 * @param x, a vector with the independent variable values.
 * @param y, a vector with the dependent variable values. It must have the same size as x.
 * @param xinterp, a vector with the independent variable for polynomial fit.
 * @param yinterp, a vector with the dependent variable for polynomial fit. It must have the same size as xinterp.
 * @param degree, the degree of the polynomial fit using for detrend.
 * @return The data with the trend removed.
 * @note When degree = 1, detrend removes the linear trend.
 */
template <typename T, typename Ret = T>
std::vector<Ret> detrend(const std::vector<T>& x, const std::vector<T>& y,
                         const std::vector<T>& xinterp, const std::vector<T>& yinterp, unsigned int degree)
{
    return dpslr::stats_private::detrend(x, y, xinterp, yinterp, degree);
}

}} // END NAMESPACES.
// =====================================================================================================================
