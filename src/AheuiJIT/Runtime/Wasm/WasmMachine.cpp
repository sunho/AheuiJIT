#include "WasmMachine.h"

#include <emscripten.h>

using namespace aheuijit;

BinaryenModuleRef WasmCodeBlock::getModule() const {
    return module_;
}

// clang-format off
EM_JS(void, _setWasm, (uint32_t ptr, uint32_t size), { 
    Asyncify.handleAsync(async() => { 
        await Module['AheuiJIT'].setWasm(ptr, size); 
    }); 
});

EM_JS(void, _runWasm, (uint32_t ptr, uint32_t sp), { 
    Asyncify.handleAsync(async() => { 
        await Module['AheuiJIT'].runWasm(ptr, sp); 
    }); 
});
// clang-format on

CodeBlockPtr WasmMachine::createBlock() {
    return std::make_unique<WasmCodeBlock>(BinaryenModuleCreate());
}

WasmMachine::WasmMachine() {
    stack = new uint8_t[WASM_STACK_SIZE];
}

WasmMachine::~WasmMachine() {
    delete[] stack;
}

void WasmMachine::addCodeBlock(const Location& location, CodeBlock* block,
                               std::set<BasicBlock*> emitted) {
    WasmCodeBlock* wasmblock = dynamic_cast<WasmCodeBlock*>(block);
    BinaryenModuleRef module_ = wasmblock->getModule();
    BinaryenModuleAllocateAndWriteResult res = BinaryenModuleAllocateAndWrite(module_, nullptr);
    _setWasm(reinterpret_cast<uint32_t>(res.binary), static_cast<uint32_t>(res.binaryBytes));
    free(res.binary);
}

bool WasmMachine::hasCodeBlock(const Location& location) {
    // We should always retanslate
    return false;
}

void WasmMachine::runCodeBlock(const Location& location, RuntimeContext* ctx) {
    _runWasm(reinterpret_cast<uint32_t>(ctx), reinterpret_cast<uint32_t>(stack) + WASM_STACK_SIZE);
}

// clang-format off
Word WasmMachine::inputChar() {
    return EM_ASM_INT({
        return Module['AheuiJIT'].inputChar();
    });
}

Word WasmMachine::inputNum() {
    return EM_ASM_INT({
        return Module['AheuiJIT'].inputNum();
    });
}

void WasmMachine::printChar(Word word) {
    EM_ASM({
        return Module['AheuiJIT'].printChar($0);
    }, static_cast<int32_t>(word));
}

void WasmMachine::printNum(Word word) {
    EM_ASM({
        return Module['AheuiJIT'].printNum($0);
    }, static_cast<int32_t>(word));
}
// clang-format on