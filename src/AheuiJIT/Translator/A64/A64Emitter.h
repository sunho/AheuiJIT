#pragma once
#include <AheuiJIT/IR/BasicBlock.h>
#include <AheuiJIT/IR/Instruction.h>
#include <AheuiJIT/IR/InstructionVisitor.h>
#include <AheuiJIT/Runtime/A64/A64Machine.h>
#include <AheuiJIT/Runtime/Runtime.h>
#include <AheuiJIT/Translator/Emitter.h>
#include <AheuiJIT/Translator/RegAlloc.h>
#include <asmjit/arm.h>
#include <asmjit/arm/a64assembler.h>

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

using A64Reg = Reg<asmjit::arm::Gp>;

struct A64RegAllocProtocol : public RegAllocProtocol<asmjit::arm::Gp> {
    A64RegAllocProtocol() = delete;
    explicit A64RegAllocProtocol(asmjit::a64::Assembler &code) : code(code) {
    }

    void emitStore(uint32_t offset, asmjit::arm::Gp reg) override {
        code.str(reg, asmjit::a64::ptr(asmjit::a64::sp, offset));
    }

    void emitLoad(asmjit::a64::Gp reg, uint32_t offset) override {
        code.ldr(reg, asmjit::a64::ptr(asmjit::a64::sp, offset));
    }

    void emitInitStub() override {
        stubOffset = code.offset();
        code.sub(asmjit::a64::sp, asmjit::a64::sp, 0);
    }

    void emitDeinitStub() override {
        deinitStubOffsets.push_back(code.offset());
        code.sub(asmjit::a64::sp, asmjit::a64::sp, 0);
    }

    void finalize(uint32_t size) override {
        const int64_t backup = code.offset();
        code.setOffset(stubOffset);
        code.sub(asmjit::a64::sp, asmjit::a64::sp, size);
        for (auto offset : deinitStubOffsets) {
            code.setOffset(offset);
            code.add(asmjit::a64::sp, asmjit::a64::sp, size);
        }
        code.setOffset(backup);
        deinitStubOffsets.clear();
        stubOffset = 0;
    }

    asmjit::arm::Gp convertToReg(uint8_t index) override {
        return asmjit::arm::Reg::fromTypeAndId(asmjit::BaseReg::RegType::kTypeGp64, index)
            .as<asmjit::arm::Gp>()
            .x();
    }

    uint8_t convertFromReg(asmjit::arm::Gp reg) override {
        return reg.id();
    }

  private:
    size_t stubOffset{ 0 };
    std::list<size_t> deinitStubOffsets;
    asmjit::a64::Assembler &code;
};

struct A64Emitter : public Emitter {
    A64Emitter() = delete;

    explicit A64Emitter(asmjit::a64::Assembler &code)
        : code(code), regAlloc(std::make_unique<A64RegAllocProtocol>(code)){};

    ~A64Emitter() = default;

    void emit(BasicBlock *bb, const TLBTable &table, std::set<BasicBlock *> &emitted) override;
    void visit(Instruction *inst);

#define INST(name, ret, ...) void name(__VA_ARGS__);
#include <AheuiJIT/IR/Instruction.inc>
#undef INST

  private:
    void emitPrologue();
    void emitEpilouge();

    void emitMethodCall(void *func, asmjit::arm::Gp arg0, asmjit::arm::Gp arg1);
    void emitRetMethodCall(void *func, asmjit::arm::Gp arg0, asmjit::arm::Gp ret);
    void emitJITRequest(const Location &location);
    void emitJITRequestImpl();
    void emitSetLocation(const Location &location);
    void emitSubroutines();

    inline A64Reg unwrapValue(Value *value);

    asmjit::a64::Assembler &code;
    RegAlloc<asmjit::arm::Gp, 29> regAlloc;
    BasicBlock *block{ nullptr };
    Instruction *inst{ nullptr };
    std::map<uint64_t, asmjit::Label> labels;
    asmjit::Label exitLabel;
    asmjit::Label jitRequestLabel;
};

}  // namespace aheuijit
