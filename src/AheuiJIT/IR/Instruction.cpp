#include "Instruction.h"

#include <AheuiJIT/IR/InstructionVisitor.h>
#include <fmt/format.h>

#include <array>

using namespace aheuijit;

Instruction::Instruction(Opcode op, uint64_t offset, Location location)
    : opcode(op), offset(offset), location(location) {
    args.fill(0);
}

std::string InstructionFormatter::format(Instruction *inst) {
    static InstructionVisitorCaller<InstructionFormatter> caller;
    this->inst = inst;
    ss.str("");
    caller.call(*this, inst);
    return ss.str();
}

void InstructionFormatter::add(Value *lhs, Value *rhs) {
    ss << fmt::format("{} = Add({}, {})", inst->output->description(), lhs->description(),
                      rhs->description());
}

void InstructionFormatter::sub(Value *lhs, Value *rhs) {
    ss << fmt::format("{} = Sub({}, {})", inst->output->description(), lhs->description(),
                      rhs->description());
}

void InstructionFormatter::div(Value *lhs, Value *rhs) {
    ss << fmt::format("{} = Div({}, {})", inst->output->description(), lhs->description(),
                      rhs->description());
}

void InstructionFormatter::mul(Value *lhs, Value *rhs) {
    ss << fmt::format("{} = Mul({}, {})", inst->output->description(), lhs->description(),
                      rhs->description());
}

void InstructionFormatter::mod(Value *lhs, Value *rhs) {
    ss << fmt::format("{} = Mod({}, {})", inst->output->description(), lhs->description(),
                      rhs->description());
}

void InstructionFormatter::pushStack(Value *value) {
    ss << fmt::format("PushStack({})", value->description());
}

void InstructionFormatter::popStack(Void *) {
    ss << fmt::format("{} = PopStack()", inst->output->description());
}

void InstructionFormatter::pushQueueBack(Value *value) {
    ss << fmt::format("pushQueueBack({})", value->description());
}

void InstructionFormatter::pushQueueFront(Value *value) {
    ss << fmt::format("pushQueueFront({})", value->description());
}

void InstructionFormatter::popQueue(Void *) {
    ss << fmt::format("{} = PopQueue()", inst->output->description());
}

void InstructionFormatter::inputChar(Void *) {
    ss << fmt::format("{} = InputChar()", inst->output->description());
}

void InstructionFormatter::inputNum(Void *) {
    ss << fmt::format("{} = InputNum()", inst->output->description());
}

void InstructionFormatter::cmp(Value *lhs, Value *rhs) {
    ss << fmt::format("{} = Cmp({}, {})", inst->output->description(), lhs->description(),
                      rhs->description());
}

void InstructionFormatter::outputChar(Value *value) {
    ss << fmt::format("OutputChar({})", value->description());
}

void InstructionFormatter::outputNum(Value *value) {
    ss << fmt::format("OutputNum({})", value->description());
}

void InstructionFormatter::getStore(Void *) {
    ss << fmt::format("{} = GetStore()", inst->output->description());
}

void InstructionFormatter::setStore(Value *value) {
    ss << fmt::format("SetStore({})", value->description());
}

void InstructionFormatter::checkStack1(Void *) {
    ss << "CheckStack1()";
}

void InstructionFormatter::checkStack2(Void *) {
    ss << "CheckStack2()";
}

void InstructionFormatter::checkQueue1(Void *) {
    ss << "CheckQueue1()";
}

void InstructionFormatter::checkQueue2(Void *) {
    ss << "CheckQueue2()";
}
