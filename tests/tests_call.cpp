#include "static_asm.hpp"

// Using internal::make_array from static_asm

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

TEST_CASE("CALL instructions are assembled correctly", "[call]") {
    REQUIRE(call(0x12345678) == internal::make_array<std::uint8_t>(0xE8, 0x78, 0x56, 0x34, 0x12));
    REQUIRE(call(rcx) == internal::make_array<std::uint8_t>(0xFF, 0xD1));
    REQUIRE(call(ptr(rcx)) == internal::make_array<std::uint8_t>(0xFF, 0x11));
}