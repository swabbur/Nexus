cmake_minimum_required(VERSION 3.13)
project(Nexus VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
        src/nexus/Buffer.cpp
        src/nexus/Exception.cpp
        src/nexus/Selector.cpp
        src/nexus/tcp/ServerSocket.cpp
        src/nexus/tcp/Socket.cpp
        src/nexus/tcp/WinSock.cpp
        src/Main.cpp)

find_package(Threads REQUIRED)

add_executable(Nexus ${SOURCES})
target_include_directories(Nexus PUBLIC include)
target_link_libraries(Nexus Threads::Threads)

