#include "static_asm.hpp"
#include <gtest/gtest.h>

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// ============================================================================
// IMUL two-operand form tests: IMUL r, r/m (0F AF /r)
// Result stored in first operand: r = r * r/m
// ============================================================================

TEST(ImulTwoOperand, Register64_Register64) {
    // imul rax, rbx: 48 0F AF C3
    EXPECT_EQ(imul(rax, rbx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xAF, 0xC3)));
    // imul rcx, rdx: 48 0F AF CA
    EXPECT_EQ(imul(rcx, rdx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xAF, 0xCA)));
    // imul r8, r9: 4D 0F AF C1
    EXPECT_EQ(imul(r8, r9), (internal::make_array<std::uint8_t>(0x4D, 0x0F, 0xAF, 0xC1)));
    // imul rax, r8: 49 0F AF C0
    EXPECT_EQ(imul(rax, r8), (internal::make_array<std::uint8_t>(0x49, 0x0F, 0xAF, 0xC0)));
    // imul r8, rax: 4C 0F AF C0
    EXPECT_EQ(imul(r8, rax), (internal::make_array<std::uint8_t>(0x4C, 0x0F, 0xAF, 0xC0)));
}

TEST(ImulTwoOperand, Register32_Register32) {
    // imul eax, ebx: 0F AF C3
    EXPECT_EQ(imul(eax, ebx), (internal::make_array<std::uint8_t>(0x0F, 0xAF, 0xC3)));
    // imul ecx, edx: 0F AF CA
    EXPECT_EQ(imul(ecx, edx), (internal::make_array<std::uint8_t>(0x0F, 0xAF, 0xCA)));
    // imul r8d, r9d: 45 0F AF C1
    EXPECT_EQ(imul(r8d, r9d), (internal::make_array<std::uint8_t>(0x45, 0x0F, 0xAF, 0xC1)));
    // imul eax, r8d: 41 0F AF C0
    EXPECT_EQ(imul(eax, r8d), (internal::make_array<std::uint8_t>(0x41, 0x0F, 0xAF, 0xC0)));
    // imul r8d, eax: 44 0F AF C0
    EXPECT_EQ(imul(r8d, eax), (internal::make_array<std::uint8_t>(0x44, 0x0F, 0xAF, 0xC0)));
}

// ============================================================================
// IMUL three-operand form tests: IMUL r, r/m, imm
// r = r/m * imm
// 6B /r ib - for imm8 (sign-extended)
// 69 /r id - for imm32
// ============================================================================

TEST(ImulThreeOperand, Register64_Register64_Imm8) {
    // imul rax, rbx, 5: 48 6B C3 05
    EXPECT_EQ(imul(rax, rbx, imm8(5)), (internal::make_array<std::uint8_t>(0x48, 0x6B, 0xC3, 0x05)));
    // imul rcx, rdx, 10: 48 6B CA 0A
    EXPECT_EQ(imul(rcx, rdx, imm8(10)), (internal::make_array<std::uint8_t>(0x48, 0x6B, 0xCA, 0x0A)));
    // imul r8, r9, 0x7F: 4D 6B C1 7F
    EXPECT_EQ(imul(r8, r9, imm8(0x7F)), (internal::make_array<std::uint8_t>(0x4D, 0x6B, 0xC1, 0x7F)));
}

TEST(ImulThreeOperand, Register32_Register32_Imm8) {
    // imul eax, ebx, 5: 6B C3 05
    EXPECT_EQ(imul(eax, ebx, imm8(5)), (internal::make_array<std::uint8_t>(0x6B, 0xC3, 0x05)));
    // imul ecx, edx, 10: 6B CA 0A
    EXPECT_EQ(imul(ecx, edx, imm8(10)), (internal::make_array<std::uint8_t>(0x6B, 0xCA, 0x0A)));
    // imul r8d, r9d, 0x7F: 45 6B C1 7F
    EXPECT_EQ(imul(r8d, r9d, imm8(0x7F)), (internal::make_array<std::uint8_t>(0x45, 0x6B, 0xC1, 0x7F)));
}

TEST(ImulThreeOperand, Register64_Register64_Imm32) {
    // imul rax, rbx, 0x12345678: 48 69 C3 78 56 34 12
    EXPECT_EQ(imul(rax, rbx, imm32(0x12345678)), (internal::make_array<std::uint8_t>(0x48, 0x69, 0xC3, 0x78, 0x56, 0x34, 0x12)));
    // imul rcx, rdx, 0x100: 48 69 CA 00 01 00 00
    EXPECT_EQ(imul(rcx, rdx, imm32(0x100)), (internal::make_array<std::uint8_t>(0x48, 0x69, 0xCA, 0x00, 0x01, 0x00, 0x00)));
}

TEST(ImulThreeOperand, Register32_Register32_Imm32) {
    // imul eax, ebx, 0x12345678: 69 C3 78 56 34 12
    EXPECT_EQ(imul(eax, ebx, imm32(0x12345678)), (internal::make_array<std::uint8_t>(0x69, 0xC3, 0x78, 0x56, 0x34, 0x12)));
    // imul ecx, edx, 0x100: 69 CA 00 01 00 00
    EXPECT_EQ(imul(ecx, edx, imm32(0x100)), (internal::make_array<std::uint8_t>(0x69, 0xCA, 0x00, 0x01, 0x00, 0x00)));
}

TEST(ImulThreeOperand, ExtendedRegisters_Imm32) {
    // imul r8, r9, 0x1000: 4D 69 C1 00 10 00 00
    EXPECT_EQ(imul(r8, r9, imm32(0x1000)), (internal::make_array<std::uint8_t>(0x4D, 0x69, 0xC1, 0x00, 0x10, 0x00, 0x00)));
    // imul r8d, r9d, 0x1000: 45 69 C1 00 10 00 00
    EXPECT_EQ(imul(r8d, r9d, imm32(0x1000)), (internal::make_array<std::uint8_t>(0x45, 0x69, 0xC1, 0x00, 0x10, 0x00, 0x00)));
}

// Test with integer literals (uses imm32 internally)
TEST(ImulThreeOperand, IntegerLiterals) {
    // imul rax, rbx, 1000 (uses 32-bit immediate): 48 69 C3 E8 03 00 00
    EXPECT_EQ(imul(rax, rbx, 1000), (internal::make_array<std::uint8_t>(0x48, 0x69, 0xC3, 0xE8, 0x03, 0x00, 0x00)));
    // imul eax, ebx, 1000: 69 C3 E8 03 00 00
    EXPECT_EQ(imul(eax, ebx, 1000), (internal::make_array<std::uint8_t>(0x69, 0xC3, 0xE8, 0x03, 0x00, 0x00)));
}

// Same register for destination and source (common pattern: r = r * imm)
TEST(ImulThreeOperand, SameRegister) {
    // imul rax, rax, 2: 48 6B C0 02
    EXPECT_EQ(imul(rax, rax, imm8(2)), (internal::make_array<std::uint8_t>(0x48, 0x6B, 0xC0, 0x02)));
    // imul eax, eax, 2: 6B C0 02
    EXPECT_EQ(imul(eax, eax, imm8(2)), (internal::make_array<std::uint8_t>(0x6B, 0xC0, 0x02)));
}
