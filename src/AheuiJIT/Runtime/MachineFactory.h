#pragma once

#include <AheuiJIT/Runtime/IOProtocol.h>
#include <AheuiJIT/Runtime/Machine.h>

namespace aheuijit {
std::unique_ptr<Machine> createWasmMachine();
std::unique_ptr<Machine> creataeStdioMachine();
std::unique_ptr<Machine> creataeGeneralMachine(std::unique_ptr<IOProtocol> io);
};  // namespace aheuijit
