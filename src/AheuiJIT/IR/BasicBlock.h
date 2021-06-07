#pragma once

#include <AheuiJIT/IR/Instruction.h>
#include <AheuiJIT/IR/Location.h>
#include <AheuiJIT/IR/Value.h>

#include <list>
#include <sstream>
#include <variant>

namespace aheuijit {

struct BasicBlock;

enum class TerminalType { Link, Exit, Conditional };

struct Terminal {
    Terminal() {
    }
    virtual ~Terminal() {
    }
    virtual std::string description() const = 0;
    virtual TerminalType getType() const = 0;
};

struct BasicBlock {
    explicit BasicBlock(uint64_t id) : id(id) {
    }
    BasicBlock() = delete;
    ~BasicBlock();

    std::string description() const;

    void ownValue(Value *value);

    void ownInstruction(Instruction *inst);
    uint64_t id;
    Location location;
    std::list<Instruction *> insts;
    std::list<Value *> values;
    Terminal *terminal = nullptr;

  private:
    std::list<Instruction *> ownedInsts;
    std::list<Value *> ownedValues;
};

struct LinkTerminal : public Terminal {
    explicit LinkTerminal(BasicBlock *block) : block(block) {
    }
    ~LinkTerminal() = default;
    TerminalType getType() const override {
        return TerminalType::Link;
    }
    std::string description() const override;
    BasicBlock *block;
};

struct ExitTerminal : public Terminal {
    ExitTerminal() = default;
    ~ExitTerminal() = default;
    TerminalType getType() const override {
        return TerminalType::Exit;
    }
    std::string description() const override;
};

struct ConditionalTerminal : public Terminal {
    explicit ConditionalTerminal(Value *predicate) : predicate(predicate) {
    }
    ~ConditionalTerminal() = default;
    TerminalType getType() const override {
        return TerminalType::Conditional;
    }
    Value *predicate;
    std::string description() const override;
    BasicBlock *pass = nullptr;
    BasicBlock *fail = nullptr;
};

}  // namespace aheuijit
