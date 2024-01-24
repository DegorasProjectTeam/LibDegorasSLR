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
 * @file dpslr_filters.h
 * @author Degoras Project Team.
 * @brief This file contains the DPSLR project filters.
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
// =====================================================================================================================

/*
 * @brief Determine what residuals are within a given prefilter window.
 *
 * Determine what residuals are within a given prefilter window. The window is generated using the upper and lower
 * limits as input parameters. All the input data must be coherent. If the residuals are given as distance, the
 * limits should be given as distance, and if the residuals are given as time, the limits should be given as time.
 * The generated acceptance interval will be closed at both ends. The return is a vector with the indexes of the
 * accepted residuals and it will be empty if there is a problem with the inputs.
 *
 * @param[in] resids, vector with the residuals that will be analyzed.
 * @param[in]  upper, the upper limit of the prefilter window.
 * @param[in]  lower, the lower limit of the prefilter window.
 * @return A vector with the indexes of the accepted residuals. It will be empty if there is an error.
 */
LIBDPSLR_EXPORT
    std::vector<std::size_t> windowPrefilter(const std::vector<long double> &resids, long double upper, long double lower);

LIBDPSLR_EXPORT
    std::vector<std::size_t> windowPrefilter(const std::vector<double> &resids, double upper, double lower);

LIBDPSLR_EXPORT
    std::vector<std::size_t> histPrefilterSLR(const std::vector<double> &times, const std::vector<double> &resids,
                     double bs, double depth, unsigned min_ph, unsigned divisions);

LIBDPSLR_EXPORT
    std::vector<std::size_t> histPrefilterBinSLR(const std::vector<double>& resids_bin, double depth, unsigned min_ph);

LIBDPSLR_EXPORT
    std::vector<std::size_t> histPostfilterSLR(const std::vector<double> &times, const std::vector<double> &resids,
                      double bs, double depth);
