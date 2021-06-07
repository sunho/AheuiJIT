#include "RegAlloc.h"

#include <AheuiJIT/Util/Util.h>

using namespace aheuijit;
using namespace asmjit;

RegAlloc::RegAlloc(x86::Assembler &code) : code(code) {
    active.fill(0);
}

void RegAlloc::setInstructionIndex(size_t index) {
    this->index = index;
    expireOldActiveIntervals();
}

Reg RegAlloc::allocSystem(x86::Gp reg) {
    if (active[reg.id()]) {
        spillAtInterval(active[reg.id()]);
        expireActiveInterval(active[reg.id()]);
    }
    // TODO: I can pool this or just not do dynamic allocation
    std::unique_ptr<RegAllocInterval> interval =
        std::make_unique<RegAllocInterval>(next_id++, index);
    RegAllocInterval *ptr = interval.get();
    registerInterval(std::move(interval));
    systemRegs.insert(ptr->id);
    active[reg.id()] = ptr->id;

    return Reg(
        ptr->id, [reg]() { return reg; }, [this, reg]() { this->active[reg.id()] = 0; });
}

Reg RegAlloc::allocLocal(Local *local) {
    auto it = localMap.find(local->id);
    if (it != localMap.end()) {
        RegAllocInterval *ptr = intervals.find(it->second)->second.get();
        return Reg(
            ptr->id, [this, ptr]() { return this->alloc(ptr->id); }, nullptr);
    }
    auto interval = std::make_unique<RegAllocInterval>(
        next_id++, index, local->endpoint ? local->endpoint.value() : MAX_ENDPOINT);
    RegAllocInterval *ptr = interval.get();
    registerInterval(std::move(interval));
    localMap.emplace(local->id, ptr->id);
    return Reg(
        ptr->id, [this, ptr]() { return this->alloc(ptr->id); }, nullptr);
}

Reg RegAlloc::allocTmp() {
    std::unique_ptr<RegAllocInterval> interval =
        std::make_unique<RegAllocInterval>(next_id++, index);
    RegAllocInterval *ptr = interval.get();
    registerInterval(std::move(interval));
    return Reg(
        ptr->id, [this, ptr]() { return this->alloc(ptr->id); },
        [this, ptr]() { this->unregisterInteval(ptr->id); });
}

void RegAlloc::release(uint64_t id) {
    unregisterInteval(id);
}

void RegAlloc::registerInterval(std::unique_ptr<RegAllocInterval> interval) {
    RegAllocInterval *ptr = interval.get();
    intervals.emplace(interval->id, std::move(interval));
}

void RegAlloc::unregisterInteval(uint64_t id) {
    expireActiveInterval(id);
    intervals.erase(id);
}

static inline x86::Gp indexToGp(uint8_t index) {
    return x86::Reg::fromTypeAndId(BaseReg::RegType::kTypeGp64, index).as<x86::Gp>();
}

x86::Gp RegAlloc::alloc(uint64_t id) {
    RegAllocInterval *ptr = intervals.find(id)->second.get();
    int emptyI = -1;
    for (int i = 0; i < active.size(); ++i) {
        if (active[i] == id) {
            return indexToGp(i);
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
        return indexToGp(emptyI);
    } else {
        RegAllocInterval *old = *active_intervals_by_start.begin();
        spillAtInterval(old->id);
        expireActiveInterval(old->id);
        active_intervals_by_start.push_back(ptr);
        active_intervals_by_end.insert(ptr);
        unspillAtInterval(id, old->activeIndex);
        active[old->activeIndex] = id;
        ptr->activeIndex = old->activeIndex;
        return indexToGp(old->activeIndex);
    }
}

void RegAlloc::expireActiveInterval(uint64_t id) {
    RegAllocInterval *ptr = intervals.find(id)->second.get();
    for (int i = 0; i < active.size(); ++i) {
        if (active[i] == id) {
            active[i] = 0;
        }
    }

    for (auto it = active_intervals_by_start.begin(); it != active_intervals_by_start.end(); ++it) {
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

void RegAlloc::unspillAtInterval(uint64_t id, uint8_t phyId) {
    const auto it = spillMap.find(id);
    if (it != spillMap.end()) {
        const x86::Gp reg = indexToGp(phyId);
        const int64_t offset = it->second;
        code.mov(reg, x86::ptr(x86::rsp, offset));
    }
}

void RegAlloc::expireOldActiveIntervals() {
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

void RegAlloc::spillAtInterval(uint64_t id) {
    uint8_t phyId = 0xFF;
    for (int i = 0; i < active.size(); ++i) {
        if (active[i] == id) {
            phyId = i;
            break;
        }
    }
    ASSERT(phyId != 0xFF);
    if (spillCursor == 0) {
        spillTop += 16;
        spillCursor = 16;
    }
    const x86::Gp reg = indexToGp(phyId);
    const uint64_t offset = spillTop - spillCursor;
    code.mov(x86::ptr(x86::rsp, offset), reg);
    spillMap[id] = offset;
    spillCursor -= 8;
}

void RegAlloc::emitStub() {
    stubOffset = code.offset();
    code.long_().sub(x86::rsp, 0);
}

void RegAlloc::patchStub() {
    const int64_t backup = code.offset();
    code.setOffset(stubOffset);
    code.long_().sub(x86::rsp, spillTop);
    code.setOffset(backup);
}

void RegAlloc::emitInit() {
    emitStub();
}

void RegAlloc::emitDeinit() {
    patchStub();
    code.add(x86::rsp, spillTop);
    for (auto offset : deinitStubOffsets) {
        const int64_t backup = code.offset();
        code.setOffset(offset);
        code.long_().add(x86::rsp, spillTop);
        code.setOffset(backup);
    }
}

void RegAlloc::emitDeinitStub() {
    deinitStubOffsets.push_back(code.offset());
    code.long_().sub(x86::rsp, 0);
}

void RegAlloc::reset() {
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
    deinitStubOffsets.clear();
    index = 0;
    spillCursor = 0;
    stubOffset = 0;
    ts = 1;
    next_id = 1;
}
