#pragma once

#include <AheuiJIT/IR/Location.h>
#include <AheuiJIT/IR/Value.h>

#include <array>
#include <sstream>
#include <tuple>
#include <variant>

namespace aheuijit {

#define INST(name, ret, ...) name,
enum class Opcode {
#include "Instruction.inc"
};
#undef INST

constexpr int MAX_INST_ARGUMENTS = 2;

struct Instruction {
    Instruction(Opcode opcode, uint64_t offset, Location location);
    ~Instruction() = default;

    Opcode opcode;
    Location location;
    Local *output = nullptr;
    std::array<Value *, MAX_INST_ARGUMENTS> args;
    uint64_t offset;
};

struct InstructionFormatter {
    std::string format(Instruction *inst);

#define INST(name, ret, ...) void name(__VA_ARGS__);
#include "Instruction.inc"
#undef INST

  private:
    std::stringstream ss;
    Instruction *inst;
};

}  // namespace aheuijit
