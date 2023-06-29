#pragma once
#include <AheuiJIT/IR/Value.h>

#include <array>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>

namespace aheuijit {

template <typename T>
struct Reg {
    uint64_t id;

    Reg() = default;
    explicit Reg(uint64_t id, std::function<T()> getter, std::function<void()> deleter)
        : id(id), getter(getter), deleter(deleter) {
    }

    ~Reg() {
        if (deleter) {
            deleter();
        }
    }

    T get() {
        return getter();
    }

    Reg(const Reg&) = delete;
    Reg& operator=(const Reg&) = delete;

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
    std::function<T()> getter = nullptr;
    std::function<void()> deleter = nullptr;
};

constexpr size_t MAX_ENDPOINT = 1U << 31;

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

template <typename T>
struct RegAllocProtocol {
    virtual ~RegAllocProtocol() {
    }
    virtual void emitStore(uint32_t offset, T reg) = 0;
    virtual void emitLoad(T reg, uint32_t offset) = 0;
    virtual void emitInitStub() = 0;
    virtual void emitDeinitStub() = 0;
    virtual void finalize(uint32_t size) = 0;
    virtual T convertToReg(uint8_t index) = 0;
    virtual uint8_t convertFromReg(T reg) = 0;
};

template <typename T, size_t MaxRegs>
struct RegAlloc {
    RegAlloc() = delete;
    ~RegAlloc() = default;

    RegAlloc(std::unique_ptr<RegAllocProtocol<T>> protocol) : protocol(std::move(protocol)) {
        active.fill(0);
    }

    void finalize() {
        protocol->finalize(spillTop);
    }

    void setInstructionIndex(size_t index) {
        this->index = index;
        expireOldActiveIntervals();
    }

    Reg<T> allocSystem(T reg) {
        const uint8_t regIndex = protocol->convertFromReg(reg);
        if (active[regIndex]) {
            spillAtInterval(active[regIndex]);
            expireActiveInterval(active[regIndex]);
        }
        // TODO: I can pool this or just not do dynamic allocation
        std::unique_ptr<RegAllocInterval> interval =
            std::make_unique<RegAllocInterval>(next_id++, index);
        RegAllocInterval *ptr = interval.get();
        registerInterval(std::move(interval));
        systemRegs.insert(ptr->id);
        active[reg.id()] = ptr->id;

        return Reg<T>(
            ptr->id, [reg]() { return reg; }, [this, reg]() { this->active[reg.id()] = 0; });
    }

    Reg<T> allocLocal(Local *local) {
        auto it = localMap.find(local->id);
        if (it != localMap.end()) {
            RegAllocInterval *ptr = intervals.find(it->second)->second.get();
            return Reg<T>(
                ptr->id, [this, ptr]() { return this->alloc(ptr->id); }, nullptr);
        }
        auto interval = std::make_unique<RegAllocInterval>(
            next_id++, index, local->endpoint ? local->endpoint.value() : MAX_ENDPOINT);
        RegAllocInterval *ptr = interval.get();
        registerInterval(std::move(interval));
        localMap.emplace(local->id, ptr->id);
        return Reg<T>(
            ptr->id, [this, ptr]() { return this->alloc(ptr->id); }, nullptr);
    }

    Reg<T> allocTmp() {
        std::unique_ptr<RegAllocInterval> interval =
            std::make_unique<RegAllocInterval>(next_id++, index);
        RegAllocInterval *ptr = interval.get();
        registerInterval(std::move(interval));
        return Reg<T>(
            ptr->id, [this, ptr]() { return this->alloc(ptr->id); },
            [this, ptr]() { this->unregisterInteval(ptr->id); });
    }

    void release(uint64_t id) {
        unregisterInteval(id);
    }

    void reset() {
        spillTop = 0;
        for (int i = 0; i < active.size(); ++i) {
            active[i] = 0;
        }
        spillMap.clear();
        active_intervals_by_end.clear();
        active_intervals_by_start.clear();
        localMap.clear();
        intervals.clear();
        systemRegs.clear();
        index = 0;
        spillCursor = 0;
        ts = 1;
        next_id = 1;
    }

    void emitInitStub() {
        protocol->emitInitStub();
    }

    void emitDeinitStub() {
        protocol->emitDeinitStub();
    }

  private:
    size_t index = 0;
    uint64_t next_id = 1;
    uint64_t ts = 1;

    T alloc(uint64_t id) {
        RegAllocInterval *ptr = intervals.find(id)->second.get();
        int emptyI = -1;
        for (int i = 0; i < active.size(); ++i) {
            if (active[i] == id) {
                return protocol->convertToReg(i);
            }
            if (active[i] == 0) {
                emptyI = i;
            }
        }
        ptr->activeTs = ts++;
        if (emptyI != -1) {
            unspillAtInterval(id, emptyI);
            active[emptyI] = id;
            active_intervals_by_start.push_back(ptr);
            active_intervals_by_end.insert(ptr);
            ptr->activeIndex = emptyI;
            return protocol->convertToReg(emptyI);
        } else {
            RegAllocInterval *old = *active_intervals_by_start.begin();
            spillAtInterval(old->id);
            expireActiveInterval(old->id);
            active_intervals_by_start.push_back(ptr);
            active_intervals_by_end.insert(ptr);
            unspillAtInterval(id, old->activeIndex);
            active[old->activeIndex] = id;
            ptr->activeIndex = old->activeIndex;
            return protocol->convertToReg(old->activeIndex);
        }
    }

    void expireOldActiveIntervals() {
        auto it = active_intervals_by_end.begin();
        while (it != active_intervals_by_end.end()) {
            RegAllocInterval *interval = *it;
            if (interval->endpoint >= index) {
                break;
            }
            expireActiveInterval(interval->id);
            it = active_intervals_by_end.begin();
        }
    }

    void spillAtInterval(uint64_t id) {
        uint8_t phyId = 0xFF;
        for (int i = 0; i < active.size(); ++i) {
            if (active[i] == id) {
                phyId = i;
                break;
            }
        }
        if (spillCursor == 0) {
            spillTop += 16;
            spillCursor = 16;
        }
        const T reg = protocol->convertToReg(phyId);
        const uint64_t offset = spillTop - spillCursor;
        protocol->emitStore(offset, reg);
        spillMap[id] = offset;
        spillCursor -= 8;
    }

    void patchStub();
    void emitStub();

    void registerInterval(std::unique_ptr<RegAllocInterval> interval) {
        RegAllocInterval *ptr = interval.get();
        intervals.emplace(interval->id, std::move(interval));
    }

    void unregisterInteval(uint64_t id) {
        expireActiveInterval(id);
        intervals.erase(id);
    }

    void expireActiveInterval(uint64_t id) {
        RegAllocInterval *ptr = intervals.find(id)->second.get();
        for (int i = 0; i < active.size(); ++i) {
            if (active[i] == id) {
                active[i] = 0;
            }
        }

        for (auto it = active_intervals_by_start.begin(); it != active_intervals_by_start.end();
             ++it) {
            if ((*it)->id == ptr->id) {
                active_intervals_by_start.erase(it);
                break;
            }
        }

        auto it2 = active_intervals_by_end.lower_bound(ptr);
        while (it2 != active_intervals_by_end.end() && (*it2)->endpoint == ptr->endpoint) {
            if ((*it2)->id == ptr->id) {
                active_intervals_by_end.erase(it2);
                break;
            }
            ++it2;
        }
    }

    void unspillAtInterval(uint64_t id, uint8_t phyId) {
        const auto it = spillMap.find(id);
        if (it != spillMap.end()) {
            const T reg = protocol->convertToReg(phyId);
            const int64_t offset = it->second;
            protocol->emitLoad(reg, offset);
        }
    }

    std::array<uint64_t, MaxRegs> active;
    std::map<uint64_t, uint64_t> localMap;
    std::set<uint64_t> systemRegs;
    std::map<uint64_t, int64_t> spillMap;
    int64_t spillTop{ 0 };
    int64_t spillCursor{ 0 };

    std::list<RegAllocInterval *> active_intervals_by_start;
    std::multiset<RegAllocInterval *, RegAllocIntervalEndCompare> active_intervals_by_end;
    std::map<uint64_t, std::unique_ptr<RegAllocInterval>> intervals;

    std::unique_ptr<RegAllocProtocol<T>> protocol;
};

}  // namespace aheuijit
