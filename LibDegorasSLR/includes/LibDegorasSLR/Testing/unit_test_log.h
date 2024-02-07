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
#include <vector>
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdpslr_global.h"
#include "LibDegorasSLR/Timing/time_utils.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace testing{
// =====================================================================================================================

// ALIAS
// ---------------------------------------------------------------------------------------------------------------------
using UnitTestResult = std::vector<std::tuple<unsigned, bool, std::string>>;
// ---------------------------------------------------------------------------------------------------------------------

struct LIBDPSLR_EXPORT UnitTestLog
{

public:

    UnitTestLog(const std::string& module,
                const std::string& submodule,
                const std::string& test,
                const std::string &det_ex,
                bool passed,
                const timing::HRTimePointStd &tp,
                long long elapsed,
                const UnitTestResult& results);

    std::string makeLog(bool force_show = false) const;

    const std::string& getModuleName() const;

    const std::string& getSubmoduleName() const;


    bool getResult() const;

private:

    std::string formatResult() const;

    // Stringstreams.
    std::string module_;
    std::string submodule_;
    std::string test_;
    std::string tp_str_;
    std::string det_ex_;
    bool passed_;
    long long elapsed_;
    UnitTestResult results_;
};

}} // END NAMESPACES.
// =====================================================================================================================
