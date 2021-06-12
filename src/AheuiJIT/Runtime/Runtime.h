#pragma once
#include <AheuiJIT/Runtime/IRBuffer.h>
#include <AheuiJIT/Runtime/Interpreter.h>
#include <AheuiJIT/Runtime/MachineFactory.h>
#include <AheuiJIT/Runtime/RuntimeContext.h>
#include <AheuiJIT/Translator/Translator.h>

namespace aheuijit {

struct Runtime {
    Runtime(std::unique_ptr<Machine> machine);
    ~Runtime() = default;

    void setConfig(RuntimeConfig conf);
    RuntimeConfig getConfig();
    Word run(const std::u16string& code);

  private:
    void resetState();
    void fixupStorage();

    std::unique_ptr<Machine> machine;
    Translator translator;
    RuntimeConfig conf;
    IRBuffer irBuffer;
    Interpreter interpreter;
    std::unique_ptr<RuntimeContext> ctx;
    std::map<Location, TranslatedFunc> entryTlbTable;
    std::array<uint64_t*, 27> storages;
    size_t remainInterpretCycle{ 0 };
};

}  // namespace aheuijit
