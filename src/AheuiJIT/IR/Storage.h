#pragma once

namespace aheuijit {

constexpr int QUEUE_STORAGE_IMM = 53;

struct Storage {
    explicit Storage(bool stack, int num) : stack(stack), num(num) {
    }
    Storage() = default;
    ~Storage() = default;

    int toImm() const {
        if (!stack)
            return QUEUE_STORAGE_IMM;
        return num;
    }

    bool stack{ false };
    int num{ 0 };
};

}  // namespace aheuijit
