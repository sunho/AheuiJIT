set(SOURCE_LIST
    IR/BasicBlock.h
    IR/BasicBlock.cpp
    IR/Builder.h
    IR/Builder.cpp
    IR/Storage.h
    IR/Instruction.h
    IR/Instruction.inc
    IR/Value.h
    IR/Value.cpp
    Runtime/Runtime.h
    Runtime/Runtime.cpp
    Translator/Emitter.cpp
    Translator/Emitter.h
    Translator/RegAlloc.h
    Translator/RegAlloc.cpp
    Translator/Translator.h
    Translator/Translator.cpp
)

add_library(AheuiJIT ${SOURCE_LIST})
target_include_directories(AheuiJIT PUBLIC ..)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})