#include "Disasm.h"

#ifdef X86
#include <capstone/capstone.h>
#include <fmt/printf.h>

using namespace aheuijit;

void aheuijit::disassembleX86(void *buf, size_t size) {
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
#endif

#ifdef AARCH
#include <capstone/capstone.h>
#include <fmt/printf.h>

using namespace aheuijit;

void aheuijit::disassembleA64(void *buf, size_t size) {
    csh handle;
    cs_insn *insn;
    if ((reinterpret_cast<uint64_t>(buf) & 1) != 0) {
        fmt::print("FUCK\n");
    }
    cs_err err = cs_open(CS_ARCH_ARM64, CS_MODE_ARM, &handle);
    size_t count = cs_disasm(handle, (uint8_t *)buf, size - 1, 0, 0, &insn);
    if (count > 0) {
        size_t j;
        int n;
        for (j = 0; j < count; j++) {
            cs_insn *i = &(insn[j]);
            fmt::print("0x{:x}: {} {}\n", reinterpret_cast<uintptr_t>(buf) + i->address,
                       i->mnemonic, i->op_str);
        }
    } else {
        fmt::print("FUCK2\n");
    }
}
#endif
