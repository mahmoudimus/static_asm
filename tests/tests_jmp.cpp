#include "static_asm.hpp"

// Using internal::make_array from static_asm

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

TEST_CASE("JMP instructions are assembled correctly", "[jmp]") {
    REQUIRE(jmp(0x12345678) == internal::make_array<std::uint8_t>(0xE9, 0x78, 0x56, 0x34, 0x12));
    REQUIRE(jmp(rcx) == internal::make_array<std::uint8_t>(0xFF, 0xE1));
    REQUIRE(jmp(ptr(rcx)) == internal::make_array<std::uint8_t>(0xFF, 0x21));
}