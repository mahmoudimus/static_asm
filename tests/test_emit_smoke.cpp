#include "static_asm.hpp"

int main() {
    constexpr auto code = static_asm::x86::instructions::nop();

#ifndef STATIC_ASM_TEST_NO_EMIT
    static_asm::core::emit(code);
#endif

    return code.size() == 1 && code[0] == 0x90 ? 0 : 1;
}
