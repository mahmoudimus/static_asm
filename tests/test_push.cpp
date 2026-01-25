#include <gtest/gtest.h>
#include "static_asm.hpp"
// Using internal::make_array from static_asm

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

TEST(PushInstructions, Register) {
    EXPECT_EQ(push(rcx), (internal::make_array<std::uint8_t>(0x51)));
    EXPECT_EQ(push(rbx), (internal::make_array<std::uint8_t>(0x53)));
}

TEST(PushInstructions, Immediate) {
    EXPECT_EQ(push(imm8(0x12)), (internal::make_array<std::uint8_t>(0x6A, 0x12)));
    EXPECT_EQ(push(0x1234), (internal::make_array<std::uint8_t>(0x68, 0x34, 0x12, 0x00, 0x00)));
    EXPECT_EQ(push(0x12345678), (internal::make_array<std::uint8_t>(0x68, 0x78, 0x56, 0x34, 0x12)));
}
