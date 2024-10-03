# **********************************************************************************************************************
# LIBDEGORASSLR TESTS CMAKELIST
# **********************************************************************************************************************

# ----------------------------------------------------------------------------------------------------------------------
# CONFIGURATION

# Set the installation example path.
set(GLOBAL_LIBDPSLR_TESTS_INSTALL_PATH ${MODULES_GLOBAL_INSTALL_BIN_PATH}/testing)

# Ignore paths for basic unit test:
set(IGNORED_PATHS
    "AlgorithmsSLR/UnitTest_PredictorSLR/*")

# ----------------------------------------------------------------------------------------------------------------------
# ADD THE MODULES (SIMPLE AND COMPLEX TESTS)

# Add all the Modules.
add_subdirectory(Helpers)
add_subdirectory(Timing)
add_subdirectory(Astronomical)
add_subdirectory(TrackingMount)


# Add the examples in the subdirectories.
macro_add_subdirs_recursive("${CMAKE_CURRENT_SOURCE_DIR}/AlgorithmsSLR" "${IGNORED_PATHS}")

# **********************************************************************************************************************
