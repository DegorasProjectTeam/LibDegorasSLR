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
#include <iostream>
#include <vector>
#include <functional>
#include <sstream>
#include <type_traits>
#include <utility>
#include <chrono>
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Helpers/string_helpers.h"
#include "LibDegorasSLR/Helpers/types/numeric_strong_type.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace testing{
// =====================================================================================================================

class LIBDPSLR_EXPORT UnitTestBase
{

protected:

    UnitTestBase(const std::string& name);

public:

    // Run all test.
    virtual void runTest();

    // Virtual destructor.
    virtual ~UnitTestBase();

    // Force stream (show) the input test data.
    void setForceStreamData(bool enable);

    // Force fail the test.
    bool forceFail();

    // Force pass the test.
    bool forcePass();

    bool expectTrue(bool result);

    bool expectFalse(bool result);

    bool expectEQ(const std::string& str1, const std::string& str2);

    bool expectEQ(const char* str1, const char* str2);

    // Custom checks.
    template<typename... Args>
    bool customCheck(const std::function<bool(const Args&...)>& check_function, const Args&... args)
    {
        std::cout<<"                           ";
        std::cout<<"- Customized function evaluation"<<std::endl;
        bool result = check_function(args...);
        this->updateCheckResults(result, args...);
        return result;
    }

    // Integral types equality (non-strong).
    template<typename T>
    typename std::enable_if_t<
        !helpers::types::is_container<T>::value &&
        !helpers::types::is_numeric_strong_type<T>::value &&
        !std::is_floating_point_v<T>,
        bool>
    expectEQ(const T& arg1, const T& arg2)
    {
        std::cout<<"                           ";
        std::cout<<"- Comparing integrals equality (non-strong type)"<<std::endl;
        bool result = (arg1 == arg2);
        this->updateCheckResults(result, arg1, arg2);
        return result;
    }

    // Integral types equality (strong).
    template<typename T>
    typename std::enable_if_t<
        !helpers::types::is_container<T>::value &&
        helpers::types::is_numeric_strong_type<T>::value &&
        helpers::types::is_strong_integral<T>::value,
        bool>
    expectEQ(const T& arg1, const T& arg2)
    {
        std::cout<<"                           ";
        std::cout<<"- Comparing integrals equality (strong type)"<<std::endl;
        bool result = (arg1 == arg2);
        this->updateCheckResults(result, arg1, arg2);
        return result;
    }

    // Integral types inequality (non-strong).
    template<typename T>
    typename std::enable_if_t<
        !helpers::types::is_container<T>::value &&
        !helpers::types::is_numeric_strong_type<T>::value &&
        !std::is_floating_point_v<T>,
        bool>
    expectNE(const T& arg1, const T& arg2)
    {
        std::cout<<"                           ";
        std::cout<<"- Comparing integrals inequality (non-strong type)"<<std::endl;
        bool result = (arg1 != arg2);
        this->updateCheckResults(result, arg1, arg2);
        return result;
    }

    // Integral types inequality (strong).
    template<typename T>
    typename std::enable_if_t<
        !helpers::types::is_container<T>::value &&
        helpers::types::is_numeric_strong_type<T>::value &&
        helpers::types::is_strong_integral<T>::value,
        bool>
    expectNE(const T& arg1, const T& arg2)
    {
        std::cout<<"                           ";
        std::cout<<"- Comparing integrals inequality (strong type)"<<std::endl;
        bool result = (arg1 != arg2);
        this->updateCheckResults(result, arg1, arg2);
        return result;
    }

    // Floating types equality (non-strong).
    template<typename T>
    typename std::enable_if_t<
        !helpers::types::is_container<T>::value &&
        !helpers::types::is_numeric_strong_type<T>::value &&
        std::is_floating_point_v<T>,
        bool>
    expectEQ(const T& arg1, const T& arg2, const T& tolerance = std::numeric_limits<T>::epsilon())
    {
        std::cout<<"                           ";
        std::cout <<"- Comparing floats equality (non-strong type)" << std::endl;
        bool result = std::abs(arg1 - arg2) <= tolerance;
        this->updateCheckResults(result, arg1, arg2);
        return result;
    }

    // Floating types equality (strong type).
    template<typename T>
    typename std::enable_if_t<
        !helpers::types::is_container<T>::value &&
        helpers::types::is_numeric_strong_type<T>::value &&
        helpers::types::is_strong_float<T>::value,
        bool>
    expectEQ(const T& arg1,
             const T& arg2,
             const helpers::types::underlying_type_t<T>& tolerance =
                std::numeric_limits<helpers::types::underlying_type_t<T>>::epsilon())
    {
        std::cout<<"                           ";
        std::cout <<"- Comparing floats equality (strong type)" << std::endl;
        bool result = std::abs(arg1 - arg2) <= tolerance;
        this->updateCheckResults(result, arg1, arg2);
        return result;
    }

    // Floating types inequality (non-strong).
    template<typename T>
    typename std::enable_if_t<
        !helpers::types::is_container<T>::value &&
        !helpers::types::is_numeric_strong_type<T>::value &&
        std::is_floating_point_v<T>,
        bool>
    expectNE(const T& arg1, const T& arg2, const T& tolerance = std::numeric_limits<T>::epsilon())
    {
        std::cout<<"                           ";
        std::cout <<"- Comparing floats inequality (non-strong type)" << std::endl;
        bool result = std::abs(arg1 - arg2) > tolerance;
        this->updateCheckResults(result, arg1, arg2);
        return std::abs(arg1 - arg2) > tolerance;
    }

    // Floating types inequality (strong).
    template<typename T>
    typename std::enable_if_t<
        !helpers::types::is_container<T>::value &&
        helpers::types::is_numeric_strong_type<T>::value &&
        helpers::types::is_strong_float<T>::value,
        bool>
    expectNE(const T& arg1,
             const T& arg2,
             const helpers::types::underlying_type_t<T>& tolerance =
                std::numeric_limits<helpers::types::underlying_type_t<T>>::epsilon())
    {
        std::cout<<"                           ";
        std::cout <<"- Comparing floats inequality (strong type)" << std::endl;
        bool result = std::abs(arg1 - arg2) > tolerance;
        this->updateCheckResults(result, arg1, arg2);
        return std::abs(arg1 - arg2) > tolerance;
    }

    // TODO UPDATE THE NEXT CASES TO USE STRONG TYPES.

    template<typename T>
    typename std::enable_if_t<
        !std::is_floating_point_v<T>, bool>
    expectEQ(const std::vector<T>& v1, const std::vector<T>& v2)
    {
        if (v1.size() != v2.size())
        {
            this->updateCheckResults(false);
            return false;
        }

        for (size_t i = 0; i < v1.size(); ++i)
        {
            if (v1[i] != v2[i])
            {
                this->updateCheckResults(false);
                return false;
            }
        }
        this->updateCheckResults(true, v1, v2);
        return true;
    }

    template<typename T>
    typename std::enable_if_t<
        std::is_floating_point_v<T>, bool>
    expectEQ(const std::vector<T>& v1, const std::vector<T>& v2,  const T& tol = std::numeric_limits<T>::epsilon())
    {
        if (v1.size() != v2.size())
        {
            this->updateCheckResults(false);
            return false;
        }

        for (size_t i = 0; i < v1.size(); ++i)
        {
            if (std::abs(v1[i] - v2[i]) > tol)
            {
                this->updateCheckResults(false, v1, v2);
                return false;
            }
        }
        this->updateCheckResults(true);
        return true;
    }

    template <typename T, size_t N>
    typename std::enable_if_t<!std::is_floating_point_v<T>, bool>
    expectEQ(const std::array<T, N>& arr1, const std::array<T, N>& arr2)
    {
        for (size_t i = 0; i < N; ++i)
        {
            if (arr1[i] != arr2[i])
            {
                this->updateCheckResults(false, arr1, arr2);
                return false;
            }
        }
        this->updateCheckResults(true);
        return true;
    }

    template <typename T, size_t N>
    typename std::enable_if_t<std::is_floating_point_v<T>, bool>
    expectEQ(const std::array<T, N>& arr1, const std::array<T, N>& arr2,
             const T& tol = std::numeric_limits<T>::epsilon())
    {
        for (size_t i = 0; i < N; ++i)
        {
            if (std::abs(arr1[i] - arr2[i]) > tol)
            {
                this->updateCheckResults(false, arr1, arr2);
                return false;
            }
        }
        this->updateCheckResults(true);
        return true;
    }

    // Public members.
    std::string test_name_;
    bool result_;
    bool force_stream_data_;
    unsigned current_check_n_;
    std::vector<std::tuple<unsigned, bool, std::string>> check_results_;

private:


    // Conversion to string for streamable types
    template<typename T>
    static std::enable_if_t<
        helpers::types::is_streamable<T>::value &&
        (helpers::types::is_numeric_strong_type<T>::value ?
             helpers::types::is_strong_integral<T>::value :
             !std::is_floating_point_v<T>),
        std::string>
    valueToString(const T& value)
    {
        std::ostringstream os;
        os << value;
        return os.str();
    }

    // Fallback for non-streamable types, could be adjusted based on needs.
    template<typename T>
    static std::enable_if_t<!helpers::types::is_streamable<T>::value, std::string>
    valueToString(const T&)
    {
        return "<NON STREAMABLE TYPE>";
    }

    // Specialization for floating-point types using numberToMaxDecStr
    template<typename T>
    typename std::enable_if_t<
        !helpers::types::is_container<T>::value &&
            (helpers::types::is_numeric_strong_type<T>::value ?
                 helpers::types::is_strong_float<T>::value :
                 std::is_floating_point_v<T>),
        std::string>
    valueToString(const T& value)
    {
        return helpers::strings::numberToMaxDecStr(value);
    }

    // Specialization for std::chrono::time_point types
    template<typename Clock, typename Duration>
    std::string valueToString(const std::chrono::time_point<Clock, Duration>& tp)
    {
        return valueToString(tp.time_since_epoch());
    }

    // Specialization for std::chrono::duration types
    template<typename Rep, typename Period>
    std::string valueToString(const std::chrono::duration<Rep, Period>& dur)
    {
        return std::to_string(dur.count()) + " [" + std::to_string(Period::num) +
               "/" + std::to_string(Period::den) + "]s";
    }

    template<typename... Args>
    void updateCheckResults(bool res, Args&&... args)
    {
        std::string combined_msg;
        this->current_check_n_++;

        if(!res || this->force_stream_data_)
        {
            if constexpr (sizeof...(args) > 0)
            {
                std::ostringstream os;
                (..., (os << " | " << valueToString(std::forward<Args>(args))));
                combined_msg = os.str();
            }
        }

        // Store the results.
        check_results_.emplace_back(current_check_n_, res, combined_msg);
    }
};

}} // END NAMESPACES.
// =====================================================================================================================
