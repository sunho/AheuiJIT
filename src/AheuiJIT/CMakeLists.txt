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
    Runtime/RuntimeContext.h
    Runtime/Runtime.h
    Runtime/Runtime.cpp
    Runtime/Machine.h
    Runtime/MachineFactory.h
    Runtime/MachineFactory.cpp
    Runtime/Interpreter.h
    Runtime/Interpreter.cpp
    Translator/Emitter.h
    Translator/Decoder.cpp
    Translator/TokenOp.inc
    Translator/Translator.h
    Translator/Translator.cpp
    Translator/Token.h
    Translator/RegAlloc.h
    Translator/RegAlloc.cpp
    Util/Util.h
    Util/Disasm.h
    Util/Disasm.cpp
)

if(EMSCRIPTEN)
set(SOURCE_LIST
    ${SOURCE_LIST}
    Translator/Wasm/WasmEmitter.cpp
    Translator/Wasm/WasmEmitter.h
    Runtime/Wasm/WasmMachine.h
    Runtime/Wasm/WasmMachine.cpp
)
add_library(AheuiJIT ${SOURCE_LIST})
target_include_directories(AheuiJIT PUBLIC ..)
target_include_directories(AheuiJIT PRIVATE ${binaryen_INCLUDE_DIRS})
target_link_libraries(AheuiJIT PRIVATE fmt binaryen)
else()
set(SOURCE_LIST
    ${SOURCE_LIST}
    Translator/X86/X86Emitter.cpp
    Translator/X86/X86Emitter.h
    Runtime/X86/X86Machine.h
    Runtime/X86/X86Machine.cpp
    Runtime/X86/IOProtocol.h
    Runtime/X86/IOProtocol.cpp
)
add_library(AheuiJIT ${SOURCE_LIST})
target_include_directories(AheuiJIT PUBLIC ..)
target_include_directories(AheuiJIT PRIVATE ${capstone_INCLUDE_DIRS})
target_link_libraries(AheuiJIT PRIVATE asmjit fmt capstone-static)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})


install (TARGETS AheuiJIT RUNTIME DESTINATION aheuijit)
INSTALL (
    DIRECTORY ${CMAKE_SOURCE_DIR}/src/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h*" PATTERN "*.inc" PATTERN "X86" EXCLUDE PATTERN "Wasm" EXCLUDE
    )