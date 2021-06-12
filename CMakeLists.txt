cmake_minimum_required(VERSION 3.13.4)
project(AheuiJIT)

if(EMSCRIPTEN)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/wasm)
endif()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(external)
add_subdirectory(src/AheuiJIT)

if(EMSCRIPTEN)
    add_subdirectory(src/AheuiJIT/Wasm)
else()
    add_subdirectory(src/AheuiJIT/Cli)
endif()