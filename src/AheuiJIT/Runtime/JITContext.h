#pragma once

#include <AheuiJIT/IR/Location.h>
#include <AheuiJIT/Util/Util.h>

#include <array>
#include <cstddef>
#include <cstdlib>

namespace aheuijit {

struct Runtime;

constexpr int HASH_TABLE_BEANS = 128;
constexpr int HASH_TABLE_BEAN_SIZE = 32;
constexpr int HAST_TABLE_BEAN_SHIFT = 9;
struct JITHashTableKeyValue {
    uint64_t key;
    uint64_t value;
};

struct JITHashTable {
    JITHashTable() {
        beans.fill({ 0, 0 });
    }
    ~JITHashTable() = default;
    std::array<JITHashTableKeyValue, HASH_TABLE_BEANS * HASH_TABLE_BEAN_SIZE> beans;
    void setValue(uint64_t key, uint64_t value) {
        const size_t bean = key % HASH_TABLE_BEANS;
        const size_t beanOffset = bean * HASH_TABLE_BEAN_SIZE;
        for (size_t i = 0; i < HASH_TABLE_BEAN_SIZE - 1; ++i) {
            auto& kv = beans[beanOffset + i];
            ASSERT(kv.key != key)
            if (kv.key == 0 && kv.value == 0) {
                kv.key = key;
                kv.value = value;
                return;
            }
        }
        ASSERT(false)
    }
};

struct JITContext {
    JITContext() {
        stackUppers.fill(0);
        stackTops.fill(0);
    };
    ~JITContext() = default;

    Runtime* runtime{ nullptr };
    uint64_t location{ 0 };
    int storage{ 0 };
    uint64_t queueBuffer{ 0 };
    uint64_t queueBack{ 0 };
    uint64_t queueFront{ 0 };
    std::array<uint64_t, 26> stackUppers;
    std::array<uint64_t, 26> stackTops;
    JITHashTable* exhaustPatchTable{ nullptr };
};

}  // namespace aheuijit
