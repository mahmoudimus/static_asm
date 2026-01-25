#include "static_asm.hpp"

#include <iostream>

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

int main() {
    std::cout << "this program returns 12" << std::endl;

    constexpr auto code = core::assemble(
        mov(rax, 12),
        ret());

    core::emit(code);
}