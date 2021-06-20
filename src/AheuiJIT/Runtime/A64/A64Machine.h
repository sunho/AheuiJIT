#pragma once

#include <AheuiJIT/Runtime/IOProtocol.h>
#include <AheuiJIT/Runtime/Machine.h>
#include <AheuiJIT/Runtime/NativeMachine.h>
#include <AheuiJIT/Util/Disasm.h>
#include <asmjit/a64.h>

namespace aheuijit {

using A64CodeBlock = NativeCodeBlock<asmjit::a64::Assembler>;
using A64Machine = NativeMachine<asmjit::a64::Assembler, disassembleA64>;

}  // namespace aheuijit
