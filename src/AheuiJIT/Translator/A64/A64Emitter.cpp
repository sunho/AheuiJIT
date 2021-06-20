#include "A64Emitter.h"

#include <AheuiJIT/IR/Value.h>
#include <AheuiJIT/Util/Util.h>
#include <fmt/printf.h>

#include <stack>

using namespace asmjit;
using namespace aheuijit;

std::array<arm::Gp, 18> CALLER_SAVE_REGS = { a64::x0,  a64::x1,  a64::x2,  a64::x3,  a64::x4,
                                             a64::x5,  a64::x6,  a64::x7,  a64::x8,  a64::x9,
                                             a64::x10, a64::x11, a64::x12, a64::x13, a64::x14,
                                             a64::x15, a64::x16, a64::x17 };

std::array<arm::Gp, 12> CALLE_SAVE_REGS = { a64::x19, a64::x20, a64::x21, a64::x22,
                                            a64::x23, a64::x24, a64::x25, a64::x26,
                                            a64::x27, a64::x28, a64::x29, a64::x30 };

void A64Emitter::emit(BasicBlock *bb, const TLBTable &table, std::set<BasicBlock *> &emitted) {
    emitPrologue();
    std::stack<BasicBlock *> todo;
    std::set<uint64_t> done;
    exitLabel = code.newLabel();
    jitRequestLabel = code.newLabel();

    if (table.find(bb->location) != table.end()) {
        code.b(table.at(bb->location));
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

        const auto lock = regAlloc.allocSystem(a64::x19);
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
                code.b(exitLabel);
                break;
            case TerminalType::Link: {
                regAlloc.emitDeinitStub();
                regAlloc.finalize();
                regAlloc.reset();
                LinkTerminal *term = dynamic_cast<LinkTerminal *>(block->terminal);
                const auto it = table.find(term->block->location);
                if (it != table.end()) {
                    code.b(it->second);
                } else {
                    const Label label_ = getOrCreateLabel(term->block->id);
                    code.b(label_);
                    todo.push(term->block);
                }
                break;
            }
            case TerminalType::Conditional: {
                const auto lock2 = regAlloc.allocSystem(a64::x0);
                ConditionalTerminal *term = dynamic_cast<ConditionalTerminal *>(block->terminal);
                Reg predicate = unwrapValue(term->predicate);
                code.mov(a64::x0, predicate.get());
                regAlloc.emitDeinitStub();
                regAlloc.finalize();
                regAlloc.reset();

                auto it = table.find(term->pass->location);
                if (it != table.end()) {
                    code.cbz(a64::x0, it->second);
                } else {
                    Label pass = getOrCreateLabel(term->pass->id);
                    code.cbz(a64::x0, pass);
                    todo.push(term->pass);
                }

                it = table.find(term->fail->location);
                if (it != table.end()) {
                    code.b(it->second);
                } else {
                    Label fail = getOrCreateLabel(term->fail->id);
                    code.b(fail);
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

void A64Emitter::emitMethodCall(void *func, asmjit::arm::Gp arg0, asmjit::arm::Gp arg1) {
    for (int i = 0; i < CALLER_SAVE_REGS.size(); i += 2) {
        code.stp(CALLER_SAVE_REGS[i], CALLER_SAVE_REGS[i + 1], a64::ptr(a64::sp).pre(-16));
    }
    code.mov(a64::x0, arg0);
    code.mov(a64::x1, arg1);
    code.mov(a64::x17, reinterpret_cast<uint64_t>(func));
    code.blr(a64::x17);
    for (int i = CALLER_SAVE_REGS.size() - 2; i >= 0; i -= 2) {
        code.ldp(CALLER_SAVE_REGS[i], CALLER_SAVE_REGS[i + 1], a64::ptr(a64::sp).post(16));
    }
}

void A64Emitter::emitRetMethodCall(void *func, asmjit::arm::Gp arg0, asmjit::arm::Gp ret) {
    for (int i = 0; i < CALLER_SAVE_REGS.size(); i += 2) {
        code.stp(CALLER_SAVE_REGS[i], CALLER_SAVE_REGS[i + 1], a64::ptr(a64::sp).pre(-16));
    }
    code.mov(a64::x0, arg0);
    code.mov(a64::x17, reinterpret_cast<uint64_t>(func));
    code.blr(a64::x17);
    code.mov(ret, a64::x0);
    for (int i = CALLER_SAVE_REGS.size() - 2; i >= 0; --i) {
        auto first = CALLER_SAVE_REGS[i];
        auto second = CALLER_SAVE_REGS[i + 1];
        if (first == ret) {
            first = a64::xzr;
        }
        if (second == ret) {
            second = a64::xzr;
        }
        code.ldp(first, second, a64::ptr(a64::sp).post(16));
    }
}

void A64Emitter::emitPrologue() {
    for (int i = 0; i < CALLE_SAVE_REGS.size(); i += 2) {
        code.stp(CALLE_SAVE_REGS[i], CALLE_SAVE_REGS[i + 1], a64::ptr(a64::sp).pre(-16));
    }
    code.mov(a64::x19, a64::x0);
}

void A64Emitter::emitEpilouge() {
    for (int i = CALLE_SAVE_REGS.size() - 2; i >= 0; i -= 2) {
        code.ldp(CALLE_SAVE_REGS[i], CALLE_SAVE_REGS[i + 1], a64::ptr(a64::sp).post(16));
    }
    code.ret(a64::x30);
    code.nop();
}

void A64Emitter::emitSubroutines() {
    emitJITRequestImpl();
}

void A64Emitter::emitJITRequest(const Location &location) {
    regAlloc.emitDeinitStub();
    code.mov(a64::x0, location.hash());
    code.mov(a64::x1, location.pack());
    code.b(jitRequestLabel);
}

void A64Emitter::emitJITRequestImpl() {
    // x0: hash of location
    // x1: packed location
    code.bind(jitRequestLabel);
    a64::Mem patchTable = a64::ptr(a64::x19, offsetof(RuntimeContext, exhaustPatchTable));
    code.ldr(a64::x2, patchTable);
    code.and_(a64::x3, a64::x0, HASH_TABLE_BEANS - 1);
    code.orr(a64::x3, a64::xzr, a64::x3, a64::lsl(HAST_TABLE_BEAN_SHIFT));
    code.add(a64::x3, a64::x3, a64::x2);
    code.add(a64::x3, a64::x3, offsetof(JITHashTable, beans));  // TODO: eliminate this
    code.mov(a64::x2, 0);
    const Label loopBegin = code.newLabel();
    const Label jitRequest = code.newLabel();
    code.bind(loopBegin);
    code.ldr(a64::x4, a64::ptr(a64::x3));     // key
    code.ldr(a64::x5, a64::ptr(a64::x3, 8));  // value
    code.cbz(a64::x4, jitRequest);
    code.add(a64::x3, a64::x3, 16);
    code.eor(a64::x4, a64::x0, a64::x4);
    code.cbnz(a64::x4, loopBegin);
    code.blr(a64::x5);
    code.bind(jitRequest);
    code.str(a64::x1, a64::ptr(a64::x19, offsetof(RuntimeContext, location)));
    code.b(exitLabel);
}

void A64Emitter::visit(Instruction *inst) {
    InstructionVisitorCaller<A64Emitter> caller;
    this->inst = inst;
    caller.call(*this, inst);
}

inline A64Reg A64Emitter::unwrapValue(Value *value) {
    A64Reg out;
    if (value->type() == ValueType::Local) {
        out = regAlloc.allocLocal(dynamic_cast<Local *>(value));
    } else {
        out = regAlloc.allocTmp();
        Constant *cc = dynamic_cast<Constant *>(value);
        code.mov(out.get(), cc->imm);
    }
    return out;
}

void A64Emitter::add(Value *lhs, Value *rhs) {
    A64Reg dest = regAlloc.allocLocal(inst->output);
    A64Reg a = unwrapValue(lhs);
    A64Reg b = unwrapValue(rhs);
    code.add(dest.get(), a.get(), b.get());
}

void A64Emitter::sub(Value *lhs, Value *rhs) {
    A64Reg dest = regAlloc.allocLocal(inst->output);
    A64Reg a = unwrapValue(lhs);
    A64Reg b = unwrapValue(rhs);
    code.sub(dest.get(), a.get(), b.get());
}

void A64Emitter::mul(Value *lhs, Value *rhs) {
    A64Reg dest = regAlloc.allocLocal(inst->output);
    A64Reg a = unwrapValue(lhs);
    A64Reg b = unwrapValue(rhs);
    code.mul(dest.get(), a.get(), b.get());
}

void A64Emitter::div(Value *lhs, Value *rhs) {
    A64Reg dest = regAlloc.allocLocal(inst->output);
    A64Reg a = unwrapValue(lhs);
    A64Reg b = unwrapValue(rhs);
    code.sdiv(dest.get(), a.get(), b.get());
}

void A64Emitter::mod(Value *lhs, Value *rhs) {
    A64Reg tmp = regAlloc.allocTmp();
    A64Reg dest = regAlloc.allocLocal(inst->output);
    A64Reg a = unwrapValue(lhs);
    A64Reg b = unwrapValue(rhs);
    code.sdiv(tmp.get(), a.get(), b.get());
    code.mul(tmp.get(), tmp.get(), b.get());
    code.sub(dest.get(), a.get(), tmp.get());
}

void A64Emitter::cmp(Value *lhs, Value *rhs) {
    A64Reg dest = regAlloc.allocLocal(inst->output);
    A64Reg a = unwrapValue(lhs);
    A64Reg b = unwrapValue(rhs);
    code.mov(dest.get(), 0);
    code.cmp(a.get(), b.get());
    code.cond_ge().mov(dest.get(), 1);
}

void A64Emitter::pushStack(Value *value) {
    A64Reg valueReg = unwrapValue(value);
    A64Reg store = regAlloc.allocTmp();
    A64Reg stack = regAlloc.allocTmp();
    A64Reg stackPtr = regAlloc.allocTmp();
    code.ldr(store.get().w(), a64::ptr(a64::x19, offsetof(RuntimeContext, storage)));
    code.add(stackPtr.get(), a64::x19, store.get(), a64::lsl(3));
    code.ldr(stack.get(), a64::ptr(stackPtr.get(), offsetof(RuntimeContext, stackTops)));
    code.str(valueReg.get(), a64::ptr(stack.get()).pre(-8));
    code.str(stack.get(), a64::ptr(stackPtr.get(), offsetof(RuntimeContext, stackTops)));
}

void A64Emitter::popStack(Void *) {
    A64Reg outputReg = regAlloc.allocLocal(inst->output);
    A64Reg store = regAlloc.allocTmp();
    A64Reg stack = regAlloc.allocTmp();
    A64Reg stackPtr = regAlloc.allocTmp();
    code.ldr(store.get().w(), a64::ptr(a64::x19, offsetof(RuntimeContext, storage)));
    code.add(stackPtr.get(), a64::x19, store.get(), a64::lsl(3));
    code.ldr(stack.get(), a64::ptr(stackPtr.get(), offsetof(RuntimeContext, stackTops)));
    code.ldr(outputReg.get(), a64::ptr(stack.get()).post(8));
    code.str(stack.get(), a64::ptr(stackPtr.get(), offsetof(RuntimeContext, stackTops)));
}

void A64Emitter::pushQueueFront(Value *value) {
    A64Reg valueReg = unwrapValue(value);
    A64Reg queueBack = regAlloc.allocTmp();
    A64Reg queuBuffer = regAlloc.allocTmp();
    code.ldr(queueBack.get(), a64::ptr(a64::x19, offsetof(RuntimeContext, queueBack)));
    code.ldr(queuBuffer.get(), a64::ptr(a64::x19, offsetof(RuntimeContext, queueBuffer)));
    code.add(queuBuffer.get(), queuBuffer.get(), queueBack.get(), a64::lsl(3));
    code.str(valueReg.get(), a64::ptr(queuBuffer.get()));
    code.sub(queueBack.get(), queueBack.get(), 1);
    code.and_(queueBack.get(), queueBack.get(), conf.maxStorageSize - 1);
    code.str(queueBack.get(), a64::ptr(a64::x19, offsetof(RuntimeContext, queueBack)));
}

void A64Emitter::pushQueueBack(Value *value) {
    A64Reg valueReg = unwrapValue(value);
    A64Reg queueFront = regAlloc.allocTmp();
    A64Reg queuBuffer = regAlloc.allocTmp();
    code.ldr(queueFront.get(), a64::ptr(a64::x19, offsetof(RuntimeContext, queueFront)));
    code.ldr(queuBuffer.get(), a64::ptr(a64::x19, offsetof(RuntimeContext, queueBuffer)));
    code.add(queuBuffer.get(), queuBuffer.get(), queueFront.get(), a64::lsl(3));
    code.str(valueReg.get(), a64::ptr(queuBuffer.get()));
    code.add(queueFront.get(), queueFront.get(), 1);
    code.and_(queueFront.get(), queueFront.get(), conf.maxStorageSize - 1);
    code.str(queueFront.get(), a64::ptr(a64::x19, offsetof(RuntimeContext, queueFront)));
}

void A64Emitter::popQueue(Void *) {
    A64Reg dest = regAlloc.allocLocal(inst->output);
    A64Reg queueBack = regAlloc.allocTmp();
    A64Reg queuBuffer = regAlloc.allocTmp();
    code.ldr(queueBack.get(), a64::ptr(a64::x19, offsetof(RuntimeContext, queueBack)));
    code.add(queueBack.get(), queueBack.get(), 1);
    code.and_(queueBack.get(), queueBack.get(), conf.maxStorageSize - 1);
    code.ldr(queuBuffer.get(), a64::ptr(a64::x19, offsetof(RuntimeContext, queueBuffer)));
    code.add(queuBuffer.get(), queuBuffer.get(), queueBack.get(), a64::lsl(3));
    code.ldr(dest.get(), a64::ptr(queuBuffer.get()));
    code.str(queueBack.get(), a64::ptr(a64::x19, offsetof(RuntimeContext, queueBack)));
}

void A64Emitter::setStore(Value *value) {
    A64Reg valueReg = unwrapValue(value);
    code.str(valueReg.get().w(), a64::ptr(a64::x19, offsetof(RuntimeContext, storage)));
}

void A64Emitter::getStore(Void *) {
    A64Reg dest = regAlloc.allocLocal(inst->output);
    code.ldr(dest.get().w(), a64::ptr(a64::x19, offsetof(RuntimeContext, storage)));
}

void A64Emitter::outputNum(Value *value) {
    A64Reg machinePtr = regAlloc.allocTmp();
    A64Reg valueReg = unwrapValue(value);
    code.ldr(machinePtr.get(), a64::ptr(a64::x19, offsetof(RuntimeContext, machine)));
    emitMethodCall(METHOD_TO_POINTER(A64Machine, printNumImpl, void, Word), machinePtr.get(),
                   valueReg.get());
}

void A64Emitter::outputChar(Value *value) {
    A64Reg machinePtr = regAlloc.allocTmp();
    A64Reg valueReg = unwrapValue(value);
    code.ldr(machinePtr.get(), a64::ptr(a64::x19, offsetof(RuntimeContext, machine)));
    emitMethodCall(METHOD_TO_POINTER(A64Machine, printCharImpl, void, Word), machinePtr.get(),
                   valueReg.get());
}

void A64Emitter::inputNum(Void *) {
    A64Reg machinePtr = regAlloc.allocTmp();
    A64Reg dest = regAlloc.allocLocal(inst->output);
    code.ldr(machinePtr.get(), a64::ptr(a64::x19, offsetof(RuntimeContext, machine)));
    emitRetMethodCall(METHOD_TO_POINTER(A64Machine, inputNumImpl, Word), machinePtr.get(),
                      dest.get());
}

void A64Emitter::inputChar(Void *) {
    A64Reg machinePtr = regAlloc.allocTmp();
    A64Reg dest = regAlloc.allocLocal(inst->output);
    code.ldr(machinePtr.get(), a64::ptr(a64::x19, offsetof(RuntimeContext, machine)));
    emitRetMethodCall(METHOD_TO_POINTER(A64Machine, inputCharImpl, Word), machinePtr.get(),
                      dest.get());
}

void A64Emitter::checkStack1(Void *) {
    Label die = code.newLabel();
    Label end = code.newLabel();
    A64Reg stackPtr = regAlloc.allocTmp();
    A64Reg stack = regAlloc.allocTmp();
    A64Reg stackFront = regAlloc.allocTmp();
    code.ldr(stackPtr.get().w(), a64::ptr(a64::x19, offsetof(RuntimeContext, storage)));
    code.add(stackPtr.get(), a64::x19, stackPtr.get(), a64::lsl(3));
    code.ldr(stack.get(), a64::ptr(stackPtr.get(), offsetof(RuntimeContext, stackTops)));
    code.ldr(stackFront.get(), a64::ptr(stackPtr.get(), offsetof(RuntimeContext, stackUppers)));
    code.cmp(stack.get(), stackFront.get());
    code.cond_eq().b(die);
    code.b(end);
    code.bind(die);
    emitJITRequest(inst->location);
    code.bind(end);
}

void A64Emitter::checkStack2(Void *) {
    Label die = code.newLabel();
    Label end = code.newLabel();
    A64Reg stackPtr = regAlloc.allocTmp();
    A64Reg stack = regAlloc.allocTmp();
    A64Reg stackFront = regAlloc.allocTmp();
    code.ldr(stackPtr.get().w(), a64::ptr(a64::x19, offsetof(RuntimeContext, storage)));
    code.add(stackPtr.get(), a64::x19, stackPtr.get(), a64::lsl(3));
    code.ldr(stack.get(), a64::ptr(stackPtr.get(), offsetof(RuntimeContext, stackTops)));
    code.ldr(stackFront.get(), a64::ptr(stackPtr.get(), offsetof(RuntimeContext, stackUppers)));
    code.add(stack.get(), stack.get(), 8);
    code.cmp(stack.get(), stackFront.get());
    code.cond_ge().b(die);
    code.b(end);
    code.bind(die);
    emitJITRequest(inst->location);
    code.bind(end);
}

void A64Emitter::checkQueue1(Void *) {
    Label die = code.newLabel();
    Label end = code.newLabel();
    A64Reg queueBack = regAlloc.allocTmp();
    A64Reg queueFront = regAlloc.allocTmp();
    code.ldr(queueBack.get(), a64::ptr(a64::x19, offsetof(RuntimeContext, queueBack)));
    code.add(queueBack.get(), queueBack.get(), 1);
    code.and_(queueBack.get(), queueBack.get(), conf.maxStorageSize - 1);
    code.ldr(queueFront.get(), a64::ptr(a64::x19, offsetof(RuntimeContext, queueFront)));
    code.cmp(queueBack.get(), queueFront.get());
    code.cond_eq().b(die);
    code.b(end);
    code.bind(die);
    emitJITRequest(inst->location);
    code.bind(end);
}

void A64Emitter::checkQueue2(Void *) {
    Label die = code.newLabel();
    Label end = code.newLabel();
    A64Reg queueFront = regAlloc.allocTmp();
    A64Reg queueBack = regAlloc.allocTmp();
    code.ldr(queueFront.get(), a64::ptr(a64::x19, offsetof(RuntimeContext, queueFront)));
    code.ldr(queueBack.get(), a64::ptr(a64::x19, offsetof(RuntimeContext, queueBack)));
    code.add(queueBack.get(), queueBack.get(), 1);
    code.and_(queueBack.get(), queueBack.get(), conf.maxStorageSize - 1);
    code.cmp(queueBack.get(), queueFront.get());
    code.cond_eq().b(die);
    code.add(queueBack.get(), queueBack.get(), 1);
    code.and_(queueBack.get(), queueBack.get(), conf.maxStorageSize - 1);
    code.cmp(queueBack.get(), queueFront.get());
    code.cond_eq().b(die);
    code.b(end);
    code.bind(die);
    emitJITRequest(inst->location);
    code.bind(end);
}
