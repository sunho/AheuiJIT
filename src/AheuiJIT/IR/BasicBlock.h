#pragma once

#include <AheuiJIT/IR/Instruction.h>
#include <AheuiJIT/IR/Value.h>
#include <variant>
#include <list>

using Location = int;

struct LinkTerminal {
    LinkTerminal(Location location);
};

struct ExitTerminal {
    ExitTerminal();
};

struct ConditionalTerminal {
    ConditionalTerminal(Value* predicate, Location pass, Location fail);
};

using Terminal = std::variant<LinkTerminal, ExitTerminal, ConditionalTerminal>;

struct BasicBlock {
    BasicBlock();
    ~BasicBlock() = default;

    Location location;
    std::list<Instruction*> insts;
    std::list<Value*> values;
    Terminal terminal;
};
