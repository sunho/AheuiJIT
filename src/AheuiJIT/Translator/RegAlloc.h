#pragma once
#include <AheuiJIT/IR/Value.h>
#include <asmjit/x86.h>

#include <array>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>

namespace aheuijit {

struct Reg {
    uint64_t id;

    Reg() = default;
    explicit Reg(uint64_t id, std::function<asmjit::x86::Gp()> getter,
                 std::function<void()> deleter)
        : id(id), getter(getter), deleter(deleter) {
    }

    ~Reg() {
        if (deleter) {
            deleter();
        }
    }

    asmjit::x86::Gp get() {
        return getter();
    }

    Reg(Reg &&o) noexcept {
        std::swap(deleter, o.deleter);
        std::swap(getter, o.getter);
        std::swap(id, o.id);
    }

    Reg &operator=(Reg &&o) noexcept {
        std::swap(deleter, o.deleter);
        std::swap(getter, o.getter);
        std::swap(id, o.id);
        return *this;
    }

  private:
    std::function<asmjit::x86::Gp()> getter = nullptr;
    std::function<void()> deleter = nullptr;
};

constexpr size_t MAX_ENDPOINT = 1U << 31;
constexpr size_t MAX_PHY_REGS = 16;

struct RegAllocInterval {
    RegAllocInterval() = delete;
    explicit RegAllocInterval(uint64_t id, uint64_t start) : id(id), start(start) {
    }
    explicit RegAllocInterval(uint64_t id, uint64_t start, uint64_t endpoint)
        : id(id), start(start), endpoint(endpoint) {
    }

    ~RegAllocInterval() = default;
    uint64_t id;
    size_t start;
    size_t activeIndex = 0;
    size_t activeTs = 0;
    size_t endpoint = 0;
};

struct RegAllocIntervalStartCompare {
    bool operator()(RegAllocInterval *lhs, RegAllocInterval *rhs) const {
        if (lhs->start == rhs->start) {
            return lhs->activeTs < rhs->activeTs;
        }
        return lhs->start < rhs->start;
    }
};

struct RegAllocIntervalEndCompare {
    bool operator()(RegAllocInterval *lhs, RegAllocInterval *rhs) const {
        return lhs->endpoint < rhs->endpoint;
    }
};

struct RegAlloc {
    RegAlloc() = delete;
    RegAlloc(asmjit::x86::Assembler &code);
    ~RegAlloc() = default;

    void setInstructionIndex(size_t index);

    Reg allocSystem(asmjit::x86::Gp reg);
    Reg allocLocal(Local *local);
    Reg allocTmp();

    void release(uint64_t id);

    void emitInit();
    void emitDeinit();
    void emitDeinitStub();

    void reset();

  private:
    asmjit::x86::Assembler &code;
    size_t index = 0;
    uint64_t next_id = 1;
    uint64_t ts = 1;

    asmjit::x86::Gp alloc(uint64_t id);
    void expireActiveInterval(uint64_t id);
    void expireOldActiveIntervals();
    void unspillAtInterval(uint64_t id, uint8_t phyId);
    void spillAtInterval(uint64_t id);
    void registerInterval(std::unique_ptr<RegAllocInterval> interval);
    void unregisterInteval(uint64_t id);
    void patchStub();
    void emitStub();

    std::array<uint64_t, MAX_PHY_REGS> active;
    std::map<uint64_t, uint64_t> localMap;
    std::set<uint64_t> systemRegs;
    std::map<uint64_t, int64_t> spillMap;
    int64_t spillTop{ 0 };
    int64_t spillCursor{ 0 };
    int64_t stubOffset{ 0 };
    std::list<int64_t> deinitStubOffsets;

    std::list<RegAllocInterval *> active_intervals_by_start;
    std::multiset<RegAllocInterval *, RegAllocIntervalEndCompare> active_intervals_by_end;
    std::map<uint64_t, std::unique_ptr<RegAllocInterval>> intervals;
};

}  // namespace aheuijit
