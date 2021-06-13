#include "Runtime.h"

#include <AheuiJIT/Util/Util.h>
#include <fmt/printf.h>

#include <fstream>
#include <iostream>

using namespace aheuijit;

Runtime::Runtime(std::unique_ptr<Machine> machine)
    : machine(std::move(machine))
    , ctx(std::move(std::make_unique<RuntimeContext>()))
    , translator(*this->machine) {
    storages.fill(0);
    setConfig(conf);
}

void Runtime::setConfig(RuntimeConfig conf) {
    this->conf = conf;
    translator.setConfig(conf);
    interpreter.setConfig(conf);
    machine->setConfig(conf);
}

RuntimeConfig Runtime::getConfig() {
    return conf;
}

Word Runtime::run(const std::u16string& code) {
    resetState();
    irBuffer.reset();
    machine->reset();
    Location pc = DEFAULT_LOCATION;
    remainInterpretCycle = conf.numInterpretCycle;
    translator.setCode(code);
    do {
        ctx->location = 0;
        if (!irBuffer.findBlock(pc)) {
            translator.translate(pc, irBuffer);
        }
        if (remainInterpretCycle) {
            pc = interpreter.run(pc, ctx.get(), irBuffer, remainInterpretCycle);
            if (pc == Location()) {
                break;
            }
            if (!remainInterpretCycle && !ctx->location) {
                ctx->location = 1;  // hack
                continue;
            }
        } else {
            BasicBlock* bb = irBuffer.findBlock(pc);
            if (!machine->hasCodeBlock(bb)) {
                translator.emit(pc, irBuffer);
#ifndef __EMSCRIPTEN__
                ctx->exhaustPatchTable->setValue(pc.hash(), machine->tlbTable[pc]);
#endif
            }
            machine->runCodeBlock(bb, ctx.get());
        }
        if (ctx->location) {
            const Location location = Location::unpack(ctx->location);
            const Location failLocation = translator.calculateFailLocation(location);
            const Location validFailLocation = translator.stepToValidLocation(failLocation);
            if (conf.interpretAfterFail) {
                remainInterpretCycle = conf.numInterpretCycle;
            }
            pc = validFailLocation;
        }
    } while (ctx->location);

    if (ctx->storage == QUEUE_STORAGE_IMM) {
        if (ctx->queueFront != ctx->queueBack) {
            return reinterpret_cast<Word*>(
                ctx->queueBuffer)[(ctx->queueBack + 1) & (conf.maxStorageSize - 1)];
        }
    } else {
        if (ctx->stackTops[ctx->storage] != ctx->stackUppers[ctx->storage]) {
            return *reinterpret_cast<Word*>(ctx->stackTops[ctx->storage]);
        }
    }
    return 0;
}

void Runtime::resetState() {
    for (int i = 0; i < storages.size(); ++i) {
        if (storages[i]) {
            delete[] storages[i];
        }
        storages[i] = new uint64_t[conf.maxStorageSize];
    }
    if (ctx->exhaustPatchTable) {
        delete ctx->exhaustPatchTable;
    }
#ifndef __EMSCRIPTEN__
    ctx->exhaustPatchTable = new JITHashTable;
#endif
    ctx->machine = machine.get();
    ctx->runtime = this;
    ctx->queueBuffer = reinterpret_cast<uintptr_t>(storages[0]);
    ctx->queueFront = 0;
    ctx->queueBack = conf.maxStorageSize - 1;
    ctx->location = 0;
    for (int i = 0; i < ctx->stackTops.size(); ++i) {
        ctx->stackTops[i] = reinterpret_cast<uintptr_t>(storages[i + 1]) + 8 * conf.maxStorageSize;
        ctx->stackUppers[i] = ctx->stackTops[i];
    }
    ctx->storage = 0;
    entryTlbTable.clear();
}
