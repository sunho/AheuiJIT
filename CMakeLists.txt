cmake_minimum_required(VERSION 3.13.4)
project(AheuiJIT)

option(BUILD_CLI "Build CLI" ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(external)
add_subdirectory(src/AheuiJIT)

if (BUILD_CLI)
    add_subdirectory(src/AheuiJIT/Cli)
endif()