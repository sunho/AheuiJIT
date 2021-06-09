#include <AheuiJIT/Util/Util.h>

#include <map>
#include <set>

#include "Pass.h"

using namespace aheuijit;

// push x 0 -
// push x 1 *
// pop x 1 *
// pop x 0 -
// pop x -1  ignore
// pop x -2 ignore
// push x 0 -
// pop x 0 -
// Match pushStack and popStack to same storage

bool AggregatePushPopPass::runOnBlock(Builder& b, BasicBlock* block) {
    struct TrackedPush {
        int offset;
        Value* value;
    };
    std::array<int, 27> counts;  // 26: current storage
    int trackedStorage = 26;
    std::set<int> removeInstOffsets;
    std::map<Value*, Value*> patchValues;
    std::map<uint64_t, TrackedPush> pushes;
    std::map<Location, int> popCounts;
    std::map<int, Location> removeOffsetCounterpart;

    const auto getKey = [](uint32_t count, uint32_t storage) {
        return ((uint64_t)count << 32) | (uint64_t)storage;
    };

    counts.fill(0);

    std::map<Value*, int> storageMatches;
    int getStorageRes = 0;
    for (auto inst : block->insts) {
        switch (inst->opcode) {
            case Opcode::setStore: {
                // setStore(getStore())
                if (inst->args[0]->type() == ValueType::Local) {
                    trackedStorage = storageMatches.at(inst->args[0]);
                } else {
                    const uint32_t newStorage = dynamic_cast<Constant*>(inst->args[0])->imm;
                    trackedStorage = newStorage;
                }
                break;
            }
            case Opcode::getStore: {
                storageMatches.emplace(inst->output, trackedStorage);
                break;
            }
            case Opcode::pushStack: {
                TrackedPush tp;
                tp.offset = inst->offset;
                tp.value = inst->args[0];
                const auto it = patchValues.find(tp.value);
                if (it != patchValues.end()) {
                    tp.value = it->second;
                }
                const uint64_t key = getKey(counts[trackedStorage]++, trackedStorage);
                pushes[key] = tp;
                break;
            }
            case Opcode::popStack: {
                ++popCounts[inst->location];
                if (counts[trackedStorage] == 0) {
                    break;
                }
                --popCounts[inst->location];
                const uint64_t key = getKey(--counts[trackedStorage], trackedStorage);
                const TrackedPush tp = pushes[key];
                ASSERT(tp.value)
                removeInstOffsets.insert(tp.offset);
                removeInstOffsets.insert(inst->offset);
                removeOffsetCounterpart.emplace(tp.offset, inst->location);
                patchValues.emplace(inst->output, tp.value);
                pushes.erase(key);
                break;
            }
        }
    }

    for (auto it = block->insts.begin(); it != block->insts.end();) {
        Instruction* inst = *it;
        if (removeInstOffsets.find(inst->offset) != removeInstOffsets.end()) {
            if (inst->opcode == Opcode::popStack && popCounts[inst->location] == 0) {
                block->insts.erase(it++);
                removeInstOffsets.erase(inst->offset);
                continue;
            } else if (inst->opcode == Opcode::pushStack &&
                       popCounts[removeOffsetCounterpart[inst->offset]] == 0) {
                block->insts.erase(it++);
                removeInstOffsets.erase(inst->offset);
                continue;
            }
        }

        for (int i = 0; i < inst->args.size(); ++i) {
            if (inst->args[i] && inst->args[i]->type() == ValueType::Local) {
                const auto it = patchValues.find(inst->args[i]);
                if (it != patchValues.end()) {
                    inst->args[i] = it->second;
                }
            }
        }
        ++it;
    }

    if (block->terminal->getType() == TerminalType::Conditional) {
        ConditionalTerminal* term = dynamic_cast<ConditionalTerminal*>(block->terminal);
        const auto it = patchValues.find(term->predicate);
        if (it != patchValues.end()) {
            term->predicate = it->second;
        }
    }
    return true;
}

bool RemoveDeadStoreUpdate::runOnBlock(Builder& b, BasicBlock* block) {
    auto storeIt = block->insts.end();
    int count = 0;
    std::map<Value*, Value*> getStoreQuery;
    for (auto it = block->insts.begin(); it != block->insts.end(); ++it) {
        Instruction* inst = *it;
        if (inst->opcode == Opcode::setStore) {
            if (storeIt != block->insts.end()) {
                Instruction* sinst = *storeIt;
                Value* arg0 = sinst->args[0];
                if (count == 0) {
                    block->insts.erase(storeIt);
                } else if (arg0->type() == ValueType::Local) {
                    sinst->args[0] = getStoreQuery[arg0];
                }
            }
            count = 0;
            storeIt = it;
            continue;
        }
        switch (inst->opcode) {
            case Opcode::pushStack:
            case Opcode::pushQueueBack:
            case Opcode::pushQueueFront:
            case Opcode::popQueue:
            case Opcode::popStack:
                ++count;
                break;
            case Opcode::getStore: {
                // setStore constant folding to fight getStore() corruption happen because of
                // removed setStores Based on the fact setStore(x) x is either getStore() or imm
                // setStore(local) is what's making things complicated
                //
                // Starting from i = 0,
                // Fist local = getStore() value is resolved absolutely (either actual getStore()
                // value or setStore(imm)) second getStore() value is from setStore(local) or
                // setStore(imm) only available local = first local -> resvoled third getStore()
                // value is from setStore(local) or setStore(local2) or setStore(imm)
                // -> resolved -> so on
                //
                if (storeIt == block->insts.end()) {
                    // First getStorage query
                    getStoreQuery.emplace(inst->output, inst->output);
                    break;
                }
                if ((*storeIt)->args[0]->type() == ValueType::Constant) {
                    getStoreQuery.emplace(inst->output, (*storeIt)->args[0]);
                } else {  // Local
                    Local* getStoreRes = dynamic_cast<Local*>((*storeIt)->args[0]);
                    const auto it = getStoreQuery.find(getStoreRes);
                    ASSERT(it != getStoreQuery.end())  // This should not assert; it means there's
                                                       // no avaiable local value.
                    getStoreQuery.emplace(inst->output, it->second);
                }
                break;
            }
            default:
                break;
        }
    }

    // I shouldn't delete last setStore but must constand fold it
    if (storeIt != block->insts.end()) {
        Instruction* sinst = *storeIt;
        Value* arg0 = sinst->args[0];
        if (arg0->type() == ValueType::Local) {
            sinst->args[0] = getStoreQuery[arg0];
        }
    }
    return true;
}
