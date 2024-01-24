/***********************************************************************************************************************
 * Copyright 2023 Degoras Project Team
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they will be approved by the
 * European Commission - subsequent versions of the EUPL (the "Licence");
 *
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the Licence is distributed on
 * an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the Licence for the
 * specific language governing permissions and limitations under the Licence.
 **********************************************************************************************************************/

#pragma once

#include "operators.tpp"

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace math{
namespace common{
// =====================================================================================================================

template <typename T, typename U>
std::vector<std::common_type_t<T,U>> operator +(const std::vector<T>& a, const std::vector<U>& b)
{
    return dpslr::math_operators_private::operator+(a, b);
}

template <typename T>
std::vector<T> operator -(const std::vector<T>& a)
{
    return dpslr::math_operators_private::operator-(a);
}

template <typename T, typename U>
std::vector<std::common_type_t<T,U>> operator -(const std::vector<T>& a, const std::vector<U>& b)
{
    return dpslr::math_operators_private::operator-(a, b);
}

}}} // END NAMESPACES
// =====================================================================================================================
