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

#include <vector>

// ========== DPSLR NAMESPACE ==========================================================================================
namespace dpslr
{
// ========== MATH_OPERATORS_PRIVATE NAMESPACE =========================================================================
namespace math_operators_private
{

template <typename T, typename U>
std::vector<std::common_type_t<T,U>> operator +(const std::vector<T>& a, const std::vector<U>& b)
{
    size_t min_size = std::min(a.size(), b.size());
    std::vector<std::common_type_t<T,U>> result;
    for (size_t i = 0; i < min_size; i++)
        result.push_back(a[i] + b[i]);
    return result;
}

template <typename T>
std::vector<T> operator -(const std::vector<T>& a)
{
    std::vector<T> result;
    for (const auto& element : a)
        result.push_back(-element);
    return result;
}

template <typename T, typename U>
std::vector<std::common_type_t<T,U>> operator -(const std::vector<T>& a, const std::vector<U>& b) {return a + (-b);}

} // END NAMESPACE MATH_OPERATORS_PRIVATE
// =====================================================================================================================

} // END NAMESPACE DPSLR
// =====================================================================================================================
