#include "Runtime.h"

#include <AheuiJIT/Util/Util.h>
#include <fmt/printf.h>

#include <fstream>
#include <iostream>

using namespace aheuijit;

Runtime::Runtime(std::ostream& out, std::istream& in)
    : ctx(std::make_unique<JITContext>()), translator(*this), out(out), in(in) {
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

    if (ctx->storage == 0) {
        if (ctx->storageBuffer[0] != ctx->queueCursor) {
            return *reinterpret_cast<Word*>(ctx->queueCursor);
        }
    } else {
        if (ctx->storageBuffer[ctx->storage] != ctx->stackFronts[ctx->storage]) {
            return *reinterpret_cast<Word*>(ctx->storageBuffer[ctx->storage]);
        }
    }
    return 0;
}

void Runtime::resetState() {
    for (int i = 0; i < storages.size(); ++i) {
        if (storages[i]) {
            delete[] storages[i];
        }
        storages[i] = new uint64_t[MAX_STORAGE_SIZE];
    }
    if (ctx->exhaustPatchTable) {
        delete ctx->exhaustPatchTable;
    }
    ctx->exhaustPatchTable = new JITHashTable;
    ctx->runtime = this;
    ctx->queueBack = reinterpret_cast<uintptr_t>(storages[0]);
    ctx->queueFront = reinterpret_cast<uintptr_t>(storages[0]) + 8 * MAX_STORAGE_SIZE;
    ctx->queueCursor = reinterpret_cast<uintptr_t>(storages[0]) + (8 * MAX_STORAGE_SIZE / 2);
    ctx->storageBuffer[0] = reinterpret_cast<uintptr_t>(storages[0]) + (8 * MAX_STORAGE_SIZE / 2);
    ctx->location = 0;
    for (int i = 1; i < storages.size(); ++i) {
        ctx->storageBuffer[i] = reinterpret_cast<uintptr_t>(storages[i]) + 8 * MAX_STORAGE_SIZE;
        ctx->stackFronts[i] = ctx->storageBuffer[i];
    }
    ctx->storage = 1;
    tlbTable.clear();
    entryTlbTable.clear();
}

void Runtime::printNum(Word word) {
    out << word;
}

Word Runtime::inputNum() {
    Word word;
    std::cin >> word;
    return word;
}

// https://github.com/aheui/caheui/blob/master/aheui.c#L113

inline Word readOneChar(std::istream& in) {
    char ch;
    in.get(ch);
    return (unsigned char)ch;
}

Word Runtime::inputChar() {
    Word a = readOneChar(in);

    if (a < 0x80) {
        return a;
    } else if ((a & 0xf0) == 0xf0) {
        return ((a & 0x07) << 18) + ((readOneChar(in) & 0x3f) << 12) +
               ((readOneChar(in) & 0x3f) << 6) + (readOneChar(in) & 0x3f);
    } else if ((a & 0xe0) == 0xe0) {
        return ((a & 0x0f) << 12) + ((readOneChar(in) & 0x3f) << 6) + (readOneChar(in) & 0x3f);
    } else if ((a & 0xc0) == 0xc0) {
        return ((a & 0x1f) << 6) + (readOneChar(in) & 0x3f);
    } else {
        return -1;
    }
}

void Runtime::printChar(Word word) {
    if (word < 0x80) {
        out << (char)word;
    } else if (word < 0x0800) {
        out << (char)(0xc0 | (word >> 6));
        out << (char)(0x80 | ((word >> 0) & 0x3f));
    } else if (word < 0x10000) {
        out << (char)(0xe0 | (word >> 12));
        out << (char)(0x80 | ((word >> 6) & 0x3f));
        out << (char)(0x80 | ((word >> 0) & 0x3f));
    } else if (word < 0x110000) {
        out << (char)(0xf0 | (word >> 18));
        out << (char)(0x80 | ((word >> 12) & 0x3f));
        out << (char)(0x80 | ((word >> 6) & 0x3f));
        out << (char)(0x80 | ((word >> 0) & 0x3f));
    }
}
