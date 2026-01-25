#include "static_asm.hpp"

// Using internal::make_array from static_asm

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

TEST_CASE("PUSH instructions are assembled correctly", "[push]") {
    REQUIRE(push(rcx) == internal::make_array<std::uint8_t>(0x51));
    REQUIRE(push(rbx) == internal::make_array<std::uint8_t>(0x53));

    REQUIRE(push(imm8(0x12)) == internal::make_array<std::uint8_t>(0x6A, 0x12));
    REQUIRE(push(0x1234) == internal::make_array<std::uint8_t>(0x68, 0x34, 0x12, 0x00, 0x00));
    REQUIRE(push(0x12345678) == internal::make_array<std::uint8_t>(0x68, 0x78, 0x56, 0x34, 0x12));
}