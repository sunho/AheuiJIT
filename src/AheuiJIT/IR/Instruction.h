#pragma once

#include <AheuiJIT/IR/Value.h>
#include <array>
#include <variant>

#define INST(name, ret, ...) name,
enum class Opcode {
    #include <AheuiJIT/IR/Instruction.inc>
};
#undef INST

constexpr int MAX_INST_ARGUMENTS = 2;

struct Instruction {
    Instruction(Opcode opcode);
    ~Instruction() = default;

    Opcode opcode;
    RegValue* output;
    std::array<Value*, MAX_INST_ARGUMENTS> args;
};


