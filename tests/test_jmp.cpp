#include <gtest/gtest.h>
#include "static_asm.hpp"
// Using internal::make_array from static_asm

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

TEST(JmpInstructions, Immediate) {
    EXPECT_EQ(jmp(0x12345678), (internal::make_array<std::uint8_t>(0xE9, 0x78, 0x56, 0x34, 0x12)));
}

TEST(JmpInstructions, Register) {
    EXPECT_EQ(jmp(rcx), (internal::make_array<std::uint8_t>(0xFF, 0xE1)));
    EXPECT_EQ(jmp(rax), (internal::make_array<std::uint8_t>(0xFF, 0xE0)));
}

TEST(JmpInstructions, ExtendedRegister) {
    // jmp r8 requires REX.B prefix (0x41)
    EXPECT_EQ(jmp(r8), (internal::make_array<std::uint8_t>(0x41, 0xFF, 0xE0)));
    EXPECT_EQ(jmp(r15), (internal::make_array<std::uint8_t>(0x41, 0xFF, 0xE7)));
}

TEST(JmpInstructions, Memory) {
    EXPECT_EQ(jmp(ptr(rcx)), (internal::make_array<std::uint8_t>(0xFF, 0x21)));
}
