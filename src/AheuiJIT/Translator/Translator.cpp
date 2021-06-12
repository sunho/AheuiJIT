#include "Translator.h"

#ifdef __EMSCRIPTEN__
#include <AheuiJIT/Translator/Wasm/WasmEmitter.h>
#else
#include <AheuiJIT/Translator/X86/X86Emitter.h>
#endif
#include <AheuiJIT/IR/Builder.h>
#include <AheuiJIT/Translator/Emitter.h>
#include <AheuiJIT/Util/Disasm.h>
#include <AheuiJIT/Util/Util.h>
#include <fmt/printf.h>

#include <algorithm>
#include <iostream>
#include <stack>

using namespace aheuijit;

#ifdef __EMSCRIPTEN__
static std::unique_ptr<Emitter> createEmitterImpl(CodeBlock *block) {
    WasmCodeBlock *wasmblock = dynamic_cast<WasmCodeBlock *>(block);
    return std::make_unique<WasmEmitter>(wasmblock->getModule());
}
#else
static std::unique_ptr<Emitter> createEmitterImpl(CodeBlock *block) {
    X86CodeBlock *x86block = dynamic_cast<X86CodeBlock *>(block);
    return std::make_unique<X86Emitter>(x86block->getAssembler());
}
#endif

Translator::Translator(Machine &machine) : machine(machine) {
    passManager.addBasicBlockPass(std::make_unique<AggregatePushPopPass>());
    passManager.addBasicBlockPass(std::make_unique<RemoveDeadStoreUpdate>());
    passManager.addBasicBlockPass(std::make_unique<ConstantFoldPass>());
    passManager.addBasicBlockPass(std::make_unique<EndpointPass>());
    passManager.addBasicBlockPass(std::make_unique<RemoveDeadAssignmentPass>());
}

Token Translator::parseChar(const char16_t &ch) {
    static TokenOp TOKEN_OP_TABLE[] = { TokenOp::NOP, TokenOp::NOP,    TokenOp::DIV, TokenOp::ADD,
                                        TokenOp::MUL, TokenOp::MOD,    TokenOp::POP, TokenOp::PUSH,
                                        TokenOp::DUP, TokenOp::SELECT, TokenOp::MOV, TokenOp::NOP,
                                        TokenOp::CMP, TokenOp::NOP,    TokenOp::CBZ, TokenOp::NOP,
                                        TokenOp::SUB, TokenOp::CHANGE, TokenOp::EXIT };

    static TokenDir TOKEN_DIR_TABLE[] = { TokenDir::RIGHT,    TokenDir::NONE, TokenDir::RIGHT2,
                                          TokenDir::NONE,     TokenDir::LEFT, TokenDir::NONE,
                                          TokenDir::LEFT2,    TokenDir::NONE, TokenDir::UP,
                                          TokenDir::NONE,     TokenDir::NONE, TokenDir::NONE,
                                          TokenDir::UP2,      TokenDir::DOWN, TokenDir::NONE,
                                          TokenDir::NONE,     TokenDir::NONE, TokenDir::DOWN2,
                                          TokenDir::HORIWALL, TokenDir::WALL, TokenDir::VERTWALL };

    const wchar_t chNorm = ch - 0xAC00;
    const size_t cho = static_cast<size_t>(chNorm / 28 / 21);
    const size_t jung = static_cast<size_t>((chNorm / 28) % 21);
    const TokenStorage::Value jong = static_cast<TokenStorage::Value>(chNorm % 28);

    return Token(TOKEN_OP_TABLE[cho], TOKEN_DIR_TABLE[jung], TokenStorage(jong));
};

void Translator::setCode(const std::u16string &code) {
    codeStride = 0;
    codeHeight = 1;
    size_t cursor = 0;
    for (int i = 0; i < code.size(); ++i) {
        if (code[i] == u'\n') {
            codeStride = std::max(cursor, codeStride);
            ++codeHeight;
            cursor = 0;
        } else {
            ++cursor;
        }
    }

    codeStride = std::max(cursor, codeStride);
    this->code.resize(codeHeight * codeStride);
    codeWidth.resize(codeHeight);
    std::fill(this->code.begin(), this->code.end(), u' ');
    std::fill(codeWidth.begin(), codeWidth.end(), 0);

    cursor = 0;
    size_t y = 0;
    for (int i = 0; i < code.size(); ++i) {
        if (code[i] == u'\n') {
            codeWidth[y] = cursor;
            ++y;
            cursor = 0;
        } else {
            getCode(y, cursor) = code[i];
            ++cursor;
        }
    }

    if (cursor != 0) {
        codeWidth[y] = cursor;
    }
}

inline bool Translator::isBlank(char16_t ch) {
    return !(ch >= 0xac00 && ch <= 0xd7a3);
}

inline char16_t &Translator::getCode(const Location &location) {
    return this->code[codeStride * location.y + location.x];
}

inline char16_t &Translator::getCode(int y, int x) {
    return this->code[codeStride * y + x];
}

Location Translator::calculateFailLocation(const Location &location) {
    Location out = location;
    const Token token = parseChar(getCode(location));
    out = updateLocationPointer(out, token.dir);
    out.pointer.flip();
    return stepLocation(out);
}

void Translator::setConfig(const RuntimeConfig &conf) {
    this->conf = conf;
}

Location Translator::stepToValidLocation(const Location &location) {
    Location out = location;
    while (isBlank(getCode(out))) {
        out = stepLocation(out);
    }
    return out;
}

void Translator::translate(const Location &location, IRBuffer &irBuffer) {
    std::stack<std::tuple<Location, BasicBlock **>> trStack;
    trStack.push(std::make_tuple(location, nullptr));

    Builder b;
    while (!trStack.empty()) {
        auto [loc_, label_] = trStack.top();
        trStack.pop();
        cur = stepToValidLocation(loc_);
        BasicBlock *bb = irBuffer.findBlock(cur);
        if (bb) {
            if (label_) {
                *label_ = bb;
            }
            continue;
        }
        bb = irBuffer.createBlock(cur);
        bb->location = cur;
        BasicBlock *nextBlock = nullptr;
        b.setBasicBlock(bb);
        bool notTermed;
        bool loop = false;
        Token token;
        std::set<Location> visited;
        do {
            b.setCurrentLocation(cur);
            visited.insert(cur);
            token = parseChar(getCode(cur));
            LOG("Translating x: {} y: {} op: {} orig: {}", cur.x, cur.y, describeTokenOp(token.op),
                covnert_utf16_to_utf8(getCode(cur)))
            notTermed = decodeToken(b, token);
            if (notTermed) {
                cur = updateLocationPointer(cur, token.dir);
                cur = stepLocation(cur);
            }
            cur = stepToValidLocation(cur);
            nextBlock = irBuffer.findBlock(cur);
            loop = visited.find(cur) != visited.end();
        } while (notTermed && !nextBlock && !loop);

        if (!notTermed) {
            if (bb->terminal->getType() == TerminalType::Link) {
                LinkTerminal *term = dynamic_cast<LinkTerminal *>(bb->terminal);
                term->block = nextBlock;
            } else if (bb->terminal->getType() == TerminalType::Conditional) {
                ConditionalTerminal *term = dynamic_cast<ConditionalTerminal *>(bb->terminal);
                Location next = updateLocationPointer(cur, token.dir);
                trStack.push(std::make_tuple(stepLocation(next), &term->fail));
                next.pointer.flip();
                trStack.push(std::make_tuple(stepLocation(next), &term->pass));
            }
        } else if (nextBlock) {
            b.setLinkTerminal(nextBlock);
        } else if (loop) {
            b.setLinkTerminal(nullptr);
            LinkTerminal *term = dynamic_cast<LinkTerminal *>(bb->terminal);
            trStack.push(std::make_tuple(cur, &term->block));
        }
        if (conf.optIR) {
            passManager.doBasicBlockPass(b, bb);
        }

        if (label_) {
            *label_ = bb;
        }
    }
}

void Translator::emit(const Location &location, IRBuffer &irBuffer) {
    CodeBlockPtr codeblock = machine.createBlock();
    std::unique_ptr<Emitter> emitter = createEmitterImpl(codeblock.get());
    emitter->setConfig(conf);
    const auto block = irBuffer.findBlock(location);
    std::set<BasicBlock *> emitted;
    emitter->emit(block, machine.tlbTable, emitted);
    machine.addCodeBlock(location, codeblock.get(), emitted);
}

Location Translator::addLocationX(const Location &location, int val) {
    if (!val)
        return location;
    Location out = location;
    out.x += val;

    if (out.x < 0) {
        out.x = codeWidth[out.y] + out.x;
    }
    if (out.x >= codeWidth[out.y]) {
        out.x = out.x - codeWidth[out.y];
    }
    return out;
}

Location Translator::addLocationY(const Location &location, int val) {
    if (!val)
        return location;
    Location out = location;
    out.y += val;

    if (out.y < 0) {
        out.y = codeHeight + out.y;
    }
    if (out.y >= codeHeight) {
        out.y = out.y - codeHeight;
    }
    return out;
}

Location Translator::stepLocation(const Location &location) {
    Location out = location;
    out = addLocationX(out, out.pointer.vx);
    out = addLocationY(out, out.pointer.vy);
    return out;
}

Location Translator::updateLocationPointer(const Location &location, const TokenDir &next) {
    Location out = location;
    switch (next) {
        case TokenDir::VERTWALL:
            if (out.pointer.isHorizontal()) {
                out.pointer.flip();
            }
            break;
        case TokenDir::HORIWALL:
            if (out.pointer.isVertical()) {
                out.pointer.flip();
            }
            break;
        case TokenDir::WALL:
            out.pointer.flip();
            break;
        case TokenDir::NONE:
            break;
        default:
            out.pointer = next.toPointer(out.pointer.queue);
    };
    return out;
}

bool Translator::decodeToken(Builder &b, const Token &token) {
    switch (token.op) {
#define OP(op)                                                                                     \
    case TokenOp::op:                                                                              \
        return decode##op(b, token);
#include "TokenOp.inc"
#undef OP
        default:
            fmt::print("Unknown aheui opcode: {}", token.op);
            return false;
    };
}
