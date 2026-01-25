#include <gtest/gtest.h>
#include "static_asm.hpp"

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// INC tests
TEST(IncInstructions, Register64) {
    // inc rax: 48 FF C0
    EXPECT_EQ(inc(rax), (internal::make_array<std::uint8_t>(0x48, 0xFF, 0xC0)));
    // inc rcx: 48 FF C1
    EXPECT_EQ(inc(rcx), (internal::make_array<std::uint8_t>(0x48, 0xFF, 0xC1)));
    // inc r8: 49 FF C0
    EXPECT_EQ(inc(r8), (internal::make_array<std::uint8_t>(0x49, 0xFF, 0xC0)));
    // inc r15: 49 FF C7
    EXPECT_EQ(inc(r15), (internal::make_array<std::uint8_t>(0x49, 0xFF, 0xC7)));
}

TEST(IncInstructions, Register32) {
    // inc eax: FF C0
    EXPECT_EQ(inc(eax), (internal::make_array<std::uint8_t>(0xFF, 0xC0)));
    // inc ecx: FF C1
    EXPECT_EQ(inc(ecx), (internal::make_array<std::uint8_t>(0xFF, 0xC1)));
    // inc r8d: 41 FF C0
    EXPECT_EQ(inc(r8d), (internal::make_array<std::uint8_t>(0x41, 0xFF, 0xC0)));
}

TEST(IncInstructions, Register8) {
    // inc al: FE C0
    EXPECT_EQ(inc(al), (internal::make_array<std::uint8_t>(0xFE, 0xC0)));
    // inc cl: FE C1
    EXPECT_EQ(inc(cl), (internal::make_array<std::uint8_t>(0xFE, 0xC1)));
}

// DEC tests
TEST(DecInstructions, Register64) {
    // dec rax: 48 FF C8
    EXPECT_EQ(dec(rax), (internal::make_array<std::uint8_t>(0x48, 0xFF, 0xC8)));
    // dec rcx: 48 FF C9
    EXPECT_EQ(dec(rcx), (internal::make_array<std::uint8_t>(0x48, 0xFF, 0xC9)));
    // dec r8: 49 FF C8
    EXPECT_EQ(dec(r8), (internal::make_array<std::uint8_t>(0x49, 0xFF, 0xC8)));
}

TEST(DecInstructions, Register32) {
    // dec eax: FF C8
    EXPECT_EQ(dec(eax), (internal::make_array<std::uint8_t>(0xFF, 0xC8)));
    // dec r8d: 41 FF C8
    EXPECT_EQ(dec(r8d), (internal::make_array<std::uint8_t>(0x41, 0xFF, 0xC8)));
}

TEST(DecInstructions, Register8) {
    // dec al: FE C8
    EXPECT_EQ(dec(al), (internal::make_array<std::uint8_t>(0xFE, 0xC8)));
}

// NEG tests
TEST(NegInstructions, Register64) {
    // neg rax: 48 F7 D8
    EXPECT_EQ(neg(rax), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0xD8)));
    // neg rcx: 48 F7 D9
    EXPECT_EQ(neg(rcx), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0xD9)));
    // neg r8: 49 F7 D8
    EXPECT_EQ(neg(r8), (internal::make_array<std::uint8_t>(0x49, 0xF7, 0xD8)));
}

TEST(NegInstructions, Register32) {
    // neg eax: F7 D8
    EXPECT_EQ(neg(eax), (internal::make_array<std::uint8_t>(0xF7, 0xD8)));
    // neg r8d: 41 F7 D8
    EXPECT_EQ(neg(r8d), (internal::make_array<std::uint8_t>(0x41, 0xF7, 0xD8)));
}

TEST(NegInstructions, Register8) {
    // neg al: F6 D8
    EXPECT_EQ(neg(al), (internal::make_array<std::uint8_t>(0xF6, 0xD8)));
}

// NOT tests
TEST(NotInstructions, Register64) {
    // not rax: 48 F7 D0
    EXPECT_EQ(not_(rax), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0xD0)));
    // not rcx: 48 F7 D1
    EXPECT_EQ(not_(rcx), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0xD1)));
    // not r8: 49 F7 D0
    EXPECT_EQ(not_(r8), (internal::make_array<std::uint8_t>(0x49, 0xF7, 0xD0)));
}

TEST(NotInstructions, Register32) {
    // not eax: F7 D0
    EXPECT_EQ(not_(eax), (internal::make_array<std::uint8_t>(0xF7, 0xD0)));
    // not r8d: 41 F7 D0
    EXPECT_EQ(not_(r8d), (internal::make_array<std::uint8_t>(0x41, 0xF7, 0xD0)));
}

TEST(NotInstructions, Register8) {
    // not al: F6 D0
    EXPECT_EQ(not_(al), (internal::make_array<std::uint8_t>(0xF6, 0xD0)));
}
