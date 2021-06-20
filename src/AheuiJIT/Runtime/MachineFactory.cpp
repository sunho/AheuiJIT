#include "MachineFactory.h"

using namespace aheuijit;

#ifdef __EMSCRIPTEN__
#include <AheuiJIT/Runtime/Wasm/WasmMachine.h>
std::unique_ptr<Machine> aheuijit::createWasmMachine() {
    return std::make_unique<WasmMachine>();
}
#endif
#ifdef X86
#include <AheuiJIT/Runtime/X86/X86Machine.h>
std::unique_ptr<Machine> aheuijit::creataeStdioMachine() {
    return std::make_unique<X86Machine>(std::make_unique<StdIOProtocol>());
}
std::unique_ptr<Machine> creataeGeneralMachine(std::unique_ptr<IOProtocol> io) {
    return std::make_unique<X86Machine>(std::move(io));
}
#endif
#ifdef AARCH
#include <AheuiJIT/Runtime/A64/A64Machine.h>
std::unique_ptr<Machine> aheuijit::creataeStdioMachine() {
    return std::make_unique<A64Machine>(std::make_unique<StdIOProtocol>());
}
std::unique_ptr<Machine> creataeGeneralMachine(std::unique_ptr<IOProtocol> io) {
    return std::make_unique<A64Machine>(std::move(io));
}
#endif
