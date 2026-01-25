#include "static_asm.hpp"
#include <gtest/gtest.h>
// Using internal::make_array from static_asm

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

TEST(CallInstructions, Immediate) {
    EXPECT_EQ(call(0x12345678), (internal::make_array<std::uint8_t>(0xE8, 0x78, 0x56, 0x34, 0x12)));
}

TEST(CallInstructions, Register) {
    EXPECT_EQ(call(rcx), (internal::make_array<std::uint8_t>(0xFF, 0xD1)));
    EXPECT_EQ(call(rax), (internal::make_array<std::uint8_t>(0xFF, 0xD0)));
}

TEST(CallInstructions, ExtendedRegister) {
    // call r8 requires REX.B prefix (0x41)
    EXPECT_EQ(call(r8), (internal::make_array<std::uint8_t>(0x41, 0xFF, 0xD0)));
    EXPECT_EQ(call(r15), (internal::make_array<std::uint8_t>(0x41, 0xFF, 0xD7)));
}

TEST(CallInstructions, Memory) {
    EXPECT_EQ(call(ptr(rcx)), (internal::make_array<std::uint8_t>(0xFF, 0x11)));
}
