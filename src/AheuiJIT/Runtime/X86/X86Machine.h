#pragma once

#include <AheuiJIT/Runtime/IOProtocol.h>
#include <AheuiJIT/Runtime/Machine.h>
#include <AheuiJIT/Runtime/NativeMachine.h>
#include <AheuiJIT/Util/Disasm.h>
#include <asmjit/x86.h>

namespace aheuijit {

using X86CodeBlock = NativeCodeBlock<asmjit::x86::Assembler>;
using X86Machine = NativeMachine<asmjit::x86::Assembler, disassembleX86>;

}  // namespace aheuijit
