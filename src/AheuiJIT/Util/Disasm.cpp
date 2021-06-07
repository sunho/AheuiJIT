#include "Disasm.h"

#include <capstone/capstone.h>
#include <fmt/printf.h>

void disassembleX86(void *buf, size_t size) {
    csh handle;
    cs_insn *insn;
    cs_err err = cs_open(CS_ARCH_X86, CS_MODE_64, &handle);
    size_t count = cs_disasm(handle, (uint8_t *)buf, size - 1, 0, 0, &insn);
    if (count > 0) {
        size_t j;
        int n;
        for (j = 0; j < count; j++) {
            cs_insn *i = &(insn[j]);
            fmt::print("0x{:x}: {} {}\n", reinterpret_cast<uintptr_t>(buf) + i->address,
                       i->mnemonic, i->op_str);
        }
    }
}
