#pragma once
#include <AheuiJIT/Runtime/JITContext.h>
#include <AheuiJIT/Translator/Translator.h>

namespace aheuijit {

constexpr int MAX_STORAGE_SIZE = 2048;

struct Runtime {
    Runtime();
    ~Runtime() = default;

    Word run(const std::u16string& code);
    void printChar(Word word);
    void printNum(Word word);
    Word inputChar();
    Word inputNum();

  private:
    void resetState();
    void fixupStorage();

    Translator translator;
    std::unique_ptr<JITContext> ctx;
    std::map<Location, TranslatedFunc> entryTlbTable;
    TLBTable tlbTable;
    std::array<uint64_t*, 27> storages;
};

}  // namespace aheuijit
