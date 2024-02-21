# **********************************************************************************************************************
# Updated 16/02/2024
# **********************************************************************************************************************

# **********************************************************************************************************************
# LIBDEGORASSLR EXAMPLE CMAKELIST
# **********************************************************************************************************************

# ----------------------------------------------------------------------------------------------------------------------
# CONFIGURATION

# Config.
set(EXAMPLE_NAME UtilitiesSLR_TrackingSLR)
set(EXAMPLE_DIR ${CMAKE_SOURCE_DIR}/examples/${EXAMPLE_NAME})
set(EXAMPLE_SOURCES ${EXAMPLE_DIR}/UtilitiesSLR_TrackingSLR.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Examples/${EXAMPLE_NAME}")

# Add a custom target for Python scripts to make them visible in IDEs
file(GLOB PYTHON_SCRIPTS "${EXAMPLE_DIR}/python_scripts/*.py")
source_group("PythonScripts" FILES ${PYTHON_SCRIPTS})

# Setup the launcher.
macro_setup_launcher("${EXAMPLE_NAME}"
                     "${MODULES_GLOBAL_LIBS_OPTIMIZED}"
                     "${MODULES_GLOBAL_LIBS_DEBUG}"
                     "${EXAMPLE_SOURCES}" "${PYTHON_SCRIPTS}")

# Add to the build step the necessary input files for the example.
add_custom_command(TARGET "${EXAMPLE_NAME}" PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${EXAMPLE_DIR}"/inputs/" $<TARGET_FILE_DIR:${EXAMPLE_NAME}>"/inputs")

# Add to the build step the necessary python scripts.
add_custom_command(TARGET "${EXAMPLE_NAME}" PRE_BUILD
                  COMMAND ${CMAKE_COMMAND} -E copy_directory
                  ${EXAMPLE_DIR}"/python_scripts/" $<TARGET_FILE_DIR:${EXAMPLE_NAME}>"/python_scripts")

# ----------------------------------------------------------------------------------------------------------------------
# CONFIGURE THE COMPILERS

# Configure the compilers.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -m64 -O3 -pthread -fopenmp")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    #target_compile_options(${LIB_NAME} PUBLIC)
    message(FATAL_ERROR "Compiler not supported by default.")
else()
    message(FATAL_ERROR "Compiler not supported by default.")
endif()

# ----------------------------------------------------------------------------------------------------------------------
# INSTALLATION PROCESS

# Prepare the external library search folders.
set(ext_libs_loc ${MODULES_GLOBAL_INSTALL_LIB_PATH} ${MODULES_GLOBAL_INSTALL_BIN_PATH} ${CMAKE_BINARY_DIR}/bin)

# Install the launcher.
macro_install_launcher(${EXAMPLE_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Install runtime artifacts.
macro_install_runtime_artifacts(${EXAMPLE_NAME}
                                ${MODULES_GLOBAL_MAIN_DEP_SET_NAME}
                                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Install the runtime dependencies.
macro_install_runtime_deps("${EXAMPLE_NAME}"
                           "${MODULES_GLOBAL_MAIN_DEP_SET_NAME}"
                           "${ext_libs_loc}"
                           "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
                           "" "")

# ----------------------------------------------------------------------------------------------------------------------

# **********************************************************************************************************************
