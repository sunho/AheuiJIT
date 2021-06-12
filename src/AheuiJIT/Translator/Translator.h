#pragma once
#include <AheuiJIT/IR/BasicBlock.h>
#include <AheuiJIT/IR/Builder.h>
#include <AheuiJIT/IR/Pass/Pass.h>
#include <AheuiJIT/Runtime/IRBuffer.h>
#include <AheuiJIT/Runtime/Machine.h>
#include <AheuiJIT/Translator/Token.h>

#include <clocale>
#include <cstdlib>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace aheuijit {

struct Runtime;
struct RuntimeContext;
struct Builder;

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

using TLBTable = std::map<Location, uint64_t>;

struct Translator {
    Translator(Machine &machine);
    ~Translator() = default;

    void setCode(const std::u16string &code);

    void translate(const Location &location, IRBuffer &irBuffer);
    void emit(const Location &location, IRBuffer &irBuffer);

    void setConfig(const RuntimeConfig &conf);

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
    inline char16_t &getCode(int x, int y);
    inline char16_t &getCode(const Location &location);
    inline bool isBlank(char16_t ch);

    Machine &machine;
    PassManager passManager;
    RuntimeConfig conf;
    Location cur;
    size_t codeStride{ 0 };
    size_t codeHeight{ 0 };
    std::vector<size_t> codeWidth;
    std::vector<char16_t> code;
};

}  // namespace aheuijit
