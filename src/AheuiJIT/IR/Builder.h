#pragma once

#include <AheuiJIT/IR/BasicBlock.h>
#include <AheuiJIT/IR/Instruction.h>
#include <AheuiJIT/IR/InstructionVisitor.h>
#include <AheuiJIT/IR/Value.h>

#include <array>

namespace aheuijit {

struct Builder {
    Builder();
    ~Builder() = default;

    void setBasicBlock(BasicBlock *bb);

    Constant *createConstant(int imm);
    Local *createLocal();
    Instruction *createInstruction(Opcode opcode);
    void setCurrentLocation(const Location &location);
    void setLinkTerminal(BasicBlock *block);
    void setExitTerminal();
    void setConditionalTerminal(Value *predicate);

    Local *pop(bool queue);
    void push(bool queue, Value *value);
#define INST(name, ret, ...) ret name(__VA_ARGS__);
#include "Instruction.inc"
#undef INST

  private:
    BasicBlock *currentBlock;
    Location cur;
    uint64_t nextId{ 1 };
};

}  // namespace aheuijit
