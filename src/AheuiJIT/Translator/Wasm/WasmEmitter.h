#pragma once

#include <AheuiJIT/Runtime/Wasm/WasmMachine.h>
#include <AheuiJIT/Translator/Emitter.h>
#include <AheuiJIT/Translator/RegAlloc.h>
#include <binaryen-c.h>
#include <fmt/format.h>

#include <tuple>

#define BB(_b, expr)                                                                               \
    {                                                                                              \
        BinaryenExpressionRef r = expr;                                                            \
        (_b).add(r);                                                                               \
    }

#define LL(_b, reg) ((_b).loadReg((reg).get()))

namespace aheuijit {

constexpr size_t VIRTUAL_REGS_NUM = 16;
constexpr const char *G_STACK_POINTER = "stackPointer";
constexpr const char *G_CONTEXT = "context";
using WasmReg = Reg<uint8_t>;

struct WasmBasicBlockBuilder {
    WasmBasicBlockBuilder() = delete;
    WasmBasicBlockBuilder(BinaryenModuleRef code, RelooperRef relooper)
        : code(code), relooper(relooper) {
    }
    ~WasmBasicBlockBuilder() = default;

    void add(BinaryenExpressionRef ref) {
        refs.push_back(ref);
    }

    BinaryenExpressionRef getSp() {
        return BinaryenGlobalGet(code, G_STACK_POINTER, BinaryenTypeInt32());
    }

    BinaryenExpressionRef getContextPtr() {
        return BinaryenGlobalGet(code, G_CONTEXT, BinaryenTypeInt32());
    }

    void setSp(BinaryenExpressionRef value) {
        BB(*this, BinaryenGlobalSet(code, G_STACK_POINTER, value));
    }

    BinaryenExpressionRef loadReg(uint8_t reg) {
        return BinaryenLocalGet(code, reg + 3, BinaryenTypeInt64());
    }

    void storeReg(uint8_t reg, BinaryenExpressionRef value) {
        BB(*this, BinaryenLocalSet(code, reg + 3, value));
    }

    BinaryenExpressionRef makeInt32(int32_t value) {
        return BinaryenConst(code, BinaryenLiteralInt32(value));
    }

    BinaryenExpressionRef makeInt64(int64_t value) {
        return BinaryenConst(code, BinaryenLiteralInt64(value));
    }

    RelooperBlockRef flushSwitch(BinaryenExpressionRef condition) {
        BinaryenExpressionRef cc = BinaryenBlock(code, fmt::format("bb{}", nextId++).c_str(),
                                                 refs.data(), refs.size(), BinaryenTypeAuto());
        RelooperBlockRef out = RelooperAddBlockWithSwitch(relooper, cc, condition);
        refs.clear();
        return out;
    }

    RelooperBlockRef flush(bool connect = true, bool setTail = true) {
        BinaryenExpressionRef cc = BinaryenBlock(code, fmt::format("bb{}", nextId++).c_str(),
                                                 refs.data(), refs.size(), BinaryenTypeAuto());
        RelooperBlockRef out = RelooperAddBlock(relooper, cc);
        if (!head) {
            head = out;
        }
        if (connect) {
            if (tail) {
                RelooperAddBranch(tail, out, NULL, NULL);
            }
        }
        if (setTail)
            tail = out;
        refs.clear();
        return out;
    }

    RelooperBlockRef getHead() {
        return head;
    }

    RelooperBlockRef getTail() {
        return tail;
    }

    void resetHead() {
        head = NULL;
        tail = NULL;
    }

  private:
    RelooperRef relooper;
    RelooperBlockRef head{ NULL };
    RelooperBlockRef tail{ NULL };
    std::vector<BinaryenExpressionRef> refs;
    BinaryenModuleRef code;
    size_t nextId = 1;
};

struct WasmRegAllocProtocol : public RegAllocProtocol<uint8_t> {
    WasmRegAllocProtocol() = delete;
    explicit WasmRegAllocProtocol(WasmBasicBlockBuilder &builder, BinaryenModuleRef code)
        : builder(builder), code(code) {
    }

    void emitStore(uint32_t offset, uint8_t reg) override {
        BinaryenExpressionRef value = builder.loadReg(reg);
        BB(builder, BinaryenStore(code, 8, offset, 0, builder.getSp(), value, BinaryenTypeInt64()));
    }

    void emitLoad(uint8_t reg, uint32_t offset) override {
        BinaryenExpressionRef value =
            BinaryenLoad(code, 8, true, offset, 0, BinaryenTypeInt64(), builder.getSp());
        builder.storeReg(reg, value);
    }

    void emitInitStub() override {
        BinaryenExpressionRef size = builder.makeInt32(0);
        stubConstant = size;
        BinaryenExpressionRef newSp =
            BinaryenBinary(code, BinaryenSubInt32(), builder.getSp(), size);
        builder.setSp(newSp);
    }

    void emitDeinitStub() override {
        BinaryenExpressionRef size = builder.makeInt32(0);
        deinitStubConstants.push_back(size);
        BinaryenExpressionRef newSp =
            BinaryenBinary(code, BinaryenAddInt32(), builder.getSp(), size);
        builder.setSp(newSp);
    }

    void finalize(uint32_t size) override {
        BinaryenConstSetValueI32(stubConstant, size);
        for (auto c : deinitStubConstants) {
            BinaryenConstSetValueI32(c, size);
        }
        deinitStubConstants.clear();
    }

    uint8_t convertToReg(uint8_t index) override {
        return index;
    }

    uint8_t convertFromReg(uint8_t reg) override {
        return reg;
    }

  private:
    BinaryenExpressionRef stubConstant;
    std::list<BinaryenExpressionRef> deinitStubConstants;
    BinaryenModuleRef code;
    WasmBasicBlockBuilder &builder;
};

struct WasmEmitter : public Emitter {
    WasmEmitter() = delete;

    explicit WasmEmitter(BinaryenModuleRef code)
        : code(code)
        , builder(code, relooper)
        , regAlloc(std::make_unique<WasmRegAllocProtocol>(builder, code))
        , relooper(RelooperCreate(code)){};

    ~WasmEmitter() = default;

    void emit(BasicBlock *bb, const TLBTable &table, std::set<BasicBlock *> &emitted) override;
    void visit(Instruction *inst);

#define INST(name, ret, ...) void name(__VA_ARGS__);
#include <AheuiJIT/IR/Instruction.inc>
#undef INST

  private:
    void initModule();
    void finalizeModule(BinaryenExpressionRef body);
    void emitPrologue();
    void emitEpilouge();
    void emitJITRequest(BinaryenExpressionRef head, BinaryenExpressionRef tail, bool i64);
    WasmReg unwrapValue(Value *value);

    bool popFixup{ false };
    BinaryenModuleRef code;
    RelooperRef relooper;
    WasmBasicBlockBuilder builder;
    BasicBlock *block{ nullptr };
    Instruction *inst{ nullptr };

    std::list<RelooperBlockRef> exitRequests;

    RegAlloc<uint8_t, VIRTUAL_REGS_NUM> regAlloc;
};

}  // namespace aheuijit
