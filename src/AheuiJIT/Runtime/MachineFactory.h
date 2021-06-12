#pragma once

#include <AheuiJIT/Runtime/Machine.h>

namespace aheuijit {

#ifdef __EMSCRIPTEN__
std::unique_ptr<Machine> createWasmMachine();
#else
std::unique_ptr<Machine> creataeStdioMachine();
#endif

};  // namespace aheuijit
