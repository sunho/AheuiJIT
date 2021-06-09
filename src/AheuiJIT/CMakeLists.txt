set(SOURCE_LIST
    IR/BasicBlock.h
    IR/BasicBlock.cpp
    IR/Builder.h
    IR/Builder.cpp
    IR/Storage.h
    IR/Instruction.cpp
    IR/Instruction.h
    IR/InstructionVisitor.h
    IR/Instruction.inc
    IR/Location.h
    IR/Location.cpp
    IR/Value.h
    IR/Value.cpp
    IR/Pass/Pass.h
    IR/Pass/EndpointPass.cpp
    IR/Pass/AggregatePushPopPass.cpp
    IR/Pass/ConstantFoldPass.cpp
    Runtime/JITContext.h
    Runtime/IOProtocol.h
    Runtime/IOProtocol.cpp
    Runtime/Runtime.h
    Runtime/Runtime.cpp
    Translator/Decoder.cpp
    Translator/Emitter.cpp
    Translator/Emitter.h
    Translator/RegAlloc.h
    Translator/RegAlloc.cpp
    Translator/TokenOp.inc
    Translator/Translator.h
    Translator/Translator.cpp
    Translator/Token.h
    Util/Util.h
    Util/Disasm.h
    Util/Disasm.cpp
)

add_library(AheuiJIT ${SOURCE_LIST})
target_include_directories(AheuiJIT PUBLIC ..)
target_include_directories(AheuiJIT PRIVATE ${capstone_INCLUDE_DIRS})
target_link_libraries(AheuiJIT PRIVATE asmjit fmt capstone-static)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})


install (TARGETS AheuiJIT RUNTIME DESTINATION aheuijit)
INSTALL (
    DIRECTORY ${CMAKE_SOURCE_DIR}/src/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h*" PATTERN "*.inc")