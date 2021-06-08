#pragma once

#include <cstdlib>
#include <string>

namespace aheuijit {

struct Pointer {
    Pointer() = default;
    explicit Pointer(bool queue, int vx, int vy) : queue(queue), vx(vx), vy(vy) {
    }
    ~Pointer() = default;
    int queue{ false };
    int vx{ 0 };
    int vy{ 0 };

    bool isVertical() const;
    bool isHorizontal() const;

    void flip();
    bool operator<(const Pointer &other) const;
    bool operator==(const Pointer &other) const;
};

struct Location {
    explicit Location(int x, int y, bool queue, int vx, int vy)
        : x(x), y(y), pointer(queue, vx, vy) {
    }
    static Location unpack(uint64_t packed);
    Location() = default;
    ~Location() = default;
    int x{ 0 };
    int y{ 0 };
    Pointer pointer;

    uint64_t hash() const;
    uint64_t pack() const;

    bool operator<(const Location &other) const;
    bool operator==(const Location &other) const;
    bool operator!=(const Location &other) const;
    std::string description() const;
};

static_assert(sizeof(Location) == 20);

static const Location DEFAULT_LOCATION = Location(0, 0, false, 0, 1);

}  // namespace aheuijit
