cmake_minimum_required(VERSION 3.10.2)
project(AheuiJIT)

set(AHEUIJIT_TRAGET "X86" CACHE STRING "")

if(EMSCRIPTEN)
set(WASM ON)
elseif(${AHEUIJIT_TRAGET} STREQUAL "X86" )
set(X86 ON)
add_definitions(-DX86)
elseif(${AHEUIJIT_TRAGET} STREQUAL "AARCH")
set(AARCH ON)
add_definitions(-DAARCH)
endif()

if(WASM)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/wasm)
    add_compile_options(-O3 -s DISABLE_EXCEPTION_CATCHING=1)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(external)
add_subdirectory(src/AheuiJIT)

if(WASM)
    add_subdirectory(src/AheuiJIT/Wasm)
else()
    add_subdirectory(src/AheuiJIT/Cli)
endif()