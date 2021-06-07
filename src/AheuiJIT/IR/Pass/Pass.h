#pragma once

#include <AheuiJIT/IR/Builder.h>

namespace aheuijit {

struct Pass {
    Pass() {
    }
    virtual ~Pass() {
    }
};

struct BasicBlockPass : public Pass {
    virtual ~BasicBlockPass() {
    }
    virtual bool runOnBlock(Builder& b, BasicBlock* block) = 0;
};

struct AggregatePushPopPass : public BasicBlockPass {
    ~AggregatePushPopPass() {
    }
    bool runOnBlock(Builder& b, BasicBlock* block) override;
};

struct RemoveDeadStoreUpdate : public BasicBlockPass {
    ~RemoveDeadStoreUpdate() {
    }
    bool runOnBlock(Builder& b, BasicBlock* block) override;
};

struct EndpointPass : public BasicBlockPass {
    ~EndpointPass() {
    }
    bool runOnBlock(Builder& b, BasicBlock* block) override;
};

struct RemoveDeadAssignmentPass : public BasicBlockPass {
    ~RemoveDeadAssignmentPass() {
    }
    bool runOnBlock(Builder& b, BasicBlock* block) override;
};

struct ConstantFoldPass : public BasicBlockPass {
    ~ConstantFoldPass() {
    }
    bool runOnBlock(Builder& b, BasicBlock* block) override;
};

}  // namespace aheuijit
