#pragma once

#include <cstdlib>

namespace aheuijit {

void disassembleX86(void *buf, size_t size);
void disassembleA64(void *buf, size_t size);

}  // namespace aheuijit
