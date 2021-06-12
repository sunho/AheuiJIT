#pragma once

#include <AheuiJIT/Runtime/Machine.h>
#include <AheuiJIT/Runtime/X86/IOProtocol.h>
#include <asmjit/x86.h>

namespace aheuijit {

struct X86CodeBlock : public CodeBlock {
    X86CodeBlock(asmjit::JitRuntime& rt);
    ~X86CodeBlock() = default;
    asmjit::x86::Assembler& getAssembler();
    asmjit::CodeHolder& getCodeHolder();

  private:
    asmjit::CodeHolder codeHolder;
    asmjit::x86::Assembler codeAsm;
};

using EntryFunction = void (*)(RuntimeContext*);

struct X86Machine : public Machine {
    X86Machine(std::unique_ptr<IOProtocol> io);
    ~X86Machine() = default;
    CodeBlockPtr createBlock() override;
    void addCodeBlock(const Location& location, CodeBlock* block,
                      std::set<BasicBlock*> emitted) override;
    bool hasCodeBlock(const Location& location) override;
    void runCodeBlock(const Location& location, RuntimeContext* ctx) override;

    void printChar(Word word) override;
    void printNum(Word word) override;
    Word inputChar() override;
    Word inputNum() override;

    void printCharImpl(Word word);
    void printNumImpl(Word word);
    Word inputCharImpl();
    Word inputNumImpl();

  private:
    std::unique_ptr<IOProtocol> io;
    asmjit::JitRuntime rt;
    std::map<Location, EntryFunction> blocks;
};

}  // namespace aheuijit
