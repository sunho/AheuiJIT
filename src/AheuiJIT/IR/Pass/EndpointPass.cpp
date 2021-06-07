#include <map>

#include "Pass.h"

using namespace aheuijit;

bool EndpointPass::runOnBlock(Builder& b, BasicBlock* block) {
    for (auto inst : block->insts) {
        for (int i = 0; i < inst->args.size(); ++i) {
            Value* value = inst->args[i];
            if (value && value->type() == ValueType::Local) {
                dynamic_cast<Local*>(value)->endpoint = inst->offset;
            }
        }
    }
    return true;
}

static bool isNoSideEffectOp(Opcode op) {
    switch (op) {
        case Opcode::add:
        case Opcode::sub:
        case Opcode::div:
        case Opcode::mod:
        case Opcode::cmp:
        case Opcode::mul:
        case Opcode::getStore:
            return true;
        default:
            return false;
    }
}

bool RemoveDeadAssignmentPass::runOnBlock(Builder& b, BasicBlock* block) {
    Value* pred = nullptr;
    if (block->terminal->getType() == TerminalType::Conditional) {
        ConditionalTerminal* term = dynamic_cast<ConditionalTerminal*>(block->terminal);
        pred = term->predicate;
    }
    for (auto it = block->insts.begin(); it != block->insts.end();) {
        const Instruction* inst = *it;
        const Local* output = inst->output;
        if (output && !output->endpoint && isNoSideEffectOp(inst->opcode) && output != pred) {
            block->insts.erase(it++);
            continue;
        }
        ++it;
    }
    return true;
}
