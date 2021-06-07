#pragma once

#include <list>
#include <string>

namespace aheuijit {

enum class TokenOp {
#define OP(op) op,
#include "TokenOp.inc"
#undef OP
};

static std::string describeTokenOp(TokenOp op) {
    switch (op) {
#define OP(opp)                                                                                    \
    case TokenOp::opp:                                                                             \
        return #opp;
#include "TokenOp.inc"
#undef OP
    }
    return "";
}

struct TokenDir {
    enum Value : uint8_t {
        RIGHT,
        LEFT,
        UP,
        DOWN,
        RIGHT2,
        LEFT2,
        UP2,
        DOWN2,
        VERTWALL,
        HORIWALL,
        WALL,
        NONE
    };

    TokenDir() = default;
    TokenDir(Value value) : value(value) {
    }

    bool isPrimitive() const {
        switch (value) {
            case RIGHT:
            case LEFT:
            case RIGHT2:
            case LEFT2:
            case UP:
            case DOWN:
            case UP2:
            case DOWN2:
                return true;
            default:
                return false;
        }
    }

    Pointer toPointer(bool queue) const {
        int vx = 0;
        int vy = 0;
        switch (value) {
            case RIGHT:
                vx = 1;
                break;
            case RIGHT2:
                vx = 2;
                break;
            case LEFT:
                vx = -1;
                break;
            case LEFT2:
                vx = -2;
                break;
            case DOWN:
                vy = 1;
                break;
            case DOWN2:
                vy = 2;
                break;
            case UP:
                vy = -1;
                break;
            case UP2:
                vy = -2;
                break;
        }
        return Pointer(queue, vx, vy);
    }

    bool operator<(const TokenDir &other) const {
        return value < other.value;
    }

    bool operator==(const TokenDir &other) const {
        return value == other.value;
    }

    operator Value() const {
        return value;
    }
    explicit operator bool() = delete;

  private:
    Value value{ NONE };
};

struct TokenStorage {
    enum Value : uint8_t {
        BLANK,
        GIUK,
        GIUKGIUK,
        GIUKSIOT,
        NIEN,
        NIENJIOT,
        NIENHIET,
        DIGUEK,
        RIEL,
        RIELGIUK,
        RIELMIEM,
        RIELBIEB,
        RIELSIOT,
        RIELTIGT,
        RIELPIEP,
        RIELHIET,
        MIEM,
        BIEB,
        BS,
        SIOT,
        SIOTSIOT,
        OO,
        JIOT,
        CHIOT,
        KIEK,
        TIGT,
        PIEP,
        HIET
    };

    TokenStorage() = default;
    TokenStorage(Value value) : value(value) {
    }

    operator Value() const {
        return value;
    }
    explicit operator bool() = delete;

    Storage toStorage() const {
        if (value == OO) {
            return Storage(false, 0);
        } else if (value > OO) {
            return Storage(true, value - 1);
        } else {
            return Storage(true, value);
        }
    }

    size_t toNumConst() const {
        switch (value) {
            case GIUK:
                return 2;
            case NIEN:
                return 2;
            case DIGUEK:
                return 3;
            case RIEL:
                return 5;
            case MIEM:
                return 4;
            case BIEB:
                return 4;
            case SIOT:
                return 2;
            case JIOT:
                return 3;
            case CHIOT:
                return 4;
            case KIEK:
                return 3;
            case TIGT:
                return 4;
            case PIEP:
                return 4;
            case GIUKGIUK:
                return 4;
            case GIUKSIOT:
                return 4;
            case NIENJIOT:
                return 5;
            case NIENHIET:
                return 5;
            case RIELGIUK:
                return 7;
            case RIELMIEM:
                return 9;
            case RIELBIEB:
                return 9;
            case RIELSIOT:
                return 7;
            case RIELTIGT:
                return 9;
            case RIELPIEP:
                return 9;
            case RIELHIET:
                return 8;
            case BS:
                return 6;
            case SIOTSIOT:
                return 4;
            case BLANK:
            case OO:
            case HIET:
                return 0;
        };
    }

  private:
    Value value{ BLANK };
};

struct Token {
    explicit Token(TokenOp op, TokenDir dir, TokenStorage storage)
        : op(op), dir(dir), storage(storage) {
    }
    Token() = default;
    ~Token() = default;

    TokenOp op = TokenOp::NOP;
    TokenDir dir;
    TokenStorage storage;
};

}  // namespace aheuijit
