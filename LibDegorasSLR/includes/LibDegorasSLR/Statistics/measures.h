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
 * @file measures.h
 * @author Degoras Project Team.
 * @brief This file contains the functions definition of several stadistics measures.
 * @copyright EUPL License
 2305.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <cmath>
#include <numeric>
#include <algorithm>
#include <vector>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================

// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace stats{
namespace measures{
// =====================================================================================================================

// ========== FUNCTIONS ===============================================================================================

/**
 * @brief Calculates the mean of a distribution.
 * @param x The input vector containing the distribution data.
 * @return The mean value.
 */template <typename T>
T mean(const std::vector<T> &x)
{
    return std::accumulate(x.begin(), x.end(), 0.0) / x.size();
}

/**
 * @brief Calculates the median of a distribution.
 * @param x The input vector containing the distribution data.
 * @return The median value.
 */
template <typename T>
T median(const std::vector<T>& x, bool sorted = false)
{
    if (x.empty())
        return 0;

    T med;

    // If x is sorted, avoid copy and sort. Once data is sort, choose median element:
    // centre if even elements, mean of two centre elements othersie)
    if (sorted)
    {
        med = (0 == x.size() % 2) ? (x[x.size() / 2] + x[(x.size() / 2) - 1]) / 2. : x[x.size() / 2];
    }
    else
    {
        auto x_sort = x;
        std::sort(x_sort.begin(), x_sort.end());
        med = (0 == x.size() % 2) ? (x_sort[x.size() / 2] + x_sort[(x.size() / 2) - 1]) / 2. : x_sort[x.size() / 2];
    }

    return med;
}

/**
 * @brief Calculates the variance of a distribution.
 * @param x The input vector containing the distribution data.
 * @return The variance value.
 */
template <typename T>
T var(const std::vector<T>& x)
{
    T m = mean(x);
    T u2 = 0.0;

    for (const auto& value : x)
    {
        T x_minus_mean = value - m;
        u2 += std::pow(x_minus_mean, 2);
    }

    return u2 / x.size();
}

/**
 * @brief Calculates the skewness of a distribution using the method of moments.
 * @param x The input vector containing the distribution data.
 * @param bias Flag indicating whether to apply bias correction. Default is true.
 * @return The skewness value.
 */
template <typename T>
T skew(const std::vector<T> &x, bool bias = true)
{
    T mean = mean(x);
    T u2 = 0.0;
    T u3 = 0.0;
    for (const auto& value : x)
    {
        T x_minus_mean = value - mean;
        u2 += std::pow(x_minus_mean, 2);
        u3 += std::pow(x_minus_mean, 3);
    }
    u2 /= x.size();
    u3 /= x.size();

    T res = u3 / std::pow(u2, 1.5);

    if (!bias && x.size() > 2)
    {
        T n = static_cast<T>(x.size());
        T sk_bias = std::sqrt((n * (n - 1))) / (n - 2);
        res *= sk_bias;
    }

    return res;
}

/**
 * @brief Compute the kurtosis (Fisher or Pearson) of a vector of values.
 *
 *        Kurtosis is the fourth central moment divided by the square of the variance. If Fisher’s definition
 *        is used, then 3.0 is subtracted from the result to give 0.0 for a normal distribution.
 *
 *        If bias is false, then the calculations are corrected using k statistics to eliminate bias coming from
 *        biased moment estimators
 *
 * @param x         The input vector containing the distribution data.
 * @param fisher    Flag indicating whether to apply Fisher's correction. Default is false.
 * @param bias      Flag indicating whether to apply bias correction. Default is true.
 * @return The kurtosis value.
 */
template <typename T>
T kurtosis(const std::vector<T> &x, bool fisher = false, bool bias = true)
{
    T mean = mean(x);
    T u2 = 0.0;
    T u4 = 0.0;
    for (const auto& value : x)
    {
        T x_minus_mean = value - mean;
        u2 += std::pow(x_minus_mean, 2);
        u4 += std::pow(x_minus_mean, 4);
    }
    u2 /= x.size();
    u4 /= x.size();

    T res =  u4 / std::pow(u2, 2);

    if(fisher)
        res -= 3;

    if (!bias && x.size() > 2)
    {
        T n = static_cast<T>(x.size());
        T k_bias = ((n - 1) * (n + 1)) * ((n - 2) * (n - 3));
        res = (n * (n + 1) * res - 3 * (n - 1) * (n - 1)) / k_bias;
    }

    return res;
}

/**
 * @brief Calculates the Standard Deviation of a distribution.
 * @param x The input vector containing the distribution data.
 * @return The standard deviation of the distribution.
 */
template <typename T>
T stddev(const std::vector<T>& x)
{
    return std::sqrt(var(x));
}

/**
 * @brief Calculates the Root Mean Squared of a distribution.
 * @param x The input vector containing the distribution data.
 * @return The Root Mean Squared calculation for data in x.
 */
template <typename T>
T rms(const std::vector<T>& x)
{
    return std::sqrt(std::inner_product(x.begin(), x.end(), x.begin(), T()) / x.size());
}

/**
 * @brief Calculates the leverage values for a distribution.
 * @param x The input vector containing the distribution data.
 * @return A vector with the leverage values.
 */
template <typename T, typename Ret = T>
std::vector<Ret> leverage(const std::vector<T>& x)
{
    // Leverage formula: hi = 1/n + ((xi - xm)^2 / sumsqr(x - xm))
    auto m = mean(x);
    std::vector<T> leverage_v;
    // First insert x minus mean
    std::transform(x.begin(), x.end(), std::back_inserter(leverage_v), [m](const auto& e){return e - m;});
    // Calculate deviations sum sqr
    auto devsqr = std::inner_product(leverage_v.begin(), leverage_v.end(), leverage_v.begin(), T());
    auto n_inv = 1./ x.size();
    std::transform(leverage_v.begin(), leverage_v.end(), leverage_v.begin(),
                   [devsqr, n_inv](const auto& e){return n_inv + (e*e / devsqr);});
    return leverage_v;
}

}}} // END NAMESPACES.
// =====================================================================================================================
