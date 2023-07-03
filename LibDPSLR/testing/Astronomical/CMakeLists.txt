
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++17 -fopenmp")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Testing/Astronomical)
set(LIBRARIES LibDPSLR)

if (WIN32)
    link_directories(${CMAKE_CURRENT_BINARY_DIR})
endif (WIN32)

if (UNIX)
    link_directories(${CMAKE_CURRENT_BINARY_DIR}/../lib)
endif (UNIX)

# Get a list of all .cpp files in the same folder as CMakeLists.txt
file(GLOB TEST_SOURCES *.cpp)

# Create a test executable for each test source file
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    set(APP_NAME ${TEST_NAME})
    set(SOURCES ${TEST_SOURCE})
    macro_setup_launcher(${TEST_NAME})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()


