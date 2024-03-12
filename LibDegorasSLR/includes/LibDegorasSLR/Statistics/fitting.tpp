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
 * @file fitting.tpp
 * @author Degoras Project Team.
 * @brief This file contains the template implementation of several functions related with data fitting.
 * @copyright EUPL License
 * @version 2306.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include <vector>
#include <cmath>
#include <numeric>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Statistics/types/statistics_types.h"
#include "LibDegorasSLR/Statistics/measures.h"
#include "LibDegorasSLR/Mathematics/types/matrix.h"
#include "LibDegorasSLR/Mathematics/types/vector3d.h"
#include "LibDegorasSLR/Helpers/type_traits.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace stats{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using types::LagrangeError;
using types::PolyFitRobustMethod;
using math::types::Matrix;
using math::types::Vector3D;
// ---------------------------------------------------------------------------------------------------------------------

template <typename T, typename U>
typename std::enable_if_t<helpers::traits::both_floating_v<T,U>, types::LagrangeError>
lagrangeInterpol(const std::vector<T>& x, const Matrix<U>& Y, unsigned degree, const T& x_interp,
                 std::vector<U>& y_interp)
{
    // Variables.
    unsigned first_point;
    int aux;
    LagrangeError error = LagrangeError::NOT_ERROR;

    if (x.size() > 0 && x.size() == Y.rowSize())
    {
        // x_interp is not within x range, so it cannot be interpolated.
        if (x_interp < x[0] || x_interp > x.back())
            return LagrangeError::X_OUT_OF_BOUNDS;

        // Look for given value immediately after interpolation argument
        aux = 0;
        do
        {
            aux++;
        }while(x[static_cast<std::size_t>(aux)] < x_interp);

        // Get first interpolator point. The first point should leave the interpolated point in the middle.
        aux -= (degree + 1)/2;
        if (aux < 0)
        {
            first_point = 0;
            error = LagrangeError::NOT_IN_THE_MIDDLE;
        }
        else if (static_cast<unsigned>(aux) + degree >= x.size())
        {
            first_point = static_cast<unsigned>(x.size() - degree - 1);
            error = LagrangeError::NOT_IN_THE_MIDDLE;
        }
        else
        {
            first_point = static_cast<unsigned int>(aux);
        }

        y_interp.clear();
        y_interp.insert(y_interp.begin(), Y.columnsSize(), 0);

        // Apply Lagrange polynomial interpolation to all variables in Y.
        for (unsigned i = first_point; i <= first_point + degree; i++)
        {
            std::common_type_t<T,U> pj=1.0;
            for(unsigned j = first_point; j <= first_point + degree; j++)
            {
                if (j != i) pj*=(x_interp-x[j])/(x[i]-x[j]);
            }
            for (unsigned variable = 0; variable < Y.columnsSize(); variable++)
            {
                y_interp[variable]+=Y[i][variable]*pj;
            }
        }
    }
    else
    {
        error = LagrangeError::DATA_SIZE_MISMATCH;
    }

    // Return the error.
    return error;
}

template <typename T, typename U>
typename std::enable_if_t<helpers::traits::both_floating_v<T,U>, types::LagrangeError>
lagrangeInterpol3DVec(const std::vector<T>& x, const Matrix<U>& Y, unsigned degree, const T& x_interp,
                      Vector3D<U>& y_interp)
{
    // Auxiliar containers.
    stats::types::LagrangeError lag_res;
    std::vector<U> res_y;
    // Call to lagrange.
    lag_res = lagrangeInterpol(x, Y, degree, x_interp, res_y);
    // Store the result.
    y_interp = {res_y[0], res_y[1], res_y[2]};
    // Return the error code.
    return lag_res;
}

template <typename T, typename Ret>
std::vector<Ret> robustBisquareWeights(const std::vector<T>& x, const std::vector<T>& y, const std::vector<T>& yc,
                                       const double K)
{
    auto leverages = measures::leverage(x);
    auto resids = y - yc;
    decltype(resids) resids_abs;
    std::transform(resids.begin(), resids.end(), std::back_inserter(resids_abs), [](const auto& v){return std::abs(v);});
    // Median absoulte deviation.
    long double mad = median(resids_abs);
    // For the standard normal E(MAD)=0.6745
    long double s = mad/0.6745L;
    long double Ks = static_cast<long double>(K)*s;
    std::vector<T> u;
    std::vector<Ret> w;

    // Adjust residuals using radj = r / sqrt(1 - h)
    std::transform(resids.begin(), resids.end(), leverages.begin(), resids.begin(),
                   [](const auto& r, const auto& h){return r/std::sqrt(1 - h);});

    // Standarize adjusted residuals using u = radj / K*s
    std::transform(resids.begin(), resids.end(), std::back_inserter(u), [Ks](const auto& r){return r / Ks;});

    // Compute robust weights using u = (1 - u^2)^2 if abs(u) < 1 or 0 otherwise
    std::transform(u.begin(), u.end(), std::back_inserter(w),
                   [](const auto& e){return std::abs(e) < 1 ? std::pow(1 - std::pow(e,2), 2) : 0;});

    return w;
}

template <typename T>
T applyPolynomial(const std::vector<T>& coefs, T x)
{
    if (coefs.empty())
        return T(0);

    std::vector<T> x_pow;

    for (std::size_t i = 0; i < coefs.size(); i++)
        x_pow.push_back(std::pow(x, i));

    return std::inner_product(coefs.begin(), coefs.end(), x_pow.begin(), T(0));
}

template <typename T, typename Ret>
std::vector<Ret> polynomialFit(const std::vector<T>& x, const std::vector<T>& y, unsigned int degree,
                               const std::vector<T>& w, PolyFitRobustMethod robust)
{

    // Variable declaration
    std::vector<Ret> coefs(degree+1);     // Resulting coeficients.
    std::vector<Ret> sigma_x(2*degree+1); // Values of sigma(xi),sigma(xi^2),sigma(xi^2n).
    Matrix<Ret> normal_matrix(degree+1, degree+2);
    unsigned int i, j, k;

    // Avoid X and Y size mismatch as well as weights mismatch if it is not empty
    if (x.size() != y.size() || (!w.empty() && x.size() != w.size()))
        return {};

    for (i = 0; i < 2*degree+1; i++)
    {
        sigma_x[i] = 0;
        for(j = 0; j < x.size(); j++)
        {
            auto sum = pow(x[j],i);
            if (!w.empty())
                sum *= w[j];
            sigma_x[i] += sum;
        }
    }

    // Build the Normal matrix by storing the corresponding coefficients (except for the last column of the matrix).
    for (i = 0; i <= degree; i++)
        for (j = 0; j <= degree; j++)
            normal_matrix[i][j] = sigma_x[i+j];

    // Compute last column of Normal matrix
    // Values of sigma(yi),sigma(xi*yi),sigma(xi^2*yi)...sigma(xi^n*yi)
    for (i = 0; i < degree+1; i++)
    {
        normal_matrix[i][degree+1] = 0;
        for (j = 0; j < x.size(); j++)
        {
            auto sum = pow(x[j],i) * y[j];
            if (!w.empty())
                sum *= w[j];

            normal_matrix[i][degree+1] += sum;
        }
    }

    // Increment the degree.
    degree++;

    // TODO: this may be optimized reducing number of swaps. It also does not control if matrix is singular.
    // Maybe gaussian elimination could be a separate function
    // From now, gaussian elimination starts solving the set of linear equations (pivotisation)
    for (i = 0; i < degree; i++)
        for (k = i + 1; k < degree; k++)
            if (normal_matrix[i][i] < normal_matrix[k][i])
                normal_matrix.swapRows(i,j);

    // Loop to perform the gaussian elimination
    for (i = 0; i < degree - 1; i++)
        for (k = i + 1; k < degree; k++)
        {
            Ret t = normal_matrix[k][i]/normal_matrix[i][i];
            for (j = 0; j <= degree; j++)
                normal_matrix[k][j] = normal_matrix[k][j] - t*normal_matrix[i][j];
        }

    for (int i = static_cast<int>(degree) - 1; i >= 0; i--)
    {
        coefs[i] = normal_matrix[i][degree];

        for (int j = 0; j < static_cast<int>(degree); j++)
            if (j != i)
                coefs[i] -= normal_matrix[i][j]*coefs[j];

        coefs[i] /= normal_matrix[i][i];
    }

    // If robust method is selected, calculate weights and recalculate coefficients
    if (PolyFitRobustMethod::BISQUARE_WEIGHTS == robust)
    {
        // In bisquare weights, weights are recalculated iteratively until coefficients converge
        int i = 0;
        const int limit = 400;
        const double threshold = 1e-6;
        std::vector<Ret> prev_coefs(degree+1);
        std::vector<T> y_c;
        std::vector<T> calc_weights;
        bool converged;

        do
        {
            // Compute calculated y for current fit
            std::transform(x.begin(), x.end(), std::back_inserter(y_c),
                           [&coefs](const auto& xi){return applyPolynomial(coefs, xi);});

            // Compute bisquare weights
            calc_weights = computeBisquareWeights(x, y,  y_c);

            // Redo fit with new weights and check if they converged
            prev_coefs = std::move(coefs);
            coefs = polynomialFit(x, y, degree - 1, calc_weights);

            int j = 0;
            converged = true;
            while(j < coefs.size() && converged )
            {
                converged &= std::abs(coefs[j] - prev_coefs[j]) < threshold;
                j++;
            }

            i++;
        } while (i < limit && !converged);
    }

    // Return the coefs
    return coefs;
}

template <typename T, typename Ret>
std::vector<Ret> detrend(const std::vector<T>& x, const std::vector<T>& y, unsigned int degree)
{
    // Return vector.
    std::vector<Ret> ret;

    // Calculate the polynomial fit.
    auto coefs = polynomialFit(x, y, degree);

    // Apply the polynomial fit.
    for (std::size_t i = 0; i < x.size(); i++)
    {
        auto result = applyPolynomial(coefs, x[i]);
        ret.push_back(y[i] - result);
    }

    // Return the data with the fit removed.
    return ret;
}

template <typename T, typename Ret>
std::vector<Ret> detrend(const std::vector<T>& x, const std::vector<T>& y,
                         const std::vector<T>& xinterp, const std::vector<T>& yinterp, unsigned int degree)
{
    // Return vector.
    std::vector<Ret> ret;

    // Calculate the polynomial fit.
    auto coefs = polynomialFit(xinterp, yinterp, degree);

    // Apply the polynomial fit.
    for (std::size_t i = 0; i < x.size(); i++)
    {
        auto result = applyPolynomial(coefs, x[i]);
        ret.push_back(y[i] - result);
    }

    // Return the data with the fit removed.
    return ret;
}

}} // END NAMESPACES.
// =====================================================================================================================
