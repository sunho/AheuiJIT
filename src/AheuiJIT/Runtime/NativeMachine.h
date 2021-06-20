#pragma once

#include <AheuiJIT/Runtime/IOProtocol.h>
#include <AheuiJIT/Runtime/Machine.h>
#include <asmjit/x86.h>

namespace aheuijit {

template <typename T>
struct NativeCodeBlock : public CodeBlock {
    NativeCodeBlock(asmjit::JitRuntime& rt) {
        codeHolder.init(rt.environment());
        codeHolder.attach(&codeAsm);
    }
    ~NativeCodeBlock() = default;
    T& getAssembler() {
        return codeAsm;
    }
    asmjit::CodeHolder& getCodeHolder() {
        return codeHolder;
    }

  private:
    asmjit::CodeHolder codeHolder;
    T codeAsm;
};

using EntryFunction = void (*)(RuntimeContext*);

template <typename T, auto Disasm>
struct NativeMachine : public Machine {
    NativeMachine(std::unique_ptr<IOProtocol> io) : io(std::move(io)) {
    }
    ~NativeMachine() = default;
    CodeBlockPtr createBlock() override {
        return std::make_unique<NativeCodeBlock<T>>(rt);
    }
    void addCodeBlock(const Location& location, CodeBlock* block,
                      std::set<BasicBlock*> emitted) override {
        NativeCodeBlock<T>* cblock = dynamic_cast<NativeCodeBlock<T>*>(block);
        EntryFunction func;
        rt.add(&func, &cblock->getCodeHolder());
        asmjit::CodeHolder& codeHolder = cblock->getCodeHolder();
        for (auto b : emitted) {
            const auto labelId = codeHolder.labelIdByName(std::to_string(b->id).c_str());
            tlbTable.emplace(b->location,
                             reinterpret_cast<uint64_t>(func) + codeHolder.labelOffset(labelId));
        }
        if (conf.logDisasm) {
            Disasm((void*)func, codeHolder.codeSize());
        }
        blocks.emplace(location, func);
    }
    bool hasCodeBlock(BasicBlock* bb) override {
        return blocks.find(bb->location) != blocks.end();
    }
    void runCodeBlock(BasicBlock* bb, RuntimeContext* ctx) override {
        blocks.at(bb->location)(ctx);
    }

    void printChar(Word word) override {
        return printCharImpl(word);
    }

    void printNum(Word word) override {
        return printNumImpl(word);
    }
    Word inputChar() override {
        return inputCharImpl();
    }
    Word inputNum() override {
        return io->inputNum();
    }
    void reset() override {
        blocks.clear();
    }

    void printCharImpl(Word word) {
        if (word < 0x80) {
            io->printChar(word);
        } else if (word < 0x0800) {
            io->printChar(0xc0 | (word >> 6));
            io->printChar(0x80 | ((word >> 0) & 0x3f));
        } else if (word < 0x10000) {
            io->printChar(0xe0 | (word >> 12));
            io->printChar(0x80 | ((word >> 6) & 0x3f));
            io->printChar(0x80 | ((word >> 0) & 0x3f));
        } else if (word < 0x110000) {
            io->printChar(0xf0 | (word >> 18));
            io->printChar(0x80 | ((word >> 12) & 0x3f));
            io->printChar(0x80 | ((word >> 6) & 0x3f));
            io->printChar(0x80 | ((word >> 0) & 0x3f));
        }
    }
    void printNumImpl(Word word) {
        io->printNum(word);
    }
    Word inputCharImpl() {
        Word a = io->inputChar();
        if (a == -1) {
            return -1;
        }

        if (a < 0x80) {
            return a;
        } else if ((a & 0xf0) == 0xf0) {
            return ((a & 0x07) << 18) + ((io->inputChar() & 0x3f) << 12) +
                   ((io->inputChar() & 0x3f) << 6) + (io->inputChar() & 0x3f);
        } else if ((a & 0xe0) == 0xe0) {
            return ((a & 0x0f) << 12) + ((io->inputChar() & 0x3f) << 6) + (io->inputChar() & 0x3f);
        } else if ((a & 0xc0) == 0xc0) {
            return ((a & 0x1f) << 6) + (io->inputChar() & 0x3f);
        } else {
            return -1;
        }
    }

    Word inputNumImpl() {
        return io->inputNum();
    }

  private:
    std::unique_ptr<IOProtocol> io;
    asmjit::JitRuntime rt;
    std::map<Location, EntryFunction> blocks;
};

}  // namespace aheuijit
