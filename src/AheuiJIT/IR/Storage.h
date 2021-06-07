#pragma once

namespace aheuijit {

struct Storage {
    explicit Storage(bool stack, int num) : stack(stack), num(num) {
    }
    Storage() = default;
    ~Storage() = default;

    int toImm() const {
        return !stack ? 0 : num + 1;
    }

    bool stack{ false };
    int num{ 0 };
};

}  // namespace aheuijit
