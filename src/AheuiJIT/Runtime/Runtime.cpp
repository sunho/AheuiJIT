#include "Runtime.h"

#include <AheuiJIT/Util/Util.h>
#include <fmt/printf.h>

#include <fstream>
#include <iostream>

using namespace aheuijit;

Runtime::Runtime(std::unique_ptr<IOProtocol> io)
    : io(std::move(io)), ctx(std::make_unique<JITContext>()), translator(*this) {
    storages.fill(0);
}

Word Runtime::run(const std::u16string& code) {
    resetState();

    TranslatedFunc func = translator.translate(code, tlbTable);
    do {
        ctx->location = 0;
        func(ctx.get());
        if (ctx->location) {
            // JIT translation requested
            const Location location = Location::unpack(ctx->location);
            LOG("Retranslate at location: {}", location.description());
            ASSERT(location.pack() == ctx->location)
            const Location failLocation = translator.calculateFailLocation(location);
            const Location validFailLocation = translator.stepToValidLocation(failLocation);
            const auto it = entryTlbTable.find(validFailLocation);
            if (it != entryTlbTable.end()) {
                func = reinterpret_cast<TranslatedFunc>(it->second);
            } else {
                func = translator.translate(validFailLocation, tlbTable);
                entryTlbTable.emplace(validFailLocation, func);
            }
            ASSERT(tlbTable.find(validFailLocation) != tlbTable.end())
            ctx->exhaustPatchTable->setValue(location.hash(), tlbTable[validFailLocation]);
        }
    } while (ctx->location);

    if (ctx->storage == QUEUE_STORAGE_IMM) {
        if (ctx->queueFront != ctx->queueBack) {
            return reinterpret_cast<Word*>(
                ctx->queueBuffer)[(ctx->queueBack + 1) & (MAX_STORAGE_SIZE - 1)];
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
        storages[i] = new (std::align_val_t(64)) uint64_t[MAX_STORAGE_SIZE];
    }
    if (ctx->exhaustPatchTable) {
        delete ctx->exhaustPatchTable;
    }
    ctx->exhaustPatchTable = new JITHashTable;
    ctx->runtime = this;
    ctx->queueBuffer = reinterpret_cast<uintptr_t>(storages[0]);
    ctx->queueFront = 0;
    ctx->queueBack = MAX_STORAGE_SIZE - 1;
    ctx->location = 0;
    for (int i = 0; i < ctx->stackTops.size(); ++i) {
        ctx->stackTops[i] = reinterpret_cast<uintptr_t>(storages[i + 1]) + 8 * MAX_STORAGE_SIZE;
        ctx->stackUppers[i] = ctx->stackTops[i];
    }
    ctx->storage = 0;
    tlbTable.clear();
    entryTlbTable.clear();
}

void Runtime::printNum(Word word) {
    io->printNum(word);
}

Word Runtime::inputNum() {
    return io->inputNum();
}

// https://github.com/aheui/caheui/blob/master/aheui.c#L113

Word Runtime::inputChar() {
    Word a = io->inputChar();
    if (a == -1) {
        return -1;
    }

    if (a < 0x80) {
        return a;
    } else if ((a & 0xf0) == 0xf0) {
        return ((a & 0x07) << 18) + ((io->inputChar() & 0x3f) << 12) +
               ((io->inputChar() & 0x3f) << 6) + (io->inputChar() & 0x3f);
    } else if ((a & 0xe0) == 0xe0) {
        return ((a & 0x0f) << 12) + ((io->inputChar() & 0x3f) << 6) + (io->inputChar() & 0x3f);
    } else if ((a & 0xc0) == 0xc0) {
        return ((a & 0x1f) << 6) + (io->inputChar() & 0x3f);
    } else {
        return -1;
    }
}

void Runtime::printChar(Word word) {
    if (word < 0x80) {
        io->printChar(word);
    } else if (word < 0x0800) {
        io->printChar(0xc0 | (word >> 6));
        io->printChar(0x80 | ((word >> 0) & 0x3f));
    } else if (word < 0x10000) {
        io->printChar(0xe0 | (word >> 12));
        io->printChar(0x80 | ((word >> 6) & 0x3f));
        io->printChar(0x80 | ((word >> 0) & 0x3f));
    } else if (word < 0x110000) {
        io->printChar(0xf0 | (word >> 18));
        io->printChar(0x80 | ((word >> 12) & 0x3f));
        io->printChar(0x80 | ((word >> 6) & 0x3f));
        io->printChar(0x80 | ((word >> 0) & 0x3f));
    }
}
