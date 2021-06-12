#include "X86Machine.h"

#include <AheuiJIT/Util/Disasm.h>

using namespace aheuijit;
using namespace asmjit;

X86CodeBlock::X86CodeBlock(JitRuntime& rt) {
    codeHolder.init(rt.environment());
    codeHolder.attach(&codeAsm);
}

CodeHolder& X86CodeBlock::getCodeHolder() {
    return codeHolder;
}

x86::Assembler& X86CodeBlock::getAssembler() {
    return codeAsm;
}

X86Machine::X86Machine(std::unique_ptr<IOProtocol> io) : io(std::move(io)) {
}

CodeBlockPtr X86Machine::createBlock() {
    return std::make_unique<X86CodeBlock>(rt);
}

void X86Machine::addCodeBlock(const Location& location, CodeBlock* block,
                              std::set<BasicBlock*> emitted) {
    X86CodeBlock* x86block = dynamic_cast<X86CodeBlock*>(block);
    EntryFunction func;
    rt.add(&func, &x86block->getCodeHolder());
    CodeHolder& codeHolder = x86block->getCodeHolder();
    for (auto b : emitted) {
        const auto labelId = codeHolder.labelIdByName(std::to_string(b->id).c_str());
        tlbTable.emplace(b->location,
                         reinterpret_cast<uint64_t>(func) + codeHolder.labelOffset(labelId));
    }
    if (conf.logDisasm) {
        disassembleX86((void*)func, codeHolder.codeSize());
    }
    blocks.emplace(location, func);
}

bool X86Machine::hasCodeBlock(const Location& location) {
    return blocks.find(location) != blocks.end();
}

void X86Machine::runCodeBlock(const Location& location, RuntimeContext* ctx) {
    blocks.at(location)(ctx);
}

Word X86Machine::inputChar() {
    return inputCharImpl();
}

Word X86Machine::inputNum() {
    return inputNumImpl();
}

void X86Machine::printChar(Word word) {
    return printCharImpl(word);
}

void X86Machine::printNum(Word word) {
    return printNumImpl(word);
}

void X86Machine::printNumImpl(Word word) {
    io->printNum(word);
}

Word X86Machine::inputNumImpl() {
    return io->inputNum();
}

// https://github.com/aheui/caheui/blob/master/aheui.c#L113

Word X86Machine::inputCharImpl() {
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

void X86Machine::printCharImpl(Word word) {
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
