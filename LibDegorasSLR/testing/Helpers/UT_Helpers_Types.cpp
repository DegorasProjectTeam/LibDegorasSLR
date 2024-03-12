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

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include <LibDegorasSLR/Modules/Testing>
#include <LibDegorasSLR/Modules/Mathematics>
#include <LibDegorasSLR/Modules/Helpers>
// =====================================================================================================================

// NAMESPACES
// ---------------------------------------------------------------------------------------------------------------------
using dpslr::math::types::NumericStrongType;
using dpslr::helpers::traits::is_strong_integral_v;
using dpslr::helpers::traits::is_strong_floating_v;
// ---------------------------------------------------------------------------------------------------------------------

// UNIT TEST DECLARATIONS
// ---------------------------------------------------------------------------------------------------------------------
M_DECLARE_UNIT_TEST(NumericStrongType_traits)
M_DECLARE_UNIT_TEST(NumericStrongType_basics)
//

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
    static_assert(is_strong_integral_v<CustomUnsigned>, "CustomUnsigned should be integral.");
    static_assert(is_strong_integral_v<CustomInt>, "CustomInt should be integral");
    static_assert(is_strong_integral_v<CustomLong>, "CustomLong should be integral");
    static_assert(is_strong_integral_v<CustomLongLong>, "CustomLongLong should be integral");
    static_assert(is_strong_floating_v<CustomFloat>, "CustomFloat should be floating");
    static_assert(is_strong_floating_v<CustomDouble>, "CustomDouble should be floating");
    static_assert(is_strong_floating_v<CustomLongDouble>, "CustomLongDouble should be floating");

    // Dynamic checks.
    M_EXPECTED_TRUE(is_strong_integral_v<CustomUnsigned>)
    M_EXPECTED_TRUE(is_strong_integral_v<CustomInt>)
    M_EXPECTED_TRUE(is_strong_integral_v<CustomLong>)
    M_EXPECTED_TRUE(is_strong_integral_v<CustomLongLong>)
    M_EXPECTED_TRUE(is_strong_floating_v<CustomFloat>)
    M_EXPECTED_TRUE(is_strong_floating_v<CustomDouble>)
    M_EXPECTED_TRUE(is_strong_floating_v<CustomLongDouble>)
    M_EXPECTED_FALSE(is_strong_floating_v<CustomUnsigned>)
    M_EXPECTED_FALSE(is_strong_floating_v<CustomInt>)
    M_EXPECTED_FALSE(is_strong_floating_v<CustomLong>)
    M_EXPECTED_FALSE(is_strong_floating_v<CustomLongLong>)
    M_EXPECTED_FALSE(is_strong_integral_v<CustomFloat>)
    M_EXPECTED_FALSE(is_strong_integral_v<CustomDouble>)
    M_EXPECTED_FALSE(is_strong_integral_v<CustomLongDouble>)

    // Custom assigns.
    CustomUnsigned custom_unsigned = 1234;
    CustomInt custom_int = -1234;
    CustomLong custom_long = 123456789;
    CustomLongLong custom_longlong = 12345678987654321LL;
    CustomFloat custom_float = 65.8F;
    CustomDouble custom_double = 16.123456789;
    CustomLongDouble custom_longdouble = 1.2345678987654321L;

    // Dynamic checks.
    M_EXPECTED_TRUE(is_strong_integral_v<decltype(custom_unsigned)>)
    M_EXPECTED_TRUE(is_strong_integral_v<decltype(custom_int)>)
    M_EXPECTED_TRUE(is_strong_integral_v<decltype(custom_long)>)
    M_EXPECTED_TRUE(is_strong_integral_v<decltype(custom_longlong)>)
    M_EXPECTED_TRUE(is_strong_floating_v<decltype(custom_float)>)
    M_EXPECTED_TRUE(is_strong_floating_v<decltype(custom_double)>)
    M_EXPECTED_TRUE(is_strong_floating_v<decltype(custom_longdouble)>)
    M_EXPECTED_FALSE(is_strong_floating_v<decltype(custom_unsigned)>)
    M_EXPECTED_FALSE(is_strong_floating_v<decltype(custom_int)>)
    M_EXPECTED_FALSE(is_strong_floating_v<decltype(custom_long)>)
    M_EXPECTED_FALSE(is_strong_floating_v<decltype(custom_longlong)>)
    M_EXPECTED_FALSE(is_strong_integral_v<decltype(custom_float)>)
    M_EXPECTED_FALSE(is_strong_integral_v<decltype(custom_double)>)
    M_EXPECTED_FALSE(is_strong_integral_v<decltype(custom_longdouble)>)

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
    using CustomLongLong = NumericStrongType<long long, struct CustomLongLongTag>;
    using CustomFloat = NumericStrongType<float, struct CustomFloatTag>;
    using CustomDouble = NumericStrongType<double, struct CustomDoubleTag>;
    using CustomLongDouble = NumericStrongType<long double, struct CustomDoubleTag>;

    // Custom numercis.
    // Integers.
    CustomUnsigned custom_unsigned_1 = 1234;
    CustomUnsigned custom_unsigned_2 = 1234;
    CustomUnsigned custom_unsigned_3 = 4321;
    CustomInt custom_int_1 = -123456789;
    CustomInt custom_int_2 = -123456789;
    CustomInt custom_int_3 = 278917;
    CustomLongLong custom_longlong_1 = -123456789LL;
    CustomLongLong custom_longlong_2 = -123456789LL;
    CustomLongLong custom_longlong_3 = 278917LL;
    // Floats.
    CustomFloat custom_float_1 = -123.123F;
    CustomFloat custom_float_2 = -123.123F;
    CustomFloat custom_float_3 = 0.8781F;
    CustomFloat custom_float_4 = 0.8765F;
    // Other constructors test.
    CustomDouble custom_double_1 = -65.897112;
    CustomDouble custom_double_2 = custom_double_1;
    CustomDouble custom_double_3(123.76213);
    CustomDouble custom_double_4(123.76258);
    CustomLongDouble custom_longdouble_1 = 0.12345678987654321L;
    CustomLongDouble custom_longdouble_2 = custom_longdouble_1;
    CustomLongDouble custom_longdouble_3(-123456.12345678987L);
    CustomLongDouble custom_longdouble_4(-123456.12345678984L);

    // Checks.
    // Integer.
    M_EXPECTED_EQ(custom_unsigned_1, custom_unsigned_2)
    M_EXPECTED_NE(custom_unsigned_1, custom_unsigned_3)
    // --
    M_EXPECTED_EQ(custom_int_1, custom_int_2)
    M_EXPECTED_NE(custom_int_1, custom_int_3)
    // --
    M_EXPECTED_EQ(custom_longlong_1, custom_longlong_2)
    M_EXPECTED_NE(custom_longlong_1, custom_longlong_3)
    // Floats.
    M_EXPECTED_EQ(custom_float_1, custom_float_2)
    M_EXPECTED_NE(custom_float_1, custom_float_3)
    M_EXPECTED_EQ_F(custom_float_3, custom_float_4, 0.01F)
    M_EXPECTED_NE_F(custom_float_3, custom_float_4, 0.001F)
    // --
    M_EXPECTED_EQ(custom_double_1, custom_double_2)
    M_EXPECTED_NE(custom_double_1, custom_double_3)
    M_EXPECTED_EQ_F(custom_double_3, custom_double_4, 0.001)
    M_EXPECTED_NE_F(custom_double_3, custom_double_4, 0.0001)
    //--
    M_EXPECTED_EQ(custom_longdouble_1, custom_longdouble_2)
    M_EXPECTED_NE(custom_longdouble_1, custom_longdouble_3)
    M_EXPECTED_EQ_F(custom_longdouble_3, custom_longdouble_4, 0.0000000001L)
    M_EXPECTED_NE_F(custom_longdouble_3, custom_longdouble_4, 0.00000000001L)
    // --
    // Check that the next must be not allowed:
    //M_EXPECTED_EQ(custom_unsigned_1, 1234)
}

// ---------------------------------------------------------------------------------------------------------------------

// UNIT TESTS EXECUTION
// ---------------------------------------------------------------------------------------------------------------------

// Start the Unit Test Session.
M_START_UNIT_TEST_SESSION("LibDegorasSLR Helpers Session")

// Configuration.
M_FORCE_SHOW_RESULTS(true)

// Register the tests.
M_REGISTER_UNIT_TEST(Helpers-types, NumericStrongType, NumericStrongType_traits)
M_REGISTER_UNIT_TEST(Helpers-types, NumericStrongType, NumericStrongType_basics)

// Run unit tests.
M_RUN_UNIT_TESTS()

// Finish the session.
M_FINISH_UNIT_TEST_SESSION()

// ---------------------------------------------------------------------------------------------------------------------

// =====================================================================================================================
