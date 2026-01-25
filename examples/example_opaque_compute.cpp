#include "static_asm.hpp"

#include <iostream>

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

int main() {

    std::cout << "this program still returns 12" << std::endl;

    constexpr auto code = core::assemble(
        xor_(eax, eax),
        mov(eax, 0x0C),
        add(eax, 0xff),
        adc(eax, 0x01),
        ret()
    );

    core::emit(code);
}