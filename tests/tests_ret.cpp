#include "static_asm.hpp"

// Using internal::make_array from static_asm

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

TEST_CASE("RET instructions are assembled correctly", "[ret]") {
    REQUIRE(ret() == internal::make_array<std::uint8_t>(0xC3));
    REQUIRE(ret(2) == internal::make_array<std::uint8_t>(0xC2, 0x02, 0x00));
}

TEST_CASE("RETF instructions are assembled correctly", "[ret]") {
    REQUIRE(retf() == internal::make_array<std::uint8_t>(0xCB));
    REQUIRE(retf(2) == internal::make_array<std::uint8_t>(0xCA, 0x02, 0x00));
}