#include "WasmEmitter.h"

#include <stack>

using namespace aheuijit;

void WasmEmitter::initModule() {
    BinaryenModuleAutoDrop(code);
    BinaryenAddGlobal(code, G_STACK_POINTER, BinaryenTypeInt32(), 1, builder.makeInt32(0));
    BinaryenAddGlobal(code, G_CONTEXT, BinaryenTypeInt32(), 1, builder.makeInt32(0));
    BinaryenAddFunctionImport(code, "printChar", "js", "printChar", BinaryenTypeInt32(),
                              BinaryenTypeNone());
    BinaryenAddFunctionImport(code, "printNum", "js", "printNum", BinaryenTypeInt32(),
                              BinaryenTypeNone());
    BinaryenAddFunctionImport(code, "inputChar", "js", "inputChar", BinaryenTypeNone(),
                              BinaryenTypeInt32());
    BinaryenAddFunctionImport(code, "inputNum", "js", "inputNum", BinaryenTypeNone(),
                              BinaryenTypeInt32());
    const char *segments[] = { 0 };
    bool segmentPassive[] = { false };
    BinaryenExpressionRef segmentOffsets[] = { 0 };
    BinaryenIndex segmentSizes[] = { 0 };
    BinaryenAddMemoryImport(code, "memory", "js", "memory", false);
    BinaryenSetMemory(code, conf.wasmMemorySize / WASM_PAGE_SIZE,
                      conf.wasmMemorySize / WASM_PAGE_SIZE, nullptr, segments, segmentPassive,
                      segmentOffsets, segmentSizes, 0, false);
}

void WasmEmitter::finalizeModule(BinaryenExpressionRef body) {
    std::vector<BinaryenType> localTypes;
    for (size_t i = 0; i < VIRTUAL_REGS_NUM; ++i) {
        localTypes.push_back(BinaryenTypeInt64());
    }
    BinaryenType paramTypes[] = { BinaryenTypeInt32(), BinaryenTypeInt32() };
    BinaryenFunctionRef entry =
        BinaryenAddFunction(code, "main", BinaryenTypeCreate(paramTypes, 2), BinaryenTypeNone(),
                            localTypes.data(), localTypes.size(), body);
    BinaryenAddExport(code, "main", "main");
    ASSERT(BinaryenModuleValidate(code))
    if (conf.optAsm) {
        BinaryenModuleOptimize(code);
        ASSERT(BinaryenModuleValidate(code))
    }
    if (conf.logDisasm) {
        BinaryenModulePrint(code);
    }
}

void WasmEmitter::emit(BasicBlock *bb, const TLBTable &table, std::set<BasicBlock *> &emitted) {
    std::stack<BasicBlock *> todo;
    std::set<uint64_t> done;
    std::list<std::tuple<uint64_t, uint64_t, BinaryenExpressionRef>> blockEdges;
    std::map<uint64_t, RelooperBlockRef> heads;
    std::map<uint64_t, RelooperBlockRef> tails;
    todo.push(bb);

    initModule();
    emitPrologue();
    builder.flush();
    RelooperBlockRef startBlock = builder.getHead();
    builder.resetHead();

    while (!todo.empty()) {
        block = todo.top();
        todo.pop();
        if (done.find(block->id) != done.end()) {
            continue;
        }
        if (conf.logIR) {
            fmt::print("{}", block->description());
        }
        emitted.insert(block);
        regAlloc.emitInitStub();

        Location prevLocation;
        popFixup = false;
        for (auto inst : block->insts) {
            if (inst->location != prevLocation) {
                popFixup = false;
            }
            regAlloc.setInstructionIndex(inst->offset);
            visit(inst);
            prevLocation = inst->location;
        }

        done.insert(block->id);
        const auto finalize = [&]() {
            regAlloc.emitDeinitStub();
            regAlloc.finalize();
            regAlloc.reset();
            builder.flush();
            RelooperBlockRef out = builder.getTail() ? builder.getTail() : builder.getHead();
            tails.emplace(block->id, out);
            heads.emplace(block->id, builder.getHead());
            builder.resetHead();
            return out;
        };
        switch (block->terminal->getType()) {
            case TerminalType::Exit: {
                RelooperBlockRef rb = finalize();
                exitRequests.push_back(rb);
                break;
            }
            case TerminalType::Link: {
                finalize();
                LinkTerminal *term = dynamic_cast<LinkTerminal *>(block->terminal);
                blockEdges.push_back({ block->id, term->block->id, NULL });
                todo.push(term->block);
                break;
            }
            case TerminalType::Conditional: {
                ConditionalTerminal *term = dynamic_cast<ConditionalTerminal *>(block->terminal);
                BinaryenExpressionRef pred = LL(builder, unwrapValue(term->predicate));
                pred = BinaryenUnary(code, BinaryenEqZInt64(), pred);
                blockEdges.push_back({ block->id, term->pass->id, pred });
                blockEdges.push_back({ block->id, term->fail->id, NULL });
                todo.push(term->pass);
                todo.push(term->fail);
                finalize();
                break;
            }
        }
    }
    emitEpilouge();
    builder.flush();
    RelooperBlockRef exitBlock = builder.getHead();
    for (auto b : exitRequests) {
        RelooperAddBranch(b, builder.getHead(), NULL, NULL);
    }
    for (auto edge : blockEdges) {
        const auto [from, to, pred] = edge;
        RelooperAddBranch(tails.at(from), heads.at(to), pred, NULL);
    }

    RelooperAddBranch(startBlock, heads.at(bb->id), NULL, NULL);
    BinaryenExpressionRef body = RelooperRenderAndDispose(relooper, startBlock, 0);
    finalizeModule(body);
}

WasmReg WasmEmitter::unwrapValue(Value *value) {
    if (value->type() == ValueType::Local) {
        return regAlloc.allocLocal(dynamic_cast<Local *>(value));
    } else {
        Constant *c = dynamic_cast<Constant *>(value);
        WasmReg reg = regAlloc.allocTmp();
        builder.storeReg(reg.get(), builder.makeInt64(c->imm));
        return reg;
    }
}

void WasmEmitter::visit(Instruction *inst) {
    InstructionVisitorCaller<WasmEmitter> caller;
    this->inst = inst;
    caller.call(*this, inst);
}

void WasmEmitter::add(Value *lhs, Value *rhs) {
    WasmReg dest = regAlloc.allocLocal(inst->output);
    WasmReg a = unwrapValue(lhs);
    WasmReg b = unwrapValue(rhs);
    BinaryenExpressionRef out =
        BinaryenBinary(code, BinaryenAddInt64(), LL(builder, a), LL(builder, b));
    builder.storeReg(dest.get(), out);
}

void WasmEmitter::sub(Value *lhs, Value *rhs) {
    WasmReg dest = regAlloc.allocLocal(inst->output);
    WasmReg a = unwrapValue(lhs);
    WasmReg b = unwrapValue(rhs);
    BinaryenExpressionRef out =
        BinaryenBinary(code, BinaryenSubInt64(), LL(builder, a), LL(builder, b));
    builder.storeReg(dest.get(), out);
}

void WasmEmitter::mul(Value *lhs, Value *rhs) {
    WasmReg dest = regAlloc.allocLocal(inst->output);
    WasmReg a = unwrapValue(lhs);
    WasmReg b = unwrapValue(rhs);
    BinaryenExpressionRef out =
        BinaryenBinary(code, BinaryenMulInt64(), LL(builder, a), LL(builder, b));
    builder.storeReg(dest.get(), out);
}

void WasmEmitter::div(Value *lhs, Value *rhs) {
    WasmReg dest = regAlloc.allocLocal(inst->output);
    WasmReg a = unwrapValue(lhs);
    WasmReg b = unwrapValue(rhs);
    BinaryenExpressionRef out =
        BinaryenBinary(code, BinaryenDivSInt64(), LL(builder, a), LL(builder, b));
    builder.storeReg(dest.get(), out);
}

void WasmEmitter::mod(Value *lhs, Value *rhs) {
    WasmReg dest = regAlloc.allocLocal(inst->output);
    WasmReg a = unwrapValue(lhs);
    WasmReg b = unwrapValue(rhs);
    BinaryenExpressionRef out =
        BinaryenBinary(code, BinaryenRemSInt64(), LL(builder, a), LL(builder, b));
    builder.storeReg(dest.get(), out);
}

void WasmEmitter::cmp(Value *lhs, Value *rhs) {
    WasmReg dest = regAlloc.allocLocal(inst->output);
    WasmReg a = unwrapValue(lhs);
    WasmReg b = unwrapValue(rhs);
    BinaryenExpressionRef pred =
        BinaryenBinary(code, BinaryenGeSInt64(), LL(builder, a), LL(builder, b));
    BinaryenExpressionRef f = builder.makeInt64(0);
    BinaryenExpressionRef t = builder.makeInt64(1);
    BinaryenExpressionRef out = BinaryenSelect(code, pred, t, f, BinaryenTypeInt64());
    builder.storeReg(dest.get(), out);
}

void WasmEmitter::setStore(Value *value) {
    WasmReg strReg = unwrapValue(value);
    BinaryenExpressionRef str = BinaryenUnary(code, BinaryenWrapInt64(), LL(builder, strReg));
    BB(builder, BinaryenStore(code, 4, offsetof(RuntimeContext, storage), 0,
                              builder.getContextPtr(), str, BinaryenTypeInt32()))
}

void WasmEmitter::getStore(Void *) {
    WasmReg dest = regAlloc.allocLocal(inst->output);
    BinaryenExpressionRef str = BinaryenLoad(code, 4, true, offsetof(RuntimeContext, storage), 0,
                                             BinaryenTypeInt32(), builder.getContextPtr());
    str = BinaryenUnary(code, BinaryenExtendSInt32(), str);
    builder.storeReg(dest.get(), str);
}

void WasmEmitter::pushQueueBack(Value *value) {
    WasmReg valueReg = unwrapValue(value);
    BinaryenExpressionRef queueFront =
        BinaryenLoad(code, 8, true, offsetof(RuntimeContext, queueFront), 0, BinaryenTypeInt64(),
                     builder.getContextPtr());
    BinaryenExpressionRef queueFrontOffset =
        BinaryenBinary(code, BinaryenMulInt64(), queueFront, builder.makeInt64(8));
    queueFrontOffset = BinaryenUnary(code, BinaryenWrapInt64(), queueFrontOffset);
    BinaryenExpressionRef queueBufferPtr =
        BinaryenLoad(code, 4, true, offsetof(RuntimeContext, queueBuffer), 0, BinaryenTypeInt32(),
                     builder.getContextPtr());
    BinaryenExpressionRef queueFrontPtr =
        BinaryenBinary(code, BinaryenAddInt32(), queueBufferPtr, queueFrontOffset);
    BB(builder,
       BinaryenStore(code, 8, 0, 0, queueFrontPtr, LL(builder, valueReg), BinaryenTypeInt64()));
    queueFront = BinaryenExpressionCopy(queueFront, code);
    queueFront = BinaryenBinary(code, BinaryenAddInt64(), queueFront, builder.makeInt64(1));
    queueFront = BinaryenBinary(code, BinaryenAndInt64(), queueFront,
                                builder.makeInt64(conf.maxStorageSize - 1));
    BB(builder, BinaryenStore(code, 8, offsetof(RuntimeContext, queueFront), 0,
                              builder.getContextPtr(), queueFront, BinaryenTypeInt64()))
}

void WasmEmitter::pushQueueFront(Value *value) {
    WasmReg valueReg = unwrapValue(value);
    BinaryenExpressionRef queueBack =
        BinaryenLoad(code, 8, true, offsetof(RuntimeContext, queueBack), 0, BinaryenTypeInt64(),
                     builder.getContextPtr());
    BinaryenExpressionRef queueBackOffset =
        BinaryenBinary(code, BinaryenMulInt64(), queueBack, builder.makeInt64(8));
    queueBackOffset = BinaryenUnary(code, BinaryenWrapInt64(), queueBackOffset);
    BinaryenExpressionRef queueBufferPtr =
        BinaryenLoad(code, 4, true, offsetof(RuntimeContext, queueBuffer), 0, BinaryenTypeInt32(),
                     builder.getContextPtr());
    BinaryenExpressionRef queueBackPtr =
        BinaryenBinary(code, BinaryenAddInt32(), queueBufferPtr, queueBackOffset);
    BB(builder,
       BinaryenStore(code, 8, 0, 0, queueBackPtr, LL(builder, valueReg), BinaryenTypeInt64()));
    queueBack = BinaryenExpressionCopy(queueBack, code);
    queueBack = BinaryenBinary(code, BinaryenSubInt64(), queueBack, builder.makeInt64(1));
    queueBack = BinaryenBinary(code, BinaryenAndInt64(), queueBack,
                               builder.makeInt64(conf.maxStorageSize - 1));
    BB(builder, BinaryenStore(code, 8, offsetof(RuntimeContext, queueBack), 0,
                              builder.getContextPtr(), queueBack, BinaryenTypeInt64()))
}

void WasmEmitter::popQueue(Void *) {
    WasmReg dest = regAlloc.allocLocal(inst->output);
    BinaryenExpressionRef queueBackBefore =
        BinaryenLoad(code, 8, true, offsetof(RuntimeContext, queueBack), 0, BinaryenTypeInt64(),
                     builder.getContextPtr());
    BinaryenExpressionRef queueBack =
        BinaryenBinary(code, BinaryenAddInt64(), queueBackBefore, builder.makeInt64(1));
    queueBack = BinaryenBinary(code, BinaryenAndInt64(), queueBack,
                               builder.makeInt64(conf.maxStorageSize - 1));
    BinaryenExpressionRef queueFront =
        BinaryenLoad(code, 8, true, offsetof(RuntimeContext, queueFront), 0, BinaryenTypeInt64(),
                     builder.getContextPtr());
    emitJITRequest(queueBack, queueFront, true);
    queueBack = BinaryenExpressionCopy(queueBack, code);
    BinaryenExpressionRef queueBackOffset =
        BinaryenBinary(code, BinaryenMulInt64(), queueBack, builder.makeInt64(8));
    queueBackOffset = BinaryenUnary(code, BinaryenWrapInt64(), queueBackOffset);
    BinaryenExpressionRef queueBufferPtr =
        BinaryenLoad(code, 4, true, offsetof(RuntimeContext, queueBuffer), 0, BinaryenTypeInt32(),
                     builder.getContextPtr());
    BinaryenExpressionRef queueBackPtr =
        BinaryenBinary(code, BinaryenAddInt32(), queueBufferPtr, queueBackOffset);
    BinaryenExpressionRef value =
        BinaryenLoad(code, 8, true, 0, 0, BinaryenTypeInt64(), queueBackPtr);
    builder.storeReg(dest.get(), value);
    queueBack = BinaryenExpressionCopy(queueBack, code);
    BB(builder, BinaryenStore(code, 8, offsetof(RuntimeContext, queueBack), 0,
                              builder.getContextPtr(), queueBack, BinaryenTypeInt64()))
    popFixup = true;
}

void WasmEmitter::pushStack(Value *value) {
    WasmReg valueReg = unwrapValue(value);
    BinaryenExpressionRef str = BinaryenLoad(code, 4, true, offsetof(RuntimeContext, storage), 0,
                                             BinaryenTypeInt32(), builder.getContextPtr());
    str = BinaryenBinary(code, BinaryenMulInt32(), str, builder.makeInt32(4));
    BinaryenExpressionRef stackTopPtr =
        BinaryenBinary(code, BinaryenAddInt32(), builder.getContextPtr(),
                       builder.makeInt32(offsetof(RuntimeContext, stackTops)));
    stackTopPtr = BinaryenBinary(code, BinaryenAddInt32(), stackTopPtr, str);
    BinaryenExpressionRef stackUpperPtr =
        BinaryenBinary(code, BinaryenAddInt32(), builder.getContextPtr(),
                       builder.makeInt32(offsetof(RuntimeContext, stackUppers)));
    stackUpperPtr = BinaryenBinary(code, BinaryenAddInt32(), stackUpperPtr, str);
    BinaryenExpressionRef stackTop =
        BinaryenLoad(code, 4, true, 0, 0, BinaryenTypeInt32(), stackTopPtr);
    stackTop = BinaryenBinary(code, BinaryenSubInt32(), stackTop, builder.makeInt32(8));
    BB(builder, BinaryenStore(code, 8, 0, 0, stackTop, LL(builder, valueReg), BinaryenTypeInt64()))
    stackTopPtr = BinaryenExpressionCopy(stackTopPtr, code);
    stackTop = BinaryenExpressionCopy(stackTop, code);
    BB(builder, BinaryenStore(code, 4, 0, 0, stackTopPtr, stackTop, BinaryenTypeInt32()))
}

void WasmEmitter::popStack(Void *) {
    WasmReg dest = regAlloc.allocLocal(inst->output);
    BinaryenExpressionRef str = BinaryenLoad(code, 4, true, offsetof(RuntimeContext, storage), 0,
                                             BinaryenTypeInt32(), builder.getContextPtr());
    str = BinaryenBinary(code, BinaryenMulInt32(), str, builder.makeInt32(4));
    BinaryenExpressionRef stackTopPtr =
        BinaryenBinary(code, BinaryenAddInt32(), builder.getContextPtr(),
                       builder.makeInt32(offsetof(RuntimeContext, stackTops)));
    stackTopPtr = BinaryenBinary(code, BinaryenAddInt32(), stackTopPtr, str);
    BinaryenExpressionRef stackUpperPtr =
        BinaryenBinary(code, BinaryenAddInt32(), builder.getContextPtr(),
                       builder.makeInt32(offsetof(RuntimeContext, stackUppers)));
    stackUpperPtr = BinaryenBinary(code, BinaryenAddInt32(), stackUpperPtr, str);
    BinaryenExpressionRef stackUpper =
        BinaryenLoad(code, 4, true, 0, 0, BinaryenTypeInt32(), stackUpperPtr);
    BinaryenExpressionRef stackTop =
        BinaryenLoad(code, 4, true, 0, 0, BinaryenTypeInt32(), stackTopPtr);
    emitJITRequest(stackUpper, stackTop, false);
    stackTop = BinaryenExpressionCopy(stackTop, code);
    BinaryenExpressionRef value = BinaryenLoad(code, 8, true, 0, 0, BinaryenTypeInt64(), stackTop);
    builder.storeReg(dest.get(), value);
    stackTopPtr = BinaryenExpressionCopy(stackTopPtr, code);
    stackTop = BinaryenExpressionCopy(stackTop, code);
    stackTop = BinaryenBinary(code, BinaryenAddInt32(), stackTop, builder.makeInt32(8));
    BB(builder, BinaryenStore(code, 4, 0, 0, stackTopPtr, stackTop, BinaryenTypeInt32()))
    popFixup = true;
}

void WasmEmitter::emitJITRequest(BinaryenExpressionRef head, BinaryenExpressionRef tail, bool i64) {
    BinaryenExpressionRef pred =
        BinaryenBinary(code, i64 ? BinaryenEqInt64() : BinaryenEqInt32(), head, tail);
    RelooperBlockRef body = builder.flush(true, true);
    if (popFixup) {
        if (inst->location.pointer.queue) {
            BinaryenExpressionRef queueBack =
                BinaryenLoad(code, 8, true, offsetof(RuntimeContext, queueBack), 0,
                             BinaryenTypeInt64(), builder.getContextPtr());
            queueBack = BinaryenBinary(code, BinaryenSubInt64(), queueBack, builder.makeInt64(1));
            queueBack = BinaryenBinary(code, BinaryenAndInt64(), queueBack,
                                       builder.makeInt64(conf.maxStorageSize - 1));
            BB(builder, BinaryenStore(code, 8, offsetof(RuntimeContext, queueBack), 0,
                                      builder.getContextPtr(), queueBack, BinaryenTypeInt64()))
        } else {
            BinaryenExpressionRef str =
                BinaryenLoad(code, 4, true, offsetof(RuntimeContext, storage), 0,
                             BinaryenTypeInt32(), builder.getContextPtr());
            str = BinaryenBinary(code, BinaryenMulInt32(), str, builder.makeInt32(4));
            BinaryenExpressionRef stackTopPtr =
                BinaryenBinary(code, BinaryenAddInt32(), builder.getContextPtr(),
                               builder.makeInt32(offsetof(RuntimeContext, stackTops)));
            stackTopPtr = BinaryenBinary(code, BinaryenAddInt32(), stackTopPtr, str);
            BinaryenExpressionRef stackTop =
                BinaryenLoad(code, 4, true, 0, 0, BinaryenTypeInt32(), stackTopPtr);
            stackTop = BinaryenBinary(code, BinaryenSubInt32(), stackTop, builder.makeInt32(8));
            BB(builder, BinaryenStore(code, 4, 0, 0, stackTopPtr, stackTop, BinaryenTypeInt32()))
        }
    }
    uint64_t packed_loc = inst->location.pack();
    BB(builder,
       BinaryenStore(code, 8, offsetof(RuntimeContext, location), 0, builder.getContextPtr(),
                     builder.makeInt64(reinterpret_cast<int64_t &>(packed_loc)),
                     BinaryenTypeInt64()))
    RelooperBlockRef jitReqBody = builder.flush(false, false);
    RelooperAddBranch(body, jitReqBody, pred, NULL);
    exitRequests.push_back(jitReqBody);
}

void WasmEmitter::inputChar(Void *) {
    WasmReg dest = unwrapValue(inst->output);
    BinaryenExpressionRef value = BinaryenCall(code, "inputChar", NULL, 0, BinaryenTypeInt32());
    value = BinaryenUnary(code, BinaryenExtendSInt32(), value);
    builder.storeReg(dest.get(), value);
}

void WasmEmitter::inputNum(Void *) {
    WasmReg dest = unwrapValue(inst->output);
    BinaryenExpressionRef value = BinaryenCall(code, "inputNum", NULL, 0, BinaryenTypeInt32());
    value = BinaryenUnary(code, BinaryenExtendSInt32(), value);
    builder.storeReg(dest.get(), value);
}

void WasmEmitter::outputChar(Value *value) {
    WasmReg valueReg = unwrapValue(value);
    BinaryenExpressionRef callOperands[] = { BinaryenUnary(code, BinaryenWrapInt64(),
                                                           LL(builder, valueReg)) };
    BB(builder, BinaryenCall(code, "printChar", callOperands, 1, BinaryenTypeNone()))
}

void WasmEmitter::outputNum(Value *value) {
    WasmReg valueReg = unwrapValue(value);
    BinaryenExpressionRef callOperands[] = { BinaryenUnary(code, BinaryenWrapInt64(),
                                                           LL(builder, valueReg)) };
    BB(builder, BinaryenCall(code, "printNum", callOperands, 1, BinaryenTypeNone()))
}

void WasmEmitter::emitPrologue() {
    BB(builder, BinaryenGlobalSet(code, G_CONTEXT, BinaryenLocalGet(code, 0, BinaryenTypeInt32())));
    BB(builder,
       BinaryenGlobalSet(code, G_STACK_POINTER, BinaryenLocalGet(code, 1, BinaryenTypeInt32())));
}

void WasmEmitter::emitEpilouge() {
    BB(builder, BinaryenNop(code))
}