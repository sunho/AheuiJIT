#pragma once

#include <AheuiJIT/IR/BasicBlock.h>

#include <functional>
#include <map>
#include <memory>

namespace aheuijit {

using BasicBlockPtr = std::unique_ptr<BasicBlock>;

struct IRBuffer {
    IRBuffer() {
    }

    ~IRBuffer() = default;

    BasicBlock *findBlock(const Location &location) {
        const auto it = blocks.find(location);
        if (it == blocks.end())
            return nullptr;
        return it->second.get();
    }
    BasicBlock *createBlock(const Location &location) {
        auto block = std::make_unique<BasicBlock>(nextId++);
        BasicBlock *out = block.get();
        blocks.emplace(location, std::move(block));
        return out;
    }
    void reset() {
        blocks.clear();
    }

  private:
    uint64_t nextId = 1;
    std::map<Location, BasicBlockPtr> blocks;
};

}  // namespace aheuijit