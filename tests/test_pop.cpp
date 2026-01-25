#include <gtest/gtest.h>
#include "static_asm.hpp"
// Using internal::make_array from static_asm

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

TEST(PopInstructions, Register) {
    EXPECT_EQ(pop(rcx), (internal::make_array<std::uint8_t>(0x59)));
}
