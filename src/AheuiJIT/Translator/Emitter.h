#pragma once
#include <AheuiJIT/IR/BasicBlock.h>
#include <AheuiJIT/IR/Instruction.h>
#include <AheuiJIT/IR/InstructionVisitor.h>
#include <AheuiJIT/Runtime/Runtime.h>
#include <AheuiJIT/Translator/RegAlloc.h>
#include <asmjit/x86.h>

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

using TLBTable = std::map<Location, uint64_t>;

struct Emitter {
    Emitter() = delete;

    explicit Emitter(Runtime &rt, asmjit::x86::Assembler &code)
        : rt(rt), code(code), regAlloc(code){};

    ~Emitter() = default;

    void emit(BasicBlock *bb, const TLBTable &table, std::set<BasicBlock *> &emitted);
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
    void emitJITRequest();
    void emitSetLocation(const Location &location);

    inline Reg unwrapValue(Value *value);

    bool popFixup{ false };
    RegAlloc regAlloc;
    Runtime &rt;
    asmjit::x86::Assembler &code;
    BasicBlock *block{ nullptr };
    Instruction *inst{ nullptr };
    std::map<uint64_t, asmjit::Label> labels;
    asmjit::Label exit;
};

}  // namespace aheuijit
