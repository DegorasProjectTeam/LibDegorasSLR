# **********************************************************************************************************************
# Updated 23/01/2024
# **********************************************************************************************************************

# **********************************************************************************************************************
# LIBDEGORASSLR TESTS CMAKELIST
# **********************************************************************************************************************

# ----------------------------------------------------------------------------------------------------------------------
# CONFIGURATION

# Set the installation example path.
set(GLOBAL_LIBDPSLR_TESTS_INSTALL_PATH ${MODULES_GLOBAL_INSTALL_BIN_PATH}/testing)

# Ignore paths for basic unit test:
set(IGNORED_PATHS
    "AlgorithmsSLR/UnitTest_PredictorSLR/*"
    "AlgorithmsSLR/ComplexTest_GeneratePasses/*")

# Add all the basic unit tests.
macro_setup_lib_basic_unit_tests("${CMAKE_CURRENT_SOURCE_DIR}"
                                 "${GLOBAL_LIBDPSLR_TESTS_INSTALL_PATH}"
                                 "${IGNORED_PATHS}")

# ----------------------------------------------------------------------------------------------------------------------
# COMPLEX TESTS

# Log.
message(STATUS "Setup library complex tests...")

# Ignore paths for complex unit test:
set(IGNORED_PATHS
    "AlgorithmsSLR/UnitTest_PredictorSLR/*")

# Add the tests in the subdirectories.
macro_add_subdirs_recursive("${CMAKE_CURRENT_SOURCE_DIR}" "${IGNORED_PATHS}")

# **********************************************************************************************************************
