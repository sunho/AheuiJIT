#include "Location.h"

#include <fmt/format.h>

using namespace aheuijit;

inline uint64_t hash(uint64_t key) {
    uint64_t v = key * 3935559000370003845 + 2691343689449507681;

    v ^= v >> 21;
    v ^= v << 37;
    v ^= v >> 4;

    v *= 4768777513237032717;

    v ^= v << 20;
    v ^= v >> 41;
    v ^= v << 5;

    return v;
}

bool Pointer::isVertical() const {
    return vy != 0;
}

bool Pointer::isHorizontal() const {
    return vx != 0;
}

void Pointer::flip() {
    vx *= -1;
    vy *= -1;
}

bool Pointer::operator<(const Pointer &other) const {
    if (queue == other.queue) {
        if (vx == other.vx) {
            return vy < other.vy;
        }
        return vx < other.vx;
    }
    return queue < other.queue;
}

bool Pointer::operator==(const Pointer &other) const {
    return queue == other.queue && vx == other.vx && vy == other.vy;
}

uint64_t Location::hash() const {
    return ::hash(((uint64_t)x & 0xFFFFFF) | (((uint64_t)y & 0xFFFFFF) << 24) |
                  ((uint64_t)pointer.queue << 48) | ((uint64_t)(pointer.vx + 2) << 52) |
                  ((uint64_t)(pointer.vy + 2) << 56));
}

bool Location::operator<(const Location &other) const {
    if (x == other.x) {
        if (y == other.y) {
            return pointer < other.pointer;
        }
        return y < other.y;
    }
    return x < other.x;
}

bool Location::operator==(const Location &other) const {
    return x == other.x && y == other.y && pointer == other.pointer;
}

bool Location::operator!=(const Location &other) const {
    return !(*this == other);
}

std::string Location::description() const {
    return fmt::format("(x: {}, y: {}, queue: {}, vx: {}, vy: {})", x, y, pointer.queue, pointer.vx,
                       pointer.vy);
}
