

#pragma once

#include <vector>

// ========== DPSLR NAMESPACES =========================================================================================
namespace dpslr{
namespace stats{
namespace types{

/// Histcount result for each bin: {count, left limit, right limit}
template <typename C> using HistCountBin = std::tuple<unsigned, typename C::value_type, typename C::value_type>;

/// Histcount result for all data: a vector with HistCountBin tuples for each bin
template <typename C> using HistCountRes = std::vector<HistCountBin<C>>;

// Enum for represent the possible errors from the lagrange polynomial interpolator.
enum class LagrangeError
{
    NOT_ERROR,
    NOT_IN_THE_MIDDLE,
    X_OUT_OF_BOUNDS,
    DATA_SIZE_MISMATCH
};

enum class PolyFitRobustMethod
{
    NO_ROBUST,
    BISQUARE_WEIGHTS
};


// =====================================================================================================================

// ========== STRUCTS =================================================================================================

// ====================================================================================================================

}}}
