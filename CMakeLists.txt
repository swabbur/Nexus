cmake_minimum_required(VERSION 3.13)
project(Nexus VERSION 1.0.0 LANGUAGES CXX)

# Configure CMake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Find sources
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")

# Create library
add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")

# Check if tests should be build
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)

    # Find test packages
    find_package(Catch2 REQUIRED)
    find_package(Threads REQUIRED)

    # Find test sources
    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/tests/*.cpp")

    # Create test executable
    add_executable(${PROJECT_NAME}Test ${TEST_SOURCES})
    target_link_libraries(${PROJECT_NAME}Test ${PROJECT_NAME} Catch2::Catch2WithMain Threads::Threads)
endif()
