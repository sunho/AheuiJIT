#pragma once

#include <AheuiJIT/IR/Instruction.h>
#include <AheuiJIT/Runtime/IRBuffer.h>
#include <AheuiJIT/Runtime/RuntimeContext.h>

namespace aheuijit {

struct Interpreter {
    Interpreter() = default;
    ~Interpreter() = default;
    void setConfig(RuntimeConfig conf);
    Location run(const Location& location, RuntimeContext* ctx, IRBuffer& irBuffer,
                 size_t& numCycle);
    void visit(Instruction* inst);

#define INST(name, ret, ...) void name(__VA_ARGS__);
#include <AheuiJIT/IR/Instruction.inc>
#undef INST
  private:
    Word unwrapValue(Value* value);

    RuntimeConfig conf;
    RuntimeContext* ctx;
    Instruction* inst;
    std::map<uint64_t, Word> localBank;
    bool shouldFail = false;
    bool popFixup = false;
};

}  // namespace aheuijit
