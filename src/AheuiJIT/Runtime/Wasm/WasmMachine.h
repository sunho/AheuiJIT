#pragma once

#include <AheuiJIT/Runtime/Machine.h>
#include <binaryen-c.h>

namespace aheuijit {

constexpr size_t WASM_PAGE_SIZE = 65536;
constexpr size_t WASM_STACK_SIZE = 1024;

struct WasmCodeBlock : public CodeBlock {
    explicit WasmCodeBlock(BinaryenModuleRef module_) : module_(module_) {
    }
    ~WasmCodeBlock() = default;
    BinaryenModuleRef getModule() const;

  private:
    BinaryenModuleRef module_;
};

struct WasmMachine : public Machine {
    WasmMachine();
    ~WasmMachine();
    CodeBlockPtr createBlock() override;
    void addCodeBlock(const Location& location, CodeBlock* block,
                      std::set<BasicBlock*> emitted) override;
    bool hasCodeBlock(const Location& location) override;
    void runCodeBlock(const Location& location, RuntimeContext* ctx) override;

    void printChar(Word word) override;
    void printNum(Word word) override;
    Word inputChar() override;
    Word inputNum() override;

  private:
    uint8_t* stack;
};

}  // namespace aheuijit
