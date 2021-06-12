#pragma once

#include <AheuiJIT/IR/BasicBlock.h>
#include <AheuiJIT/Runtime/RuntimeContext.h>

#include <functional>
#include <map>
#include <set>

namespace aheuijit {

using TranslatedFunc = std::function<void(RuntimeContext* ctx)>;

struct CodeBlock {
    virtual ~CodeBlock() {
    }
};

using CodeBlockPtr = std::unique_ptr<CodeBlock>;
using TLBTable = std::map<Location, uint64_t>;
using CodeBlockId = uint32_t;

struct Machine {
    TLBTable tlbTable;
    virtual ~Machine() {
    }
    virtual CodeBlockPtr createBlock() = 0;
    virtual void addCodeBlock(const Location& location, CodeBlock* block,
                              std::set<BasicBlock*> emitted) = 0;
    virtual bool hasCodeBlock(const Location& location) = 0;
    virtual void runCodeBlock(const Location& location, RuntimeContext* ctx) = 0;
    virtual void printChar(Word word) = 0;
    virtual void printNum(Word word) = 0;
    virtual Word inputChar() = 0;
    virtual Word inputNum() = 0;

    void setConfig(const RuntimeConfig& conf) {
        this->conf = conf;
    }

  protected:
    RuntimeConfig conf;
};

};  // namespace aheuijit
