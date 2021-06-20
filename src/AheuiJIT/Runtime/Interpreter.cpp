#include "Interpreter.h"

#include <AheuiJIT/IR/InstructionVisitor.h>
#include <AheuiJIT/Runtime/Runtime.h>
#include <fmt/printf.h>

using namespace aheuijit;

Location Interpreter::run(const Location &location, RuntimeContext *ctx, IRBuffer &irBuffer,
                          size_t &numCycle) {
    this->ctx = ctx;
    shouldFail = false;

    Location pc = location;
    InstructionFormatter formatter;

    while (numCycle && pc != Location()) {
        BasicBlock *block = irBuffer.findBlock(pc);
        ASSERT(block)
        for (auto inst : block->insts) {
            if (conf.logStep) {
                fmt::print("{}: {}\n", inst->location.description(), formatter.format(inst));
            }
            visit(inst);
            if (shouldFail)
                return pc;
        }
        const size_t deltaCycle = block->insts.size();
        if (deltaCycle > numCycle) {
            numCycle = 0;
        } else {
            numCycle -= deltaCycle;
        }
        switch (block->terminal->getType()) {
            case TerminalType::Exit:
                pc = Location();
                break;
            case TerminalType::Link: {
                LinkTerminal *term = dynamic_cast<LinkTerminal *>(block->terminal);
                pc = term->block->location;
                break;
            }
            case TerminalType::Conditional: {
                ConditionalTerminal *term = dynamic_cast<ConditionalTerminal *>(block->terminal);
                if (unwrapValue(term->predicate) == 0) {
                    pc = term->pass->location;
                } else {
                    pc = term->fail->location;
                }
                break;
            }
        }
    }
    return pc;
}

void Interpreter::setConfig(RuntimeConfig conf) {
    this->conf = conf;
}

Word Interpreter::unwrapValue(Value *value) {
    if (value->type() == ValueType::Local) {
        return localBank[dynamic_cast<Local *>(value)->id];
    } else {
        return dynamic_cast<Constant *>(value)->imm;
    }
}

void Interpreter::visit(Instruction *inst) {
    InstructionVisitorCaller<Interpreter> caller;
    this->inst = inst;
    caller.call(*this, inst);
}

void Interpreter::add(Value *lhs, Value *rhs) {
    Word a = unwrapValue(lhs);
    Word b = unwrapValue(rhs);
    localBank[inst->output->id] = a + b;
}

void Interpreter::sub(Value *lhs, Value *rhs) {
    Word a = unwrapValue(lhs);
    Word b = unwrapValue(rhs);
    localBank[inst->output->id] = a - b;
}

void Interpreter::mul(Value *lhs, Value *rhs) {
    Word a = unwrapValue(lhs);
    Word b = unwrapValue(rhs);
    localBank[inst->output->id] = a * b;
}

void Interpreter::div(Value *lhs, Value *rhs) {
    Word a = unwrapValue(lhs);
    Word b = unwrapValue(rhs);
    localBank[inst->output->id] = a / b;
}

void Interpreter::mod(Value *lhs, Value *rhs) {
    Word a = unwrapValue(lhs);
    Word b = unwrapValue(rhs);
    localBank[inst->output->id] = a % b;
}

void Interpreter::cmp(Value *lhs, Value *rhs) {
    Word a = unwrapValue(lhs);
    Word b = unwrapValue(rhs);
    localBank[inst->output->id] = a >= b;
}

void Interpreter::pushStack(Value *value) {
    uintptr_t stackTop = ctx->stackTops[ctx->storage];
    stackTop -= 8;
    *reinterpret_cast<Word *>(stackTop) = unwrapValue(value);
    ctx->stackTops[ctx->storage] = stackTop;
}

void Interpreter::popStack(Void *) {
    uintptr_t stackTop = ctx->stackTops[ctx->storage];
    localBank[inst->output->id] = *reinterpret_cast<Word *>(stackTop);
    stackTop += 8;
    ctx->stackTops[ctx->storage] = stackTop;
}

void Interpreter::pushQueueFront(Value *value) {
    uint64_t queueBack = ctx->queueBack;
    uintptr_t queueBackPtr = ctx->queueBuffer + queueBack * 8;
    *reinterpret_cast<Word *>(queueBackPtr) = unwrapValue(value);
    queueBack -= 1;
    queueBack %= conf.maxStorageSize;
    ctx->queueBack = queueBack;
}

void Interpreter::pushQueueBack(Value *value) {
    uint64_t queueFront = ctx->queueFront;
    uintptr_t queueFrontPtr = ctx->queueBuffer + queueFront * 8;
    *reinterpret_cast<Word *>(queueFrontPtr) = unwrapValue(value);
    queueFront += 1;
    queueFront %= conf.maxStorageSize;
    ctx->queueFront = queueFront;
}

void Interpreter::popQueue(Void *) {
    uint64_t queueBack = ctx->queueBack;
    queueBack += 1;
    queueBack %= conf.maxStorageSize;
    uintptr_t queueBackPtr = ctx->queueBuffer + queueBack * 8;
    localBank[inst->output->id] = *reinterpret_cast<Word *>(queueBackPtr);
    ctx->queueBack = queueBack;
}

void Interpreter::setStore(Value *value) {
    ctx->storage = unwrapValue(value);
}

void Interpreter::getStore(Void *) {
    localBank[inst->output->id] = ctx->storage;
}

void Interpreter::outputNum(Value *value) {
    ctx->machine->printNum(unwrapValue(value));
}

void Interpreter::outputChar(Value *value) {
    ctx->machine->printChar(unwrapValue(value));
}

void Interpreter::inputNum(Void *) {
    localBank[inst->output->id] = ctx->machine->inputNum();
}

void Interpreter::inputChar(Void *) {
    localBank[inst->output->id] = ctx->machine->inputChar();
}

void Interpreter::checkStack1(Void *) {
    uintptr_t stackTop = ctx->stackTops[ctx->storage];
    uintptr_t stackUpper = ctx->stackUppers[ctx->storage];
    if (stackTop == stackUpper) {
        shouldFail = true;
        ctx->location = inst->location.pack();
    }
}

void Interpreter::checkStack2(Void *) {
    uintptr_t stackTop = ctx->stackTops[ctx->storage] + 8;
    uintptr_t stackUpper = ctx->stackUppers[ctx->storage];
    if (stackTop >= stackUpper) {
        shouldFail = true;
        ctx->location = inst->location.pack();
    }
}

void Interpreter::checkQueue1(Void *) {
    uint64_t queueFront = ctx->queueFront;
    uint64_t queueBack = ctx->queueBack;
    queueBack += 1;
    queueBack %= conf.maxStorageSize;
    if (queueFront == queueBack) {
        shouldFail = true;
        ctx->location = inst->location.pack();
    }
}

void Interpreter::checkQueue2(Void *) {
    uint64_t queueFront = ctx->queueFront;
    uint64_t queueBack = ctx->queueBack;
    queueBack += 1;
    queueBack %= conf.maxStorageSize;
    if (queueFront == queueBack) {
        shouldFail = true;
        ctx->location = inst->location.pack();
        return;
    }
    queueBack += 1;
    queueBack %= conf.maxStorageSize;
    if (queueFront == queueBack) {
        shouldFail = true;
        ctx->location = inst->location.pack();
    }
}
