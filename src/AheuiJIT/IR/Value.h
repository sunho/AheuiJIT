#pragma once
#include <cstdlib>
#include <AheuiJIT/IR/Storage.h>

struct Value {
    Value();
    virtual ~Value();
};

struct RegValue : public Value {
    explicit RegValue(uint64_t id);
    ~RegValue() = default;

    uint64_t getID() const;

    void setEndpoint(size_t endpoint);
    size_t getEndpoint() const;
private:
    uint64_t id;
    size_t endpoint;
};

struct ConstantValue : public Value {
    ConstantValue(uint64_t value);
    ~ConstantValue() = default;

    uint64_t get();
private:
    uint64_t value;
};

struct StorageValue : public Value {
    StorageValue(Storage store);
    ~StorageValue() = default;
private:
    Storage store;
};
