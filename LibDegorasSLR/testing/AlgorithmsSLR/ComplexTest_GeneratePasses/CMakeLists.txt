# **********************************************************************************************************************
# Updated 23/01/2024
# **********************************************************************************************************************

# ----------------------------------------------------------------------------------------------------------------------
# ComplexTest_GeneratePassesCPF configuration

# Test name.
set(TEST_NAME "ComplexTest_GeneratePassesCPF")

# Set the installation path.
set(TEST_INSTALL_PATH ${GLOBAL_LIBDPSLR_TESTS_INSTALL_PATH}/${TEST_NAME})

# Configure names and build path.
set(APP_BUILD_FOLDER ${CMAKE_BINARY_DIR}/bin/Testing/${TEST_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${APP_BUILD_FOLDER})

# Log.
message(STATUS "Setup complex test: ${TEST_NAME}")

# Set the sources.
set(SOURCES "ComplexTest_GeneratePassesCPF.cpp")

# Set resources.
set(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/inputs/39380_cpf_230309_5681.tjr)
source_group("Resource Files" FILES ${RESOURCES})

# Setup the launcher.
macro_setup_launcher("${TEST_NAME}"
                     "${MODULES_GLOBAL_LIBS_OPTIMIZED}"
                     "${MODULES_GLOBAL_LIBS_DEBUG}"
                     "${SOURCES}")

# Include directories for the target.
target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/includes)

# Install the launcher.
macro_install_launcher(${TEST_NAME} ${TEST_INSTALL_PATH})

# Install runtime artifacts.
macro_install_runtime_artifacts(${TEST_NAME}
                                ${MODULES_GLOBAL_MAIN_DEP_SET_NAME}
                                ${TEST_INSTALL_PATH})

# Prepare the external library search folders.
set(ext_libs_loc
    ${MODULES_GLOBAL_INSTALL_LIB_PATH}
    ${MODULES_GLOBAL_INSTALL_BIN_PATH}
    ${CMAKE_BINARY_DIR}/bin)

# Install the runtime dependencies.
macro_install_runtime_deps("${TEST_NAME}"
                           "${MODULES_GLOBAL_MAIN_DEP_SET_NAME}"
                           "${ext_libs_loc}"
                           "${TEST_INSTALL_PATH}"
                           "" "")

# Add to the build step the necessary input files for the example.
add_custom_command(TARGET "${TEST_NAME}" PRE_BUILD
                  COMMAND ${CMAKE_COMMAND} -E copy_directory
                  "${CMAKE_CURRENT_SOURCE_DIR}/inputs/" "$<TARGET_FILE_DIR:${TEST_NAME}>/inputs")

# Install example input data.
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/inputs/"
      DESTINATION "${TEST_INSTALL_PATH}/inputs"
      FILES_MATCHING PATTERN "*")

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/inputs/39380_cpf_230309_5681.tjr
#     DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/inputs)


