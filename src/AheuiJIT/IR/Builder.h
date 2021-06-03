#include <AheuiJIT/IR/BasicBlock.h>
#include <AheuiJIT/IR/Value.h>
#include <AheuiJIT/IR/Instruction.h>

struct Builder {
    BasicBlock* currentBlock;
    ConstantValue* createConstant(int imm);
    RegValue* createLocal();
    Instruction* createInstruction(Opcode opcode);

    #define INST(name, ret, ...) ret name(__VA_ARGS__);
    #include "Instruction.inc"
    #undef INST
};

