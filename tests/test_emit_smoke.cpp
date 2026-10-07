#include "static_asm.hpp"

int main() {
    constexpr auto code = static_asm::x86::instructions::nop();
    constexpr auto label = static_asm::x86::label<"emit">;
    constexpr auto symbolic = static_asm::core::assemble(label.assemble(code));

#ifndef STATIC_ASM_TEST_NO_EMIT
    static_asm::core::emit(code);
    static_asm::core::emit(symbolic);
#endif

    return code.size() == 1 && code[0] == 0x90 && symbolic.size() == 1 && symbolic[0] == 0x90 ? 0 : 1;
}
