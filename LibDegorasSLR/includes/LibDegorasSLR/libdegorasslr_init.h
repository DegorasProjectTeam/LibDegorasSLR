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

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <atomic>
#include <omp.h>
#include <stdexcept>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
// =====================================================================================================================

static long double kFloatingCompEpsilon = 0.000000001L;

class LIBDPSLR_EXPORT DegorasInit
{

public:

    static void setOMPNumThreads(int n = 0)
    {
        int n_th = n == 0 ? omp_get_max_threads() : n;
        omp_set_num_threads(n_th);
    }

    static void setFloatingComparationEpsilon(long double epsilon = 0.000000001L)
    {
        kFloatingCompEpsilon = epsilon;
    }

    static void init()
    {
        DegorasInit::setOMPNumThreads();
        DegorasInit::initialized = true;
    }

    static void checkMandatoryInit()
    {
        if(!DegorasInit::initialized)
            throw std::runtime_error("[LibDegorasSLR,DegorasInit] LibDegorasSLR not initialized.");
    }

    inline static std::atomic<bool> initialized = false;
};








} // END NAMESPACES
// =====================================================================================================================
