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
 * @version 2306.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Statistics/types/statistics_types.h"
#include "LibDegorasSLR/Mathematics/types/matrix.h"
#include "LibDegorasSLR/Mathematics/types/vector3d.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace stats{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using math::types::Matrix;
using math::types::Vector3D;
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Performs Lagrange polynomial interpolation on the given data points and degree.
 *
 * The function returns the interpolated y-values for each variable in the y_interp vector. The x_interp value must
 * be within the range of x-values; otherwise, the function returns LagrangeError::X_OUT_OF_BOUNDS. The function uses
 * the Lagrange polynomial formula to compute the interpolated values.
 *
 * @tparam T The type of the x-values and interpolated value.
 * @tparam U The type of the y-values.
 * @param x The vector of x-values (must be sorted).
 * @param Y The matrix of y-values, where each row represents a different variable and each column represents a data point.
 * @param degree The degree of the Lagrange polynomial (number of points - 1, order 9 will use 10 points).
 * @param x_interp The x-value at which interpolation is performed.
 * @param y_interp The resulting interpolated y-values for each variable.
 * @return The LagrangeError indicating the result of the interpolation.
 */
template <typename T, typename U>
typename std::enable_if_t<
    (std::is_floating_point_v<T> || helpers::types::is_strong_float<T>::value) &&
        (std::is_floating_point_v<U> || helpers::types::is_strong_float<U>::value),
    types::LagrangeError>
lagrangeInterpol(const std::vector<T>& x, const Matrix<U>& Y, unsigned degree, const T& x_interp,
                 std::vector<U>& y_interp);

template <typename T, typename U>
typename std::enable_if_t<
    (std::is_floating_point_v<T> || helpers::types::is_strong_float<T>::value) &&
    (std::is_floating_point_v<U> || helpers::types::is_strong_float<U>::value),
    types::LagrangeError>
lagrangeInterpol3DVec(const std::vector<T>& x, const Matrix<T>& Y, unsigned degree, const T& x_interp,
                      Vector3D<U>& y_interp);

/**
 * @brief Computes the robust bisquare weights (Tukey bisquare) for robust regression or outlier detection.
 *
 * Given the input vectors x, y, and yc, this function computes the robust bisquare weights based on the bisquare
 * weight function. The weights are designed to downweight outliers and provide robust estimates in the presence of
 * outliers in the data.
 *
 * @param x The input vector x for the regression or outlier detection.
 * @param y The observed values vector y for the regression or outlier detection.
 * @param yc The predicted values vector yc for the regression or outlier detection.
 * @param K The tuning constant for the bisquare weight function. Default value is 4.685.
 * @return A vector of computed bisquare weights.
 *
 * @tparam T The data type of the input vectors `x`, `y`, and `yc`.
 * @tparam Ret The data type of the returned weights. Defaults to the same data type as the input vectors.
 */
template <typename T, typename Ret = T>
std::vector<Ret> robustBisquareWeights(const std::vector<T>& x, const std::vector<T>& y, const std::vector<T>& yc,
                                       const double K = 4.685);

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
                               types::PolyFitRobustMethod robust = types::PolyFitRobustMethod::NO_ROBUST);

/**
 * @brief Resolve for @param x the polynomial given by its coefficients in @param coefs.
 * @param coefs, the coefficients of the polynomial: coefs[0] + coefs[1] * x + cofes[2] * x^2 + ... + coefs[n] * x^n
 * @param x, the independent variable for resolving the polynomial.
 * @return The solution of the polynomial: coefs[0] + coefs[1] * x + cofes[2] * x^2 + ... + coefs[n] * x^n.
 *         If @param coefs is empty, returns 0.
 */
template <typename T>
T applyPolynomial(const std::vector<T>& coefs, T x);


/**
 * @brief Removes polynomial trend from the data in y.
 * @param x, a vector with the independent variable values.
 * @param y, a vector with the dependent variable values. It must have the same size as x.
 * @param degree, the degree of the polynomial fit using for detrend.
 * @return The data with the trend removed.
 * @note When degree = 1, detrend removes the linear trend.
 */
template <typename T, typename Ret = T>
std::vector<Ret> detrend(const std::vector<T>& x, const std::vector<T>& y, unsigned int degree);

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
                         const std::vector<T>& xinterp, const std::vector<T>& yinterp, unsigned int degree);

}} // END NAMESPACES.
// =====================================================================================================================

// TEMPLATES INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Statistics/fitting.tpp"
// =====================================================================================================================
