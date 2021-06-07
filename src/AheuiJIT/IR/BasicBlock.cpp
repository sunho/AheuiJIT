#include "BasicBlock.h"

#include <fmt/format.h>

using namespace aheuijit;

BasicBlock::~BasicBlock() {
    if (terminal) {
        delete terminal;
    }
    for (auto inst : ownedInsts) {
        delete inst;
    }
    for (auto val : ownedValues) {
        delete val;
    }
}

void BasicBlock::ownValue(Value *value) {
    ownedValues.push_back(value);
}

void BasicBlock::ownInstruction(Instruction *inst) {
    ownedInsts.push_back(inst);
}

std::string BasicBlock::description() const {
    std::stringstream ss;
    InstructionFormatter formatter;
    ss << fmt::format("BasicBlock(x: {}, y: {})", location.x, location.y) << std::endl;
    for (auto inst : insts) {
        ss << "\t" << formatter.format(inst) << std::endl;
    }
    ss << terminal->description() << std::endl;
    return ss.str();
}

std::string ExitTerminal::description() const {
    return "ExitTerminal()";
}

std::string LinkTerminal::description() const {
    return fmt::format("LinkTerminal(BasicBlock(x: {}, y: {}))", block->location.x,
                       block->location.y);
}

std::string ConditionalTerminal::description() const {
    if (pass && fail)
        return fmt::format("ConditionalTerminal({}, pass: ({}, {}), fail: ({}, {}))",
                           predicate->description(), pass->location.x, pass->location.y,
                           fail->location.x, fail->location.y);
    return fmt::format("ConditionalTerminal({}, pass: nil, fail: nil)", predicate->description());
}
