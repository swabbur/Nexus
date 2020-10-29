cmake_minimum_required(VERSION 3.13)
project(Nexus VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
        src/nexus/Selector.cpp
        src/nexus/ServerSocket.cpp
        src/nexus/Socket.cpp)

find_package(Threads REQUIRED)

add_library(Nexus ${SOURCES})
target_include_directories(Nexus PUBLIC include/)
target_link_libraries(Nexus Threads::Threads)

add_executable(NexusExample src/Main.cpp)
target_link_libraries(NexusExample Nexus)

include(CMakeDependentOption)

cmake_dependent_option(
        NEXUS_BUILD_TESTS
        "Enable ${PROJECT_NAME} project tests targets" ON
        "BUILD_TESTING" OFF)

if (NEXUS_BUILD_TESTS)

    enable_testing()

    find_package(GTest REQUIRED)
    include(GoogleTest REQUIRED)

    add_executable(NexusTest
            tests/nexus/ServerSocketTest.cpp
            tests/nexus/SocketTest.cpp)

    target_link_libraries(NexusTest Nexus GTest::GTest GTest::Main)
    gtest_discover_tests(NexusTest)
endif()
