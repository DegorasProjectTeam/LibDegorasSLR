/***********************************************************************************************************************
 *   LibDegorasSLR (Degoras Project SLR Library).                                                                      *
 *                                                                                                                     *
 *   A modern and efficient C++ base library for Satellite Laser Ranging (SLR) software and real-time hardware         *
 *   related developments. Developed as a free software under the context of Degoras Project for the Spanish Navy      *
 *   Observatory SLR station (SFEL) in San Fernando and, of course, for any other station that wants to use it!        *
 *                                                                                                                     *
 *   Copyright (C) 2024 Degoras Project Team                                                                           *
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >                             *
 *                      < Jesús Relinque Madroñal >                                                                    *
 *                                                                                                                     *
 *   This file is part of LibDegorasSLR.                                                                               *
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

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Testing/unit_test/unit_test_base.h"
// =====================================================================================================================


// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace testing{
// =====================================================================================================================


UnitTestBase::UnitTestBase(const std::string &name):
    test_name_(name),
    result_(true),
    force_stream_data_(false),
    current_check_n_(0)
{}

void UnitTestBase::runTest(){}

bool UnitTestBase::forceFail()
{
    this->updateCheckResults(false);
    return false;
}

bool UnitTestBase::forcePass()
{
    this->updateCheckResults(true);
    return false;
}

bool UnitTestBase::expectTrue(bool result)
{
    std::cout<<"                           - Expecting true result"<<std::endl;
    this->updateCheckResults(result);
    return result;
}

bool UnitTestBase::expectFalse(bool result)
{
    std::cout<<"                           - Expecting false result"<<std::endl;
    this->updateCheckResults(!result);
    return !result;
}

bool UnitTestBase::expectEQ(const std::string &str1, const std::string &str2)
{
    bool result = (str1 == str2);
    this->updateCheckResults(result, str1, str2);
    return result;
}

bool UnitTestBase::expectEQ(const char *str1, const char *str2)
{
    bool result = (std::string(str1) == std::string(str2));
    this->updateCheckResults(result, str1, str2);
    return result;
}

UnitTestBase::~UnitTestBase(){}

void UnitTestBase::setForceStreamData(bool enable)
{
    this->force_stream_data_ = enable;
}

}} // END NAMESPACES.
// =====================================================================================================================
