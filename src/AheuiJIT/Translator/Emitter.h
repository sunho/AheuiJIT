#pragma once

#include <AheuiJIT/Runtime/Machine.h>
#include <AheuiJIT/Runtime/Runtime.h>

namespace aheuijit {

struct Emitter {
    virtual ~Emitter() {
    }
    void setConfig(const RuntimeConfig &conf) {
        this->conf = conf;
    }
    virtual void emit(BasicBlock *bb, const TLBTable &table, std::set<BasicBlock *> &emitted) = 0;

  protected:
    RuntimeConfig conf;
};

}  // namespace aheuijit
