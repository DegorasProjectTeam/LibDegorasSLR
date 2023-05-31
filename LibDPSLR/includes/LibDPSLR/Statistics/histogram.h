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
 * @file histogram.h
 * @author Degoras Project Team.
 * @brief This file contains the declaration of several functions related with histogram calculations.
 * @copyright EUPL License
 * @version 2305.1
 * @todo Add a high precission histogram class and related if neccesary.
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include <string>
#include <vector>
#include <algorithm>
#include <omp.h>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDPSLR/Statistics/statistics_types.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace stats{
// =====================================================================================================================

template <typename C>
HistCountRes<C> histcounts1D(const C& data)
{
    // TODO. AUTOMATIC BINDING ALGORITHM (SCOTT).
}

template <typename C>
HistCountRes<C> histcounts1D(const C& data, size_t nbins, typename C::value_type min_edge,
                             typename C::value_type max_edge)
{
    // Convenient alias.
    using ConType = typename C::value_type;

    // Return container.
    std::vector<std::tuple<unsigned, ConType, ConType>> result(nbins);

    // Get the division.
    ConType div = (max_edge - min_edge) / nbins;

    // Parallel loop for each bin.
    omp_set_num_threads(omp_get_num_procs());
    #pragma omp parallel for
    for (size_t i = 0; i < nbins; i++ )
    {
        // Update the next counter.
        ConType min = min_edge + i * div;
        ConType max = min + div;
        // Count the data in the bin.
        unsigned counter = countBin(data, min,  max);
        // Push the new data in the result vector, and update the min counter.
        result[i] = {counter, min, max};
    }

    // Return the result.
    return result;
}

template <typename C>
HistCountRes<C> histcounts1D(const C& data, size_t nbins)
{
    // Convenient alias.
    using ConType = typename C::value_type;

    // Return container.
    std::vector<std::tuple<unsigned, ConType, ConType>> result(nbins);

    // Get the minimum and maximum values.
    auto minmax = std::minmax_element(data.begin(), data.end());
    ConType min_counter = *(minmax.first);
    ConType max_counter = *(minmax.second);

    // Get the division.
    ConType div = (std::abs(max_counter) + std::abs(min_counter)) / nbins;

    // Parallel loop for each bin.
    omp_set_num_threads(omp_get_num_procs());
    #pragma omp parallel for
    for (size_t i = 0; i < nbins; i++ )
    {
        // Update the next counter.
        ConType min = min_counter + i * div;
        ConType max = min + div;
        // Count the data in the bin.
        unsigned counter = countBin(data, min,  max);
        // Push the new data in the result vector, and update the min counter.
        result[i] = {counter, min, max};
    }

    // Return the result.
    return result;
}

/**
 * @brief Custom count bin function.
 *
 * This function counts how many values in the container are in the given interval. The boundaries can be
 * customized as open or closed intervals (in the mathematical sense). The default interval is [min, max).
 *
 * @param[in] container The container with the values.
 * @param[in] min
 * @param[in] max
 * @param[in] ex_min True if you want to exclude the minimum value (open interval).
 * @param[in] ex_max True if you want to exclude the maximum value (open interval).
 * @return The number of elements in the container that are in the given interval.
 * @warning When comparing floating-point values, precision issues may arise due to the inherent limitations
 *          of floating-point representation. Take care when comparing floating-point values.
 */
template <typename Container, typename T>
unsigned countBin(const Container& container, T min,T max, bool exmin = false, bool exmax = true)
{
    // Convenient alias.
    using ConType = typename Container::value_type;

    // Count the values.
    unsigned counter = std::count_if(container.begin(), container.end(),
                                     [&min, &max, &exmin, &exmax](const ConType& i)
                                     {
                                         // TODO Floating comparation?
                                         bool result;
                                         if(exmin && exmax)
                                             result = (i > min) && (i < max);
                                         else if(exmin && !exmax)
                                             result = (i > min) && (i <= max);

                                         else if(!exmin && exmax)
                                             result = (i >= min) && (i < max);
                                         else
                                             result = (i >= min) && (i <= max);
                                         return result;
                                     });
    // Return the result.
    return counter;
}



}} // END NAMESPACES.
// =====================================================================================================================
