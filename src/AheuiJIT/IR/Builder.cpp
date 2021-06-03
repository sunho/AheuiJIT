#include "Builder.h"

Instruction* Builder::createInstruction(Opcode opcode) {
    Instruction* inst = new Instruction(opcode);
    currentBlock->insts.push_back(inst);
    return inst;
}

RegValue* Builder::add(Value* lhs, Value* rhs) {
    RegValue* output = createLocal();
    Instruction* inst = createInstruction(Opcode::add);
    inst->args[0] = lhs;
    inst->args[1] = rhs;
    inst->output = output;
    return output;
}

