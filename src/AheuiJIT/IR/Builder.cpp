#include "Builder.h"

#include <AheuiJIT/Util/Util.h>

using namespace aheuijit;

Builder::Builder() {
}

void Builder::setCurrentLocation(const Location &location) {
    cur = location;
}

// TODO: I can pool this resources
Local *Builder::createLocal() {
    Local *value = new Local(nextId++);
    currentBlock->values.push_back(value);
    currentBlock->ownValue(value);
    return value;
}

Constant *Builder::createConstant(int imm) {
    Constant *value = new Constant(imm);
    currentBlock->values.push_back(value);
    currentBlock->ownValue(value);
    return value;
}

Instruction *Builder::createInstruction(Opcode opcode) {
    Instruction *inst = new Instruction(opcode, currentBlock->insts.size(), cur);
    currentBlock->insts.push_back(inst);
    currentBlock->ownInstruction(inst);
    return inst;
}

void Builder::setBasicBlock(BasicBlock *bb) {
    currentBlock = bb;
}

void Builder::setLinkTerminal(BasicBlock *block) {
    currentBlock->terminal = new LinkTerminal(block);
}

void Builder::setExitTerminal() {
    currentBlock->terminal = new ExitTerminal();
}

void Builder::setConditionalTerminal(Value *predicate) {
    currentBlock->terminal = new ConditionalTerminal(predicate);
}

Local *Builder::add(Value *lhs, Value *rhs) {
    Local *output = createLocal();
    Instruction *inst = createInstruction(Opcode::add);
    inst->args[0] = lhs;
    inst->args[1] = rhs;
    inst->output = output;
    return output;
}

Local *Builder::sub(Value *lhs, Value *rhs) {
    Local *output = createLocal();
    Instruction *inst = createInstruction(Opcode::sub);
    inst->args[0] = lhs;
    inst->args[1] = rhs;
    inst->output = output;
    return output;
}

Local *Builder::div(Value *lhs, Value *rhs) {
    Local *output = createLocal();
    Instruction *inst = createInstruction(Opcode::div);
    inst->args[0] = lhs;
    inst->args[1] = rhs;
    inst->output = output;
    return output;
}

Local *Builder::mul(Value *lhs, Value *rhs) {
    Local *output = createLocal();
    Instruction *inst = createInstruction(Opcode::mul);
    inst->args[0] = lhs;
    inst->args[1] = rhs;
    inst->output = output;
    return output;
}

Local *Builder::mod(Value *lhs, Value *rhs) {
    Local *output = createLocal();
    Instruction *inst = createInstruction(Opcode::mod);
    inst->args[0] = lhs;
    inst->args[1] = rhs;
    inst->output = output;
    return output;
}

Local *Builder::cmp(Value *lhs, Value *rhs) {
    Local *output = createLocal();
    Instruction *inst = createInstruction(Opcode::cmp);
    inst->args[0] = lhs;
    inst->args[1] = rhs;
    inst->output = output;
    return output;
}

void Builder::setStore(Value *value) {
    Instruction *inst = createInstruction(Opcode::setStore);
    inst->args[0] = value;
}

Local *Builder::getStore(Void *) {
    Local *output = createLocal();
    Instruction *inst = createInstruction(Opcode::getStore);
    inst->output = output;
    return output;
}

Local *Builder::inputNum(Void *) {
    Local *output = createLocal();
    Instruction *inst = createInstruction(Opcode::inputNum);
    inst->output = output;
    return output;
}

Local *Builder::inputChar(Void *) {
    Local *output = createLocal();
    Instruction *inst = createInstruction(Opcode::inputChar);
    inst->output = output;
    return output;
}

void Builder::outputNum(Value *value) {
    Instruction *inst = createInstruction(Opcode::outputNum);
    inst->args[0] = value;
}

void Builder::outputChar(Value *value) {
    Instruction *inst = createInstruction(Opcode::outputChar);
    inst->args[0] = value;
}

Local *Builder::pop(bool queue) {
    if (queue) {
        return popQueue(nullptr);
    }
    return popStack(nullptr);
}

void Builder::push(bool queue, Value *value) {
    if (queue) {
        pushQueueBack(value);
    } else {
        pushStack(value);
    }
}

void Builder::pushStack(Value *value) {
    Instruction *inst = createInstruction(Opcode::pushStack);
    inst->args[0] = value;
}

void Builder::pushQueueBack(Value *value) {
    Instruction *inst = createInstruction(Opcode::pushQueueBack);
    inst->args[0] = value;
}

void Builder::pushQueueFront(Value *value) {
    Instruction *inst = createInstruction(Opcode::pushQueueFront);
    inst->args[0] = value;
}

Local *Builder::popStack(Void *) {
    Local *output = createLocal();
    Instruction *inst = createInstruction(Opcode::popStack);
    inst->output = output;
    return output;
}

Local *Builder::popQueue(Void *) {
    Local *output = createLocal();
    Instruction *inst = createInstruction(Opcode::popQueue);
    inst->output = output;
    return output;
}

void Builder::checkStack1(Void *) {
    createInstruction(Opcode::checkStack1);
}

void Builder::checkStack2(Void *) {
    createInstruction(Opcode::checkStack2);
}

void Builder::checkQueue1(Void *) {
    createInstruction(Opcode::checkQueue1);
}

void Builder::checkQueue2(Void *) {
    createInstruction(Opcode::checkQueue2);
}

void Builder::checkStorage(bool queue, int n) {
    if (queue && n == 1) {
        checkQueue1(nullptr);
    } else if (queue && n == 2) {
        checkQueue2(nullptr);
    } else if (!queue && n == 1) {
        checkStack1(nullptr);
    } else if (!queue && n == 2) {
        checkStack2(nullptr);
    } else {
        ASSERT(false)
    }
}
