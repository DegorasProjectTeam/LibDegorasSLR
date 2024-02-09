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

#define __USE_MINGW_ANSI_STDIO 1

// C++ INCLUDES
// =====================================================================================================================
#include <iostream>
#include <regex>
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include <LibDegorasSLR/Testing/UnitTest>
#include <LibDegorasSLR/Helpers/Traits>
// =====================================================================================================================

// NAMESPACES
// ---------------------------------------------------------------------------------------------------------------------
using namespace dpslr;
using namespace dpslr::helpers::types;
// ---------------------------------------------------------------------------------------------------------------------

// UNIT TEST DECLARATIONS
// ---------------------------------------------------------------------------------------------------------------------
M_DECLARE_UNIT_TEST(NumericStrongType_traits)
M_DECLARE_UNIT_TEST(NumericStrongType_basics)

// ---------------------------------------------------------------------------------------------------------------------

// UNIT TESTS IMPLEMENTATIONS
// ---------------------------------------------------------------------------------------------------------------------

M_DEFINE_UNIT_TEST(NumericStrongType_traits)
{
    // Fail not allowed assertion example.
    //using NotAllowed = NumericStrongType<std::string, struct CustomDoubleTag>;
    //NotAllowed not_allowed;

    // Create a strong type alias.
    using CustomUnsigned = NumericStrongType<unsigned, struct CustomUnsignedTag>;
    using CustomInt = NumericStrongType<int, struct CustomIntTag>;
    using CustomLong = NumericStrongType<long, struct CustomLongTag>;
    using CustomLongLong = NumericStrongType<long long, struct CustomLongLongTag>;
    using CustomFloat = NumericStrongType<float, struct CustomFloatTag>;
    using CustomDouble = NumericStrongType<double, struct CustomDoubleTag>;
    using CustomLongDouble = NumericStrongType<long double, struct CustomDoubleTag>;

    // Static asserts testing.
    static_assert(is_strong_integral<CustomUnsigned>::value, "CustomUnsigned should be integral.");
    static_assert(is_strong_integral<CustomInt>::value, "CustomInt should be integral");
    static_assert(is_strong_integral<CustomLong>::value, "CustomLong should be integral");
    static_assert(is_strong_integral<CustomLongLong>::value, "CustomLongLong should be integral");
    static_assert(is_strong_floating<CustomFloat>::value, "CustomFloat should be floating");
    static_assert(is_strong_floating<CustomDouble>::value, "CustomDouble should be floating");
    static_assert(is_strong_floating<CustomLongDouble>::value, "CustomLongDouble should be floating");

    // Dynamic checks.
    M_EXPECTED_TRUE(is_strong_integral<CustomUnsigned>::value)
    M_EXPECTED_TRUE(is_strong_integral<CustomInt>::value)
    M_EXPECTED_TRUE(is_strong_integral<CustomLong>::value)
    M_EXPECTED_TRUE(is_strong_integral<CustomLongLong>::value)
    M_EXPECTED_TRUE(is_strong_floating<CustomFloat>::value)
    M_EXPECTED_TRUE(is_strong_floating<CustomDouble>::value)
    M_EXPECTED_TRUE(is_strong_floating<CustomLongDouble>::value)
    M_EXPECTED_FALSE(is_strong_floating<CustomUnsigned>::value)
    M_EXPECTED_FALSE(is_strong_floating<CustomInt>::value)
    M_EXPECTED_FALSE(is_strong_floating<CustomLong>::value)
    M_EXPECTED_FALSE(is_strong_floating<CustomLongLong>::value)
    M_EXPECTED_FALSE(is_strong_integral<CustomFloat>::value)
    M_EXPECTED_FALSE(is_strong_integral<CustomDouble>::value)
    M_EXPECTED_FALSE(is_strong_integral<CustomLongDouble>::value)

    // Custom assigns.
    CustomUnsigned custom_unsigned = 1234;
    CustomInt custom_int = -1234;
    CustomLong custom_long = 123456789;
    CustomLongLong custom_longlong = 12345678987654321LL;
    CustomFloat custom_float = 65.8F;
    CustomDouble custom_double = 16.123456789;
    CustomLongDouble custom_longdouble = 1.2345678987654321L;

    // Dynamic checks.
    M_EXPECTED_TRUE(is_strong_integral<decltype(custom_unsigned)>::value)
    M_EXPECTED_TRUE(is_strong_integral<decltype(custom_int)>::value)
    M_EXPECTED_TRUE(is_strong_integral<decltype(custom_long)>::value)
    M_EXPECTED_TRUE(is_strong_integral<decltype(custom_longlong)>::value)
    M_EXPECTED_TRUE(is_strong_floating<decltype(custom_float)>::value)
    M_EXPECTED_TRUE(is_strong_floating<decltype(custom_double)>::value)
    M_EXPECTED_TRUE(is_strong_floating<decltype(custom_longdouble)>::value)
    M_EXPECTED_FALSE(is_strong_floating<decltype(custom_unsigned)>::value)
    M_EXPECTED_FALSE(is_strong_floating<decltype(custom_int)>::value)
    M_EXPECTED_FALSE(is_strong_floating<decltype(custom_long)>::value)
    M_EXPECTED_FALSE(is_strong_floating<decltype(custom_longlong)>::value)
    M_EXPECTED_FALSE(is_strong_integral<decltype(custom_float)>::value)
    M_EXPECTED_FALSE(is_strong_integral<decltype(custom_double)>::value)
    M_EXPECTED_FALSE(is_strong_integral<decltype(custom_longdouble)>::value)

    // Check numeric limits for long double (MinGW use extended 80-bit for long double).
    M_EXPECTED_TRUE(std::numeric_limits<decltype(custom_longdouble)>::digits10 >= 18)

    // Unncomment if you want to see all the limits.
    /*
    std::cout<<std::numeric_limits<decltype(custom_unsigned)>::digits10<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_unsigned)>::max()<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_unsigned)>::min()<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_unsigned)>::epsilon()<<std::endl;
    //
    std::cout<<std::numeric_limits<decltype(custom_int)>::digits10<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_int)>::max()<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_int)>::min()<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_int)>::epsilon()<<std::endl;
    //
    std::cout<<std::numeric_limits<decltype(custom_long)>::digits10<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_long)>::max()<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_long)>::min()<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_long)>::epsilon()<<std::endl;
    //
    std::cout<<std::numeric_limits<decltype(custom_longlong)>::digits10<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_longlong)>::max()<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_longlong)>::min()<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_longlong)>::epsilon()<<std::endl;
    //
    std::cout<<std::numeric_limits<decltype(custom_float)>::digits10<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_float)>::max()<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_float)>::min()<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_float)>::epsilon()<<std::endl;
    //
    std::cout<<std::numeric_limits<decltype(custom_double)>::digits10<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_double)>::max()<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_double)>::min()<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_double)>::epsilon()<<std::endl;
    //
    std::cout<<std::numeric_limits<decltype(custom_longdouble)>::digits10<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_longdouble)>::max()<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_longdouble)>::min()<<std::endl;
    std::cout<<std::numeric_limits<decltype(custom_longdouble)>::epsilon()<<std::endl;
    */
}


M_DEFINE_UNIT_TEST(NumericStrongType_basics)
{
    // Create a strong type alias.
    using CustomUnsigned = NumericStrongType<unsigned, struct CustomUnsignedTag>;
    using CustomInt = NumericStrongType<int, struct CustomIntTag>;
    using CustomLong = NumericStrongType<long, struct CustomLongTag>;
    using CustomLongLong = NumericStrongType<long long, struct CustomLongLongTag>;
    using CustomFloat = NumericStrongType<float, struct CustomFloatTag>;
    using CustomDouble = NumericStrongType<double, struct CustomDoubleTag>;
    using CustomLongDouble = NumericStrongType<long double, struct CustomDoubleTag>;

    // Custom unsigned assign and checks.
    CustomUnsigned custom_unsigned_1 = 1234;
    CustomUnsigned custom_unsigned_2 = 1234;
    CustomUnsigned custom_unsigned_3 = 4321;


    M_EXPECTED_EQ(custom_unsigned_1, custom_unsigned_2)
    M_EXPECTED_NE(custom_unsigned_1, custom_unsigned_3)



    // Custom double assign.
    CustomDouble custom_double_1 = 65.8;
    CustomDouble custom_double_2 = 65.10;


   // M_EXPECTED_EQ(custom_double_1, custom_double_2)
}

// ---------------------------------------------------------------------------------------------------------------------

// UNIT TESTS EXECUTION
// ---------------------------------------------------------------------------------------------------------------------

// Start the Unit Test Session.
M_START_UNIT_TEST_SESSION("LibDegorasSLR Helpers Session")

// Configuration.
M_FORCE_SHOW_RESULTS(false)

// Register the tests.
M_REGISTER_UNIT_TEST(Helpers-types, NumericStrongType, NumericStrongType_traits)
M_REGISTER_UNIT_TEST(Helpers-types, NumericStrongType, NumericStrongType_basics)

// Run unit tests.
M_RUN_UNIT_TESTS()

// Finish the session.
M_FINISH_UNIT_TEST_SESSION()

// ---------------------------------------------------------------------------------------------------------------------

// =====================================================================================================================
