#include "WasmMachine.h"

#include <emscripten.h>

using namespace aheuijit;

BinaryenModuleRef WasmCodeBlock::getModule() const {
    return module_;
}

// clang-format off
EM_JS(void, _setWasm, (uint32_t id, uint32_t ptr, uint32_t size), { 
    Asyncify.handleAsync(async() => { 
        await Module['AheuiJIT'].setWasm(id, ptr, size); 
    }); 
});

EM_JS(void, _runWasm, (uint32_t id, uint32_t ptr, uint32_t sp, uint32_t bb), { 
    Asyncify.handleAsync(async() => { 
        await Module['AheuiJIT'].runWasm(id, ptr, sp, bb); 
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
    const uint32_t codeBlockId = nextId++;
    _setWasm(codeBlockId, reinterpret_cast<uint32_t>(res.binary),
             static_cast<uint32_t>(res.binaryBytes));
    free(res.binary);
    BinaryenModuleDispose(module_);
    for (auto bb : emitted) {
        blockTable[bb->id] = codeBlockId;
    }
}

bool WasmMachine::hasCodeBlock(BasicBlock* bb) {
    return blockTable.find(bb->id) != blockTable.end();
}

void WasmMachine::runCodeBlock(BasicBlock* bb, RuntimeContext* ctx) {
    const uint32_t codeBlockId = blockTable.at(bb->id);
    _runWasm(codeBlockId, reinterpret_cast<uint32_t>(ctx),
             reinterpret_cast<uint32_t>(stack) + WASM_STACK_SIZE, bb->id);
}

void WasmMachine::reset() {
    nextId = 1;
    blockTable.clear();
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