#include "MachineFactory.h"

using namespace aheuijit;

#ifdef __EMSCRIPTEN__

#include <AheuiJIT/Runtime/Wasm/WasmMachine.h>
std::unique_ptr<Machine> aheuijit::createWasmMachine() {
    return std::make_unique<WasmMachine>();
}

#else

#include <AheuiJIT/Runtime/X86/X86Machine.h>
std::unique_ptr<Machine> aheuijit::creataeStdioMachine() {
    return std::make_unique<X86Machine>(std::make_unique<StdIOProtocol>());
}

#endif
