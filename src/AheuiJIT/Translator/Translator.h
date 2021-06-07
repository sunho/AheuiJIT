#pragma once
#include <AheuiJIT/IR/BasicBlock.h>
#include <AheuiJIT/IR/Builder.h>
#include <AheuiJIT/IR/Pass/Pass.h>
#include <AheuiJIT/Translator/Token.h>

#include <clocale>
#include <cstdlib>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace asmjit {
class JitRuntime;
}

namespace aheuijit {

struct Runtime;
struct JITContext;
struct Builder;

using TLBTable = std::map<Location, uint64_t>;
using TranslatedFunc = void (*)(JITContext *ctx);
using BasicBlockPtr = std::unique_ptr<BasicBlock>;

struct IRBuffer {
    IRBuffer();
    ~IRBuffer() = default;

    BasicBlock *findBlock(const Location &location);
    BasicBlock *createBlock(const Location &location);

    void print();
    void reset();

  private:
    uint64_t nextId = 1;
    std::map<Location, BasicBlockPtr> blocks;
};

struct PassManager {
    PassManager() {
    }
    ~PassManager() {
    }

    void addBasicBlockPass(std::unique_ptr<BasicBlockPass> pass) {
        basicBlockPasses.push_back(std::move(pass));
    }

    void doBasicBlockPass(Builder &b, BasicBlock *block) {
        for (auto &pass : basicBlockPasses) {
            pass->runOnBlock(b, block);
        }
    }

  private:
    std::list<std::unique_ptr<BasicBlockPass>> basicBlockPasses;
};

using AsmJitRuntimePtr =
    std::unique_ptr<asmjit::JitRuntime, std::function<void(asmjit::JitRuntime *)>>;

struct Translator {
    Translator(Runtime &rt);
    ~Translator() = default;

    TranslatedFunc translate(const std::u16string &code, TLBTable &table);
    TranslatedFunc translate(const Location &location, TLBTable &table);
    IRBuffer &debugIR(const std::u16string &code);

    static Token parseChar(const char16_t &ch);

    Location calculateFailLocation(const Location &location);
    Location updateLocationPointer(const Location &location, const TokenDir &next);
    Location addLocationX(const Location &location, int val);
    Location addLocationY(const Location &location, int val);
    Location stepLocation(const Location &location);
    Location stepToValidLocation(const Location &location);

  private:
    bool decodeToken(Builder &b, const Token &token);
#define OP(op) bool decode##op(Builder &b, const Token &token);
#include "TokenOp.inc"
#undef OP
    void buildBlocks(const Location &location);
    void setCode(const std::u16string &code);
    inline char16_t &getCode(int x, int y);
    inline char16_t &getCode(const Location &location);
    inline bool isBlank(char16_t ch);

    PassManager passManager;
    Runtime &parent;
    IRBuffer irBuffer;
    Location cur;
    size_t codeStride{ 0 };
    size_t codeHeight{ 0 };
    std::vector<size_t> codeWidth;
    std::vector<char16_t> code;
    AsmJitRuntimePtr rt;
};

}  // namespace aheuijit
