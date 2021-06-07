#pragma once
#include <AheuiJIT/IR/Storage.h>

#include <cassert>
#include <cstdlib>
#include <optional>
#include <string>
#include <type_traits>

namespace aheuijit {

using Word = int64_t;

enum class ValueType { Local, Constant, Void };

struct Value {
    Value() {
    }
    virtual ~Value() {
    }
    virtual std::string description() const = 0;
    virtual ValueType type() const {
        return ValueType::Void;
    }
};

using Void = Value;

struct Local : public Value {
    Local() : id(0) {
    }
    ~Local() = default;
    explicit Local(uint64_t id) : id(id) {
    }
    std::string description() const override;

    ValueType type() const override;

    uint64_t id;
    std::optional<uint64_t> endpoint = std::nullopt;
};

struct Constant : public Value {
    Constant() : imm(0) {
    }
    ~Constant() = default;
    explicit Constant(Word imm) : imm(imm) {
    }
    std::string description() const override;

    ValueType type() const override;

    Word imm;
};

}  // namespace aheuijit
