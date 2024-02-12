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

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Testing/unit_test.h"
// =====================================================================================================================

// MACROS
// =====================================================================================================================

#define M_START_UNIT_TEST_SESSION(SessionName)                    \
int main(){                                                       \
UnitTest::instance().clear();                                     \
UnitTest::instance().setSessionName(std::string(SessionName));    \

#define M_FORCE_SHOW_RESULTS(enable)                              \
UnitTest::instance().clear();                                     \
    UnitTest::instance().setForceShowResults(enable);             \

#define M_DECLARE_UNIT_TEST(TestName)                       \
using dpslr::testing::UnitTestBase;                         \
using dpslr::testing::UnitTest;                             \
class Test_##TestName : public UnitTestBase                 \
{                                                           \
        Test_##TestName(): UnitTestBase(#TestName){}        \
        public:                                             \
        static Test_##TestName* instance()                  \
    {                                                       \
            static Test_##TestName test;                    \
            return &test;                                   \
    }                                                       \
        void runTest() override;                            \
};                                                          \

#define M_DEFINE_UNIT_TEST(TestName) \
void Test_##TestName::runTest() \

#define M_REGISTER_UNIT_TEST(Module, Submodule, TestName) \
    UnitTest::instance().registerTest(#Module, #Submodule, Test_##TestName::instance()); \

#define M_RUN_UNIT_TESTS() \
bool final_res = dpslr::testing::UnitTest::instance().runTests(); \

#define M_FINISH_UNIT_TEST_SESSION() \
return (final_res ? 0 : 1); } \

#define M_CUSTOM_CHECK(func, ...) \
this->result_ &= customCheck(func, __VA_ARGS__); \

#define M_EXPECTED_TRUE(arg1) \
this->result_ &= expectTrue(arg1); \

#define M_EXPECTED_FALSE(arg1) \
this->result_ &= expectFalse(arg1); \

#define M_EXPECTED_EQ(arg1, arg2) \
this->result_ &= expectEQ(arg1, arg2); \

#define M_EXPECTED_EQ_F(arg1, arg2, eps) \
this->result_ &= expectEQ(arg1, arg2, eps); \

#define M_EXPECTED_NE_F(arg1, arg2, eps) \
this->result_ &= expectNE(arg1, arg2, eps); \

#define M_EXPECTED_EQ(arg1, arg2) \
this->result_ &= expectEQ(arg1, arg2); \

#define M_EXPECTED_NE(arg1, arg2) \
this->result_ &= expectNE(arg1, arg2); \

#define M_FORCE_FAIL() \
this->result_ &= forceFail(); \

#define M_FORCE_PASS() \
this->result_ &= forcePass(); \

#define M_SLEEP_US(arg1) \
std::this_thread::sleep_for(std::chrono::microseconds(arg1)); \
 \
// =====================================================================================================================
