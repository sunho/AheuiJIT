#pragma once
#include <AheuiJIT/IR/BasicBlock.h>
#include <AheuiJIT/IR/Instruction.h>
#include <AheuiJIT/IR/InstructionVisitor.h>
#include <AheuiJIT/Runtime/Runtime.h>
#include <AheuiJIT/Runtime/X86/X86Machine.h>
#include <AheuiJIT/Translator/Emitter.h>
#include <AheuiJIT/Translator/RegAlloc.h>
#include <asmjit/x86.h>

#include <list>

#ifdef _WIN32

#define METHOD_TO_POINTER(class, method, ret, ...)                                                 \
    ([]() {                                                                                        \
        ret (__thiscall class ::*pFunc)(__VA_ARGS__) = &class ::method;                            \
        return (void *&)pFunc;                                                                     \
    }())

#else

#define METHOD_TO_POINTER(class, method, ret, ...)                                                 \
    ([]() {                                                                                        \
        ret (class ::*pFunc)(__VA_ARGS__) = &class ::method;                                       \
        return (void *&)pFunc;                                                                     \
    }())

#endif

namespace aheuijit {

using X86Reg = Reg<asmjit::x86::Gp>;

struct X86RegAllocProtocol : public RegAllocProtocol<asmjit::x86::Gp> {
    X86RegAllocProtocol() = delete;
    explicit X86RegAllocProtocol(asmjit::x86::Assembler &code) : code(code) {
    }

    void emitStore(uint32_t offset, asmjit::x86::Gp reg) override {
        code.mov(asmjit::x86::ptr(asmjit::x86::rsp, offset), reg);
    }

    void emitLoad(asmjit::x86::Gp reg, uint32_t offset) override {
        code.mov(reg, asmjit::x86::ptr(asmjit::x86::rsp, offset));
    }

    void emitInitStub() override {
        stubOffset = code.offset();
        code.long_().sub(asmjit::x86::rsp, 0);
    }

    void emitDeinitStub() override {
        deinitStubOffsets.push_back(code.offset());
        code.long_().sub(asmjit::x86::rsp, 0);
    }

    void finalize(uint32_t size) override {
        const int64_t backup = code.offset();
        code.setOffset(stubOffset);
        code.long_().sub(asmjit::x86::rsp, size);
        for (auto offset : deinitStubOffsets) {
            code.setOffset(offset);
            code.long_().add(asmjit::x86::rsp, size);
        }
        code.setOffset(backup);
        deinitStubOffsets.clear();
        stubOffset = 0;
    }

    asmjit::x86::Gp convertToReg(uint8_t index) override {
        return asmjit::x86::Reg::fromTypeAndId(asmjit::BaseReg::RegType::kTypeGp64, index)
            .as<asmjit::x86::Gp>();
    }

    uint8_t convertFromReg(asmjit::x86::Gp reg) override {
        return reg.id();
    }

  private:
    size_t stubOffset{ 0 };
    std::list<size_t> deinitStubOffsets;
    asmjit::x86::Assembler &code;
};

struct X86Emitter : public Emitter {
    X86Emitter() = delete;

    explicit X86Emitter(asmjit::x86::Assembler &code)
        : code(code), regAlloc(std::make_unique<X86RegAllocProtocol>(code)){};

    ~X86Emitter() = default;

    void emit(BasicBlock *bb, const TLBTable &table, std::set<BasicBlock *> &emitted) override;
    void visit(Instruction *inst);

#define INST(name, ret, ...) void name(__VA_ARGS__);
#include <AheuiJIT/IR/Instruction.inc>
#undef INST

  private:
    void emitPrologue();
    void emitEpilouge();

    void emitMethodCall(void *func, asmjit::x86::Gp arg0, asmjit::x86::Gp arg1);
    void emitRetMethodCall(void *func, asmjit::x86::Gp arg0, asmjit::x86::Gp ret);
    void emitFunctionCall(void *func, asmjit::x86::Gp arg0);
    void emitJITRequest(const Location &location);
    void emitJITRequestImpl();
    void emitSetLocation(const Location &location);
    void emitSubroutines();

    inline X86Reg unwrapValue(Value *value);

    bool popFixup{ false };
    asmjit::x86::Assembler &code;
    RegAlloc<asmjit::x86::Gp, 16> regAlloc;
    BasicBlock *block{ nullptr };
    Instruction *inst{ nullptr };
    std::map<uint64_t, asmjit::Label> labels;
    asmjit::Label exitLabel;
    asmjit::Label jitRequestLabel;
};

}  // namespace aheuijit
