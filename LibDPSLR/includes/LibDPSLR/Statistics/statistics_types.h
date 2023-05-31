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

// ========== DPSLR NAMESPACES =========================================================================================
namespace dpslr{
namespace stats{


/// Histcount result for each bin: {count, left limit, right limit}
template <typename C> using HistCountBin = std::tuple<unsigned, typename C::value_type, typename C::value_type>;

/// Histcount result for all data: a vector with HistCountBin tuples for each bin
template <typename C> using HistCountRes = std::vector<HistCountBin<C>>;



// =====================================================================================================================

// ========== STRUCTS =================================================================================================

// ====================================================================================================================

}
}
