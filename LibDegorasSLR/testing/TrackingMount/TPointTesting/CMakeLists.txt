# **********************************************************************************************************************
# Updated 23/01/2024
# **********************************************************************************************************************

# ----------------------------------------------------------------------------------------------------------------------
# TPointTesting configuration

# Test name.
set(TEST_NAME "TPointTesting")

# Set the installation path.
set(TEST_INSTALL_PATH ${GLOBAL_LIBDPSLR_TESTS_INSTALL_PATH}/${TEST_NAME})

# Configure names and build path.
set(APP_BUILD_FOLDER ${CMAKE_BINARY_DIR}/bin/Testing/${TEST_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${APP_BUILD_FOLDER})

# Add a custom target for Python scripts to make them visible in IDEs
file(GLOB PYTHON_SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/python_scripts/*.py")
source_group("PythonScripts" FILES ${PYTHON_SCRIPTS})

# Log.
message(STATUS "Setup complex test: ${TEST_NAME}")

# Set the sources.
set(SOURCES "TPointTesting.cpp")

# Set resources.
set(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/inputs/model.dat)
source_group("Resource Files" FILES ${RESOURCES})

# Setup the launcher.
macro_setup_launcher("${TEST_NAME}"
                     "${MODULES_GLOBAL_LIBS_OPTIMIZED}"
                     "${MODULES_GLOBAL_LIBS_DEBUG}"
                     "${SOURCES}" "${PYTHON_SCRIPTS}")

# Include directories for the target.
target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/includes)



# Add to the build step the necessary input files for the example.
add_custom_command(TARGET "${TEST_NAME}" PRE_BUILD
                  COMMAND ${CMAKE_COMMAND} -E copy_directory
                  "${CMAKE_CURRENT_SOURCE_DIR}/inputs/" "$<TARGET_FILE_DIR:${TEST_NAME}>/inputs")

# Add to the build step the necessary python scripts.
add_custom_command(TARGET "${TEST_NAME}" PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   "${CMAKE_CURRENT_SOURCE_DIR}/python_scripts/" "$<TARGET_FILE_DIR:${TEST_NAME}>/python_scripts")


# Prepare the external library search folders.
set(ext_libs_loc
    ${MODULES_GLOBAL_INSTALL_LIB_PATH}
    ${MODULES_GLOBAL_INSTALL_BIN_PATH}
    ${CMAKE_BINARY_DIR}/bin)

# Install the launcher.
macro_install_launcher(${TEST_NAME} ${TEST_INSTALL_PATH})

# Install runtime artifacts.
macro_install_runtime_artifacts(${TEST_NAME}
                                ${MODULES_GLOBAL_MAIN_DEP_SET_NAME}
                                ${TEST_INSTALL_PATH})



# Install the runtime dependencies.
macro_install_runtime_deps("${TEST_NAME}"
                           "${MODULES_GLOBAL_MAIN_DEP_SET_NAME}"
                           "${ext_libs_loc}"
                           "${TEST_INSTALL_PATH}"
                           "" "")

# Install example input data.
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/inputs/"
      DESTINATION "${TEST_INSTALL_PATH}/inputs"
      FILES_MATCHING PATTERN "*")


# Install auxiliar python scripts.
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/python_scripts/"
        DESTINATION "${TEST_INSTALL_PATH}/python_scripts"
        FILES_MATCHING PATTERN "*.py")




