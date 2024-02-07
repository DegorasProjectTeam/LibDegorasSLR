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
#include <functional>
#include <sstream>
#include <type_traits>
#include <utility>
#include <chrono>
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdpslr_global.h"
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

    bool forceFail();

    bool forcePass();

    // Type traits.
    template <typename T>
    struct is_container : std::false_type {};

    template <typename... Args>
    struct is_container<std::vector<Args...>> : std::true_type {};

    template <typename T, size_t N>
    struct is_container<std::array<T, N>> : std::true_type {};

    template<typename... Args>
    bool customCheck(const std::function<bool(const Args&...)>& check_function, const Args&... args)
    {
        bool result = check_function(args...);
        this->updateCheckResults(result, args...);
        return result;
    }

    bool expectEQ(const std::string& str1, const std::string& str2)
    {
        bool result = (str1 == str2);
        this->updateCheckResults(result, str1, str2);
        return result;
    }

    bool expectEQ(const char* str1, const char* str2)
    {
        bool result = (std::string(str1) == std::string(str2));
        this->updateCheckResults(result, str1, str2);
        return result;
    }

    template<typename T>
    typename std::enable_if_t<
        !is_container<T>::value &&
            !std::is_floating_point_v<T>, bool>
    expectEQ(const T& arg1, const T& arg2)
    {
        bool result = (arg1 == arg2);
        this->updateCheckResults(result, arg1, arg2);
        return result;
    }

    template<typename T>
    typename std::enable_if_t<
        !is_container<T>::value &&
            std::is_floating_point_v<T>, bool>
    expectEQ(const T& arg1, const T& arg2, const T& tolerance = std::numeric_limits<T>::epsilon())
    {
        bool result = std::abs(arg1 - arg2) <= tolerance;
        this->updateCheckResults(result, arg1, arg2);
        return result;
    }

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
    expectEQ(const std::array<T, N>& arr1, const std::array<T, N>& arr2, const T& tol = std::numeric_limits<T>::epsilon())
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

    template<typename T>
    typename std::enable_if_t<
        !is_container<T>::value &&
            !std::is_floating_point_v<T>, bool>
    expectNE(const T& arg1, const T& arg2)
    {
        bool result = (arg1 != arg2);
        this->updateCheckResults(result, arg1, arg2);
        return result;
    }

    template<typename T>
    typename std::enable_if_t<
        !is_container<T>::value &&
            std::is_floating_point_v<T>, bool>
    expectNE(const T& arg1, const T& arg2, const T& tolerance = std::numeric_limits<T>::epsilon())
    {
        bool result = std::abs(arg1 - arg2) > tolerance;
        this->updateCheckResults(result, arg1, arg2);
        return std::abs(arg1 - arg2) > tolerance;
    }

    virtual void runTest();

    virtual ~UnitTestBase();

    void setForceStreamData(bool enable);

    std::string test_name_;
    bool result_;
    bool force_stream_data_;
    unsigned current_check_n_;
    std::vector<std::tuple<unsigned, bool, std::string>> check_results_;

private:

    // Helper to convert value to string if it supports streaming to an ostringstream.
    template<typename T, typename = void>
    struct is_streamable : std::false_type {};

    template<typename T>
    struct is_streamable<T, std::void_t<decltype(std::declval<std::ostringstream&>()
                                                 << std::declval<T>())>> : std::true_type {};
    template<typename T>
    static std::enable_if_t<is_streamable<T>::value, std::string> valueToString(const T& value)
    {
        std::ostringstream os;
        os << value;
        return os.str();
    }

    // Fallback for non-streamable types, could be adjusted based on needs.
    template<typename T>
    static std::enable_if_t<!is_streamable<T>::value, std::string> valueToString(const T&)
    {
        return "<NON STREAMABLE TYPE>";
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
