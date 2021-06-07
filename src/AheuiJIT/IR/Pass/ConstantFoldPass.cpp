#include <AheuiJIT/Util/Util.h>

#include <map>
#include <set>

#include "Pass.h"

using namespace aheuijit;

bool ConstantFoldPass::runOnBlock(Builder& b, BasicBlock* block) {
    std::map<Value*, Value*> foldedValues;

    const auto doALU = [](Opcode op, int64_t lhs, int64_t rhs) {
        switch (op) {
            case Opcode::add:
                return lhs + rhs;
            case Opcode::sub:
                return lhs - rhs;
            case Opcode::mul:
                return lhs * rhs;
            case Opcode::mod:
                return lhs % rhs;
            case Opcode::div:
                return lhs / rhs;
            case Opcode::cmp:
                return (Word)(lhs >= rhs);
            default:
                ASSERT(false)
        }
    };

    for (auto it = block->insts.begin(); it != block->insts.end();) {
        Instruction* inst = *it;
        for (int i = 0; i < inst->args.size(); ++i) {
            if (inst->args[i] && inst->args[i]->type() == ValueType::Local) {
                const auto it = foldedValues.find(inst->args[i]);
                if (it != foldedValues.end()) {
                    inst->args[i] = it->second;
                }
            }
        }
        switch (inst->opcode) {
            case Opcode::add:
            case Opcode::sub:
            case Opcode::mul:
            case Opcode::mod:
            case Opcode::div:
            case Opcode::cmp: {
                if (inst->args[0]->type() == ValueType::Constant &&
                    inst->args[1]->type() == ValueType::Constant) {
                    Constant* lhs = dynamic_cast<Constant*>(inst->args[0]);
                    Constant* rhs = dynamic_cast<Constant*>(inst->args[1]);
                    lhs->imm = doALU(inst->opcode, lhs->imm, rhs->imm);
                    foldedValues.emplace(inst->output, lhs);
                    block->insts.erase(it++);
                }
                break;
            }
            default:
                break;
        }
        ++it;
    }

    if (block->terminal->getType() == TerminalType::Conditional) {
        ConditionalTerminal* term = dynamic_cast<ConditionalTerminal*>(block->terminal);
        const auto it = foldedValues.find(term->predicate);
        if (it != foldedValues.end()) {
            term->predicate = it->second;
        }
    }
    return true;
}
