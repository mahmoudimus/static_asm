#include "static_asm.hpp"
#include <gtest/gtest.h>

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// TEST reg, reg
TEST(TestInstructions, RegisterToRegister64) {
    // test rax, rbx: 48 85 D8
    EXPECT_EQ(test(rax, rbx), (internal::make_array<std::uint8_t>(0x48, 0x85, 0xD8)));
    // test rcx, rdx: 48 85 D1
    EXPECT_EQ(test(rcx, rdx), (internal::make_array<std::uint8_t>(0x48, 0x85, 0xD1)));
    // test r8, r9: 4D 85 C8
    EXPECT_EQ(test(r8, r9), (internal::make_array<std::uint8_t>(0x4D, 0x85, 0xC8)));
}

TEST(TestInstructions, RegisterToRegister32) {
    // test eax, ebx: 85 D8
    EXPECT_EQ(test(eax, ebx), (internal::make_array<std::uint8_t>(0x85, 0xD8)));
    // test ecx, edx: 85 D1
    EXPECT_EQ(test(ecx, edx), (internal::make_array<std::uint8_t>(0x85, 0xD1)));
}

TEST(TestInstructions, RegisterToRegister8) {
    // test al, bl: 84 D8
    EXPECT_EQ(test(al, bl), (internal::make_array<std::uint8_t>(0x84, 0xD8)));
    // test cl, dl: 84 D1
    EXPECT_EQ(test(cl, dl), (internal::make_array<std::uint8_t>(0x84, 0xD1)));
}

// TEST reg, imm
TEST(TestInstructions, RegisterImmediate32) {
    // test eax, 0x12345678: F7 C0 78 56 34 12
    EXPECT_EQ(test(eax, 0x12345678), (internal::make_array<std::uint8_t>(0xF7, 0xC0, 0x78, 0x56, 0x34, 0x12)));
    // test ecx, 1: F7 C1 01 00 00 00
    EXPECT_EQ(test(ecx, 1), (internal::make_array<std::uint8_t>(0xF7, 0xC1, 0x01, 0x00, 0x00, 0x00)));
}

TEST(TestInstructions, RegisterImmediate64) {
    // test rax, 0x12345678: 48 F7 C0 78 56 34 12
    EXPECT_EQ(test(rax, 0x12345678), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0xC0, 0x78, 0x56, 0x34, 0x12)));
}
