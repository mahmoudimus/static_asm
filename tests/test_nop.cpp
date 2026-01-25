#include <gtest/gtest.h>
#include "static_asm.hpp"
// Using internal::make_array from static_asm

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

TEST(NopInstructions, SingleByte) {
    EXPECT_EQ(nop(), (internal::make_array<std::uint8_t>(0x90)));
    EXPECT_EQ(NOP(1), (internal::make_array<std::uint8_t>(0x90)));
}

TEST(NopInstructions, MultiByte) {
    EXPECT_EQ(NOP(2), (internal::make_array<std::uint8_t>(0x66, 0x90)));
    EXPECT_EQ(NOP(3), (internal::make_array<std::uint8_t>(0x0F, 0x1F, 0x00)));
    EXPECT_EQ(NOP(4), (internal::make_array<std::uint8_t>(0x0F, 0x1F, 0x40, 0x00)));
    EXPECT_EQ(NOP(5), (internal::make_array<std::uint8_t>(0x0F, 0x1F, 0x44, 0x00, 0x00)));
    EXPECT_EQ(NOP(6), (internal::make_array<std::uint8_t>(0x66, 0x0F, 0x1F, 0x44, 0x00, 0x00)));
    EXPECT_EQ(NOP(7), (internal::make_array<std::uint8_t>(0x0F, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00)));
    EXPECT_EQ(NOP(8), (internal::make_array<std::uint8_t>(0x0F, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00)));
    EXPECT_EQ(NOP(9), (internal::make_array<std::uint8_t>(0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00)));
}
