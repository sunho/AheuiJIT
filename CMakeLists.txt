cmake_minimum_required(VERSION 3.10.2)
project(AheuiJIT)

option(AHEUIJIT_TRAGET "target for JIT compilation" "X86")
option(ENABLE_ASAN "turn on address santiziers and various debug flags" OFF)

if (ENABLE_ASAN)
    add_compile_options(-fsanitize=address,undefined -g)
    add_link_options(-fsanitize=address,undefined -g)
endif()

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