#include "X86Emitter.h"

#include <AheuiJIT/IR/Value.h>
#include <AheuiJIT/Util/Util.h>
#include <fmt/printf.h>

#include <stack>

using namespace asmjit;
using namespace aheuijit;

#ifdef _WIN32

std::array<x86::Gp, 7> CALLER_SAVE_REGS = { x86::rax, x86::rcx, x86::rdx, x86::r8,
                                            x86::r9,  x86::r10, x86::r11 };

std::array<x86::Gp, 7> CALLE_SAVE_REGS = { x86::rsi, x86::rdi, x86::rbx, x86::r12,
                                           x86::r13, x86::r14, x86::r15 };

std::array<x86::Gp, 2> PARAM_REGS = { x86::rcx, x86::rdx };

constexpr int SHADOW_SPACE = 32;

#else

std::array<x86::Gp, 9> CALLER_SAVE_REGS = { x86::rax, x86::rcx, x86::rdx, x86::rsi, x86::rdi,
                                            x86::r8,  x86::r9,  x86::r10, x86::r11 };

std::array<x86::Gp, 2> PARAM_REGS = { x86::rdi, x86::rsi };

std::array<x86::Gp, 5> CALLE_SAVE_REGS = { x86::rbx, x86::r12, x86::r13, x86::r14, x86::r15 };

constexpr int SHADOW_SPACE = 0;

#endif

void X86Emitter::emit(BasicBlock *bb, const TLBTable &table, std::set<BasicBlock *> &emitted) {
    emitPrologue();
    std::stack<BasicBlock *> todo;
    std::set<uint64_t> done;
    exitLabel = code.newLabel();
    jitRequestLabel = code.newLabel();

    if (table.find(bb->location) != table.end()) {
        code.jmp(table.at(bb->location));
    } else {
        todo.push(bb);
    }

    const auto getOrCreateLabel = [&](uint64_t id) {
        if (labels.find(id) == labels.end()) {
            labels.emplace(id, code.newNamedLabel(std::to_string(id).c_str()));
        }
        return labels[id];
    };

    while (!todo.empty()) {
        block = todo.top();
        todo.pop();
        if (done.find(block->id) != done.end()) {
            continue;
        }
        if (conf.logIR) {
            fmt::print("{}", block->description());
        }
        emitted.insert(block);

        const Label label = getOrCreateLabel(block->id);
        code.bind(label);
        regAlloc.emitInitStub();

        const auto lock = regAlloc.allocSystem(x86::rbp);
        const auto lock2 = regAlloc.allocSystem(x86::rsp);
        const auto lock3 = regAlloc.allocSystem(x86::rdi);

        for (auto inst : block->insts) {
            regAlloc.setInstructionIndex(inst->offset);
            visit(inst);
        }

        done.insert(block->id);

        switch (block->terminal->getType()) {
            case TerminalType::Exit:
                regAlloc.emitDeinitStub();
                regAlloc.finalize();
                regAlloc.reset();
                code.jmp(exitLabel);
                break;
            case TerminalType::Link: {
                regAlloc.emitDeinitStub();
                regAlloc.finalize();
                regAlloc.reset();
                LinkTerminal *term = dynamic_cast<LinkTerminal *>(block->terminal);
                const auto it = table.find(term->block->location);
                if (it != table.end()) {
                    code.jmp(it->second);
                } else {
                    const Label label_ = getOrCreateLabel(term->block->id);
                    code.jmp(label_);
                    todo.push(term->block);
                }
                break;
            }
            case TerminalType::Conditional: {
                const auto lock4 = regAlloc.allocSystem(x86::rax);
                ConditionalTerminal *term = dynamic_cast<ConditionalTerminal *>(block->terminal);
                Reg predicate = unwrapValue(term->predicate);
                code.mov(x86::rax, predicate.get());
                regAlloc.emitDeinitStub();
                regAlloc.finalize();
                regAlloc.reset();
                code.cmp(x86::rax, 0);

                auto it = table.find(term->pass->location);
                if (it != table.end()) {
                    code.je(it->second);
                } else {
                    Label pass = getOrCreateLabel(term->pass->id);
                    code.je(pass);
                    todo.push(term->pass);
                }

                it = table.find(term->fail->location);
                if (it != table.end()) {
                    code.jmp(it->second);
                } else {
                    Label fail = getOrCreateLabel(term->fail->id);
                    code.jmp(fail);
                    todo.push(term->fail);
                }

                break;
            }
        }
    }
    code.bind(exitLabel);
    emitEpilouge();
    emitSubroutines();
}

void X86Emitter::emitMethodCall(void *func, asmjit::x86::Gp arg0, asmjit::x86::Gp arg1) {
    for (int i = 0; i < CALLER_SAVE_REGS.size(); ++i) {
        code.push(CALLER_SAVE_REGS[i]);
    }
    code.sub(x86::rsp, 8);
    code.sub(x86::rsp, SHADOW_SPACE);
    code.mov(PARAM_REGS[0], arg0);
    code.mov(PARAM_REGS[1], arg1);
    code.call(reinterpret_cast<uint64_t>(func));
    code.add(x86::rsp, SHADOW_SPACE);
    code.add(x86::rsp, 8);
    for (int i = CALLER_SAVE_REGS.size() - 1; i >= 0; --i) {
        code.pop(CALLER_SAVE_REGS[i]);
    }
}

void X86Emitter::emitRetMethodCall(void *func, asmjit::x86::Gp arg0, asmjit::x86::Gp ret) {
    bool filtered = false;
    for (int i = 0; i < CALLER_SAVE_REGS.size(); ++i) {
        if (CALLER_SAVE_REGS[i] != ret) {
            code.push(CALLER_SAVE_REGS[i]);
        } else {
            filtered = true;
        }
    }
    if (!filtered)
        code.sub(x86::rsp, 8);
    code.sub(x86::rsp, SHADOW_SPACE);
    code.mov(PARAM_REGS[0], arg0);
    code.call(reinterpret_cast<uint64_t>(func));
    code.mov(ret, x86::rax);
    code.add(x86::rsp, SHADOW_SPACE);
    if (!filtered)
        code.add(x86::rsp, 8);
    for (int i = CALLER_SAVE_REGS.size() - 1; i >= 0; --i) {
        if (CALLER_SAVE_REGS[i] != ret) {
            code.pop(CALLER_SAVE_REGS[i]);
        }
    }
}

void X86Emitter::emitFunctionCall(void *func, asmjit::x86::Gp arg0) {
    for (int i = 0; i < CALLER_SAVE_REGS.size(); ++i) {
        code.push(CALLER_SAVE_REGS[i]);
    }
    code.sub(x86::rsp, 8);
    code.sub(x86::rsp, SHADOW_SPACE);
    code.mov(PARAM_REGS[0], arg0);
    code.call(reinterpret_cast<uint64_t>(func));
    code.add(x86::rsp, SHADOW_SPACE);
    code.add(x86::rsp, 8);
    for (int i = CALLER_SAVE_REGS.size() - 1; i >= 0; --i) {
        code.pop(CALLER_SAVE_REGS[i]);
    }
}

void X86Emitter::emitPrologue() {
    code.push(x86::rbp);
    code.mov(x86::rbp, x86::rsp);
    code.sub(x86::rsp, 8);

    for (int i = 0; i < CALLE_SAVE_REGS.size(); ++i) {
        code.push(CALLE_SAVE_REGS[i]);
    }

    code.mov(x86::rdi, PARAM_REGS[0]);
}

void X86Emitter::emitEpilouge() {
    for (int i = CALLE_SAVE_REGS.size() - 1; i >= 0; --i) {
        code.pop(CALLE_SAVE_REGS[i]);
    }

    code.mov(x86::rsp, x86::rbp);
    code.pop(x86::rbp);
    code.ret();
}

void X86Emitter::emitSubroutines() {
    emitJITRequestImpl();
}

void X86Emitter::emitJITRequest(const Location &location) {
    regAlloc.emitDeinitStub();
    code.mov(x86::rbx, location.pack());
    code.mov(x86::rdx, imm(location.hash()));
    code.jmp(jitRequestLabel);
}

void X86Emitter::emitJITRequestImpl() {
    // rdx: hash of location
    // rbx: packed location
    code.bind(jitRequestLabel);
    x86::Mem patchTable = x86::ptr(x86::rdi, offsetof(RuntimeContext, exhaustPatchTable));
    code.mov(x86::rax, patchTable);
    code.mov(x86::rcx, x86::rdx);
    code.and_(x86::rcx, HASH_TABLE_BEANS - 1);
    code.shl(x86::rcx, HAST_TABLE_BEAN_SHIFT);
    code.lea(x86::rax, x86::ptr(x86::rax, x86::rcx, 0, offsetof(JITHashTable, beans)));
    code.xor_(x86::rcx, x86::rcx);
    const Label loopBegin = code.newLabel();
    const Label jitRequest = code.newLabel();
    code.bind(loopBegin);
    code.mov(x86::r9, x86::qword_ptr(x86::rax, x86::rcx));
    code.mov(x86::r10, x86::qword_ptr(x86::rax, x86::rcx, 0, 8));
    code.mov(x86::r8, x86::r10);
    code.or_(x86::r10, x86::r9);
    code.cmp(x86::r10, 0);
    code.je(jitRequest);
    code.add(x86::rcx, 16);
    code.cmp(x86::r9, x86::rdx);
    code.jne(loopBegin);
    code.jmp(x86::r8);
    code.bind(jitRequest);
    x86::Mem locationPtr = x86::dword_ptr(x86::rdi, offsetof(RuntimeContext, location));
    code.mov(locationPtr, x86::rbx);
    code.jmp(exitLabel);
}

void X86Emitter::visit(Instruction *inst) {
    InstructionVisitorCaller<X86Emitter> caller;
    this->inst = inst;
    caller.call(*this, inst);
}

inline X86Reg X86Emitter::unwrapValue(Value *value) {
    X86Reg out;
    if (value->type() == ValueType::Local) {
        out = regAlloc.allocLocal(dynamic_cast<Local *>(value));
    } else {
        out = regAlloc.allocTmp();
        Constant *cc = dynamic_cast<Constant *>(value);
        code.mov(out.get(), cc->imm);
    }
    return out;
}

void X86Emitter::add(Value *lhs, Value *rhs) {
    X86Reg dest = regAlloc.allocLocal(inst->output);
    X86Reg a = unwrapValue(lhs);
    X86Reg b = unwrapValue(rhs);
    code.mov(dest.get(), a.get());
    code.add(dest.get(), b.get());
}

void X86Emitter::sub(Value *lhs, Value *rhs) {
    X86Reg dest = regAlloc.allocLocal(inst->output);
    X86Reg a = unwrapValue(lhs);
    X86Reg b = unwrapValue(rhs);
    code.mov(dest.get(), a.get());
    code.sub(dest.get(), b.get());
}

void X86Emitter::mul(Value *lhs, Value *rhs) {
    const auto lock = regAlloc.allocSystem(x86::rax);
    const auto lock2 = regAlloc.allocSystem(x86::rdx);
    X86Reg dest = regAlloc.allocLocal(inst->output);
    X86Reg a = unwrapValue(lhs);
    X86Reg b = unwrapValue(rhs);
    code.mov(x86::rax, a.get());
    code.imul(b.get());
    code.mov(dest.get(), x86::rax);
}

void X86Emitter::div(Value *lhs, Value *rhs) {
    const auto lock = regAlloc.allocSystem(x86::rax);
    const auto lock2 = regAlloc.allocSystem(x86::rdx);
    X86Reg dest = regAlloc.allocLocal(inst->output);
    X86Reg a = unwrapValue(lhs);
    X86Reg b = unwrapValue(rhs);
    code.mov(x86::rax, a.get());
    code.cqo(x86::rdx, x86::rax);
    code.idiv(b.get());
    code.mov(dest.get().r32(), x86::eax);
}

void X86Emitter::mod(Value *lhs, Value *rhs) {
    const auto lock = regAlloc.allocSystem(x86::rax);
    const auto lock2 = regAlloc.allocSystem(x86::rdx);
    X86Reg dest = regAlloc.allocLocal(inst->output);
    X86Reg a = unwrapValue(lhs);
    X86Reg b = unwrapValue(rhs);
    code.mov(x86::rax, a.get());
    code.cqo(x86::rdx, x86::rax);
    code.idiv(b.get());
    code.mov(dest.get().r32(), x86::edx);
}

void X86Emitter::cmp(Value *lhs, Value *rhs) {
    X86Reg dest = regAlloc.allocLocal(inst->output);
    X86Reg a = unwrapValue(lhs);
    X86Reg b = unwrapValue(rhs);
    code.xor_(dest.get(), dest.get());
    code.cmp(a.get(), b.get());
    code.setge(dest.get().r8());
}

void X86Emitter::pushStack(Value *value) {
    X86Reg valueReg = unwrapValue(value);
    X86Reg store = regAlloc.allocTmp();
    X86Reg stack = regAlloc.allocTmp();
    code.mov(store.get().r32(), x86::ptr(x86::rdi, offsetof(RuntimeContext, storage)));
    x86::Mem stackPtr = x86::ptr(x86::rdi, store.get(), 3, offsetof(RuntimeContext, stackTops));
    code.mov(stack.get(), stackPtr);
    code.lea(stack.get(), x86::ptr(stack.get(), -8));
    code.mov(x86::ptr(stack.get()), valueReg.get());
    code.mov(stackPtr, stack.get());
}

void X86Emitter::popStack(Void *) {
    X86Reg outputReg = regAlloc.allocLocal(inst->output);
    X86Reg store = regAlloc.allocTmp();
    X86Reg stack = regAlloc.allocTmp();
    code.mov(store.get().r32(), x86::ptr(x86::rdi, offsetof(RuntimeContext, storage)));
    x86::Mem stackPtr = x86::ptr(x86::rdi, store.get(), 3, offsetof(RuntimeContext, stackTops));
    code.mov(stack.get(), stackPtr);
    code.mov(outputReg.get(), x86::ptr(stack.get()));
    code.lea(stack.get(), x86::ptr(stack.get(), 8));
    code.mov(stackPtr, stack.get());
}

void X86Emitter::pushQueueFront(Value *value) {
    X86Reg valueReg = unwrapValue(value);
    x86::Mem queueBackPtr = x86::ptr(x86::rdi, offsetof(RuntimeContext, queueBack));
    X86Reg queueBack = regAlloc.allocTmp();
    X86Reg queueBuffer = regAlloc.allocTmp();
    code.mov(queueBack.get(), queueBackPtr);
    code.mov(queueBuffer.get(), x86::ptr(x86::rdi, offsetof(RuntimeContext, queueBuffer)));
    code.lea(queueBuffer.get(), x86::ptr(queueBuffer.get(), queueBack.get(), 3, 0));
    code.mov(x86::ptr(queueBuffer.get()), valueReg.get());
    code.sub(queueBack.get(), 1);
    code.and_(queueBack.get(), conf.maxStorageSize - 1);
    code.mov(queueBackPtr, queueBack.get());
}

void X86Emitter::pushQueueBack(Value *value) {
    X86Reg valueReg = unwrapValue(value);
    x86::Mem queueFrontPtr = x86::ptr(x86::rdi, offsetof(RuntimeContext, queueFront));
    X86Reg queueFront = regAlloc.allocTmp();
    X86Reg queueBuffer = regAlloc.allocTmp();
    code.mov(queueFront.get(), queueFrontPtr);
    code.mov(queueBuffer.get(), x86::ptr(x86::rdi, offsetof(RuntimeContext, queueBuffer)));
    code.lea(queueBuffer.get(), x86::ptr(queueBuffer.get(), queueFront.get(), 3, 0));
    code.mov(x86::ptr(queueBuffer.get()), valueReg.get());
    code.add(queueFront.get(), 1);
    code.and_(queueFront.get(), conf.maxStorageSize - 1);
    code.mov(queueFrontPtr, queueFront.get());
}

void X86Emitter::popQueue(Void *) {
    X86Reg outputReg = regAlloc.allocLocal(inst->output);
    x86::Mem queueBackPtr = x86::ptr(x86::rdi, offsetof(RuntimeContext, queueBack));
    X86Reg queueBack = regAlloc.allocTmp();
    X86Reg queueBuffer = regAlloc.allocTmp();
    code.mov(queueBack.get(), queueBackPtr);
    code.add(queueBack.get(), 1);
    code.and_(queueBack.get(), conf.maxStorageSize - 1);
    code.mov(queueBuffer.get(), x86::ptr(x86::rdi, offsetof(RuntimeContext, queueBuffer)));
    code.mov(outputReg.get(), x86::ptr(queueBuffer.get(), queueBack.get(), 3, 0));
    code.mov(queueBackPtr, queueBack.get());
}

void X86Emitter::setStore(Value *value) {
    X86Reg valueReg = unwrapValue(value);
    code.mov(x86::ptr(x86::rdi, offsetof(RuntimeContext, storage)), valueReg.get());
}

void X86Emitter::getStore(Void *) {
    X86Reg dest = regAlloc.allocLocal(inst->output);
    code.mov(dest.get(), x86::ptr(x86::rdi, offsetof(RuntimeContext, storage)));
}

void X86Emitter::outputNum(Value *value) {
    X86Reg machinePtr = regAlloc.allocTmp();
    X86Reg valueReg = unwrapValue(value);
    code.mov(machinePtr.get(), x86::ptr(x86::rdi, offsetof(RuntimeContext, machine)));
    emitMethodCall(METHOD_TO_POINTER(X86Machine, printNumImpl, void, Word), machinePtr.get(),
                   valueReg.get());
}

void X86Emitter::outputChar(Value *value) {
    X86Reg machinePtr = regAlloc.allocTmp();
    X86Reg valueReg = unwrapValue(value);
    code.mov(machinePtr.get(), x86::ptr(x86::rdi, offsetof(RuntimeContext, machine)));
    emitMethodCall(METHOD_TO_POINTER(X86Machine, printCharImpl, void, Word), machinePtr.get(),
                   valueReg.get());
}

void X86Emitter::inputNum(Void *) {
    X86Reg machinePtr = regAlloc.allocTmp();
    X86Reg dest = regAlloc.allocLocal(inst->output);
    code.mov(machinePtr.get(), x86::ptr(x86::rdi, offsetof(RuntimeContext, machine)));
    emitRetMethodCall(METHOD_TO_POINTER(X86Machine, inputNumImpl, Word), machinePtr.get(),
                      dest.get());
}

void X86Emitter::inputChar(Void *) {
    X86Reg machinePtr = regAlloc.allocTmp();
    X86Reg dest = regAlloc.allocLocal(inst->output);
    code.mov(machinePtr.get(), x86::ptr(x86::rdi, offsetof(RuntimeContext, machine)));
    emitRetMethodCall(METHOD_TO_POINTER(X86Machine, inputCharImpl, Word), machinePtr.get(),
                      dest.get());
}

void X86Emitter::checkStack1(Void *) {
    Label die = code.newLabel();
    Label end = code.newLabel();
    X86Reg store = regAlloc.allocTmp();
    X86Reg stack = regAlloc.allocTmp();
    X86Reg stackFront = regAlloc.allocTmp();
    code.mov(store.get().r32(), x86::ptr(x86::rdi, offsetof(RuntimeContext, storage)));
    x86::Mem stackPtr = x86::ptr(x86::rdi, store.get(), 3, offsetof(RuntimeContext, stackTops));
    x86::Mem stackUpperPtr =
        x86::ptr(x86::rdi, store.get(), 3, offsetof(RuntimeContext, stackUppers));
    code.mov(stack.get(), stackPtr);
    code.mov(stackFront.get(), stackUpperPtr);
    code.cmp(stack.get(), stackFront.get());
    code.je(die);
    code.jmp(end);
    code.bind(die);
    emitJITRequest(inst->location);
    code.bind(end);
}

void X86Emitter::checkStack2(Void *) {
    Label die = code.newLabel();
    Label end = code.newLabel();
    X86Reg store = regAlloc.allocTmp();
    X86Reg stack = regAlloc.allocTmp();
    X86Reg stackFront = regAlloc.allocTmp();
    code.mov(store.get().r32(), x86::ptr(x86::rdi, offsetof(RuntimeContext, storage)));
    x86::Mem stackPtr = x86::ptr(x86::rdi, store.get(), 3, offsetof(RuntimeContext, stackTops));
    x86::Mem stackUpperPtr =
        x86::ptr(x86::rdi, store.get(), 3, offsetof(RuntimeContext, stackUppers));
    code.mov(stackFront.get(), stackUpperPtr);
    code.mov(stack.get(), stackPtr);
    code.add(stack.get(), 8);
    code.cmp(stack.get(), stackFront.get());
    code.jge(die);
    code.jmp(end);
    code.bind(die);
    emitJITRequest(inst->location);
    code.bind(end);
}

void X86Emitter::checkQueue1(Void *) {
    Label die = code.newLabel();
    Label end = code.newLabel();
    x86::Mem queueFrontPtr = x86::ptr(x86::rdi, offsetof(RuntimeContext, queueFront));
    x86::Mem queueBackPtr = x86::ptr(x86::rdi, offsetof(RuntimeContext, queueBack));
    X86Reg queueFront = regAlloc.allocTmp();
    X86Reg queueBack = regAlloc.allocTmp();
    code.mov(queueFront.get(), queueFrontPtr);
    code.mov(queueBack.get(), queueBackPtr);
    code.add(queueBack.get(), 1);
    code.and_(queueBack.get(), conf.maxStorageSize - 1);
    code.cmp(queueFront.get(), queueBack.get());
    code.je(die);
    code.jmp(end);
    code.bind(die);
    emitJITRequest(inst->location);
    code.bind(end);
}

void X86Emitter::checkQueue2(Void *) {
    Label die = code.newLabel();
    Label end = code.newLabel();
    x86::Mem queueFrontPtr = x86::ptr(x86::rdi, offsetof(RuntimeContext, queueFront));
    x86::Mem queueBackPtr = x86::ptr(x86::rdi, offsetof(RuntimeContext, queueBack));
    X86Reg queueFront = regAlloc.allocTmp();
    X86Reg queueBack = regAlloc.allocTmp();
    code.mov(queueFront.get(), queueFrontPtr);
    code.mov(queueBack.get(), queueBackPtr);
    code.add(queueBack.get(), 1);
    code.and_(queueBack.get(), conf.maxStorageSize - 1);
    code.cmp(queueFront.get(), queueBack.get());
    code.je(die);
    code.add(queueBack.get(), 1);
    code.and_(queueBack.get(), conf.maxStorageSize - 1);
    code.cmp(queueFront.get(), queueBack.get());
    code.je(die);
    code.jmp(end);
    code.bind(die);
    emitJITRequest(inst->location);
    code.bind(end);
}
