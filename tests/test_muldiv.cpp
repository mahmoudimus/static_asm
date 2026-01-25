#include <gtest/gtest.h>
#include "static_asm.hpp"

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// MUL tests - unsigned multiply (rdx:rax = rax * operand)
TEST(MulInstructions, Register64) {
    // mul rax: 48 F7 E0
    EXPECT_EQ(mul(rax), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0xE0)));
    // mul rcx: 48 F7 E1
    EXPECT_EQ(mul(rcx), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0xE1)));
    // mul r8: 49 F7 E0
    EXPECT_EQ(mul(r8), (internal::make_array<std::uint8_t>(0x49, 0xF7, 0xE0)));
    // mul r15: 49 F7 E7
    EXPECT_EQ(mul(r15), (internal::make_array<std::uint8_t>(0x49, 0xF7, 0xE7)));
}

TEST(MulInstructions, Register32) {
    // mul eax: F7 E0
    EXPECT_EQ(mul(eax), (internal::make_array<std::uint8_t>(0xF7, 0xE0)));
    // mul ecx: F7 E1
    EXPECT_EQ(mul(ecx), (internal::make_array<std::uint8_t>(0xF7, 0xE1)));
    // mul r8d: 41 F7 E0
    EXPECT_EQ(mul(r8d), (internal::make_array<std::uint8_t>(0x41, 0xF7, 0xE0)));
}

TEST(MulInstructions, Register8) {
    // mul al: F6 E0
    EXPECT_EQ(mul(al), (internal::make_array<std::uint8_t>(0xF6, 0xE0)));
    // mul cl: F6 E1
    EXPECT_EQ(mul(cl), (internal::make_array<std::uint8_t>(0xF6, 0xE1)));
}

// IMUL tests (one-operand form) - signed multiply
TEST(ImulInstructions, Register64) {
    // imul rax: 48 F7 E8
    EXPECT_EQ(imul(rax), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0xE8)));
    // imul rcx: 48 F7 E9
    EXPECT_EQ(imul(rcx), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0xE9)));
    // imul r8: 49 F7 E8
    EXPECT_EQ(imul(r8), (internal::make_array<std::uint8_t>(0x49, 0xF7, 0xE8)));
}

TEST(ImulInstructions, Register32) {
    // imul eax: F7 E8
    EXPECT_EQ(imul(eax), (internal::make_array<std::uint8_t>(0xF7, 0xE8)));
    // imul r8d: 41 F7 E8
    EXPECT_EQ(imul(r8d), (internal::make_array<std::uint8_t>(0x41, 0xF7, 0xE8)));
}

TEST(ImulInstructions, Register8) {
    // imul al: F6 E8
    EXPECT_EQ(imul(al), (internal::make_array<std::uint8_t>(0xF6, 0xE8)));
}

// DIV tests - unsigned divide
TEST(DivInstructions, Register64) {
    // div rax: 48 F7 F0
    EXPECT_EQ(div(rax), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0xF0)));
    // div rcx: 48 F7 F1
    EXPECT_EQ(div(rcx), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0xF1)));
    // div r8: 49 F7 F0
    EXPECT_EQ(div(r8), (internal::make_array<std::uint8_t>(0x49, 0xF7, 0xF0)));
}

TEST(DivInstructions, Register32) {
    // div eax: F7 F0
    EXPECT_EQ(div(eax), (internal::make_array<std::uint8_t>(0xF7, 0xF0)));
    // div r8d: 41 F7 F0
    EXPECT_EQ(div(r8d), (internal::make_array<std::uint8_t>(0x41, 0xF7, 0xF0)));
}

TEST(DivInstructions, Register8) {
    // div al: F6 F0
    EXPECT_EQ(div(al), (internal::make_array<std::uint8_t>(0xF6, 0xF0)));
}

// IDIV tests - signed divide
TEST(IdivInstructions, Register64) {
    // idiv rax: 48 F7 F8
    EXPECT_EQ(idiv(rax), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0xF8)));
    // idiv rcx: 48 F7 F9
    EXPECT_EQ(idiv(rcx), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0xF9)));
    // idiv r8: 49 F7 F8
    EXPECT_EQ(idiv(r8), (internal::make_array<std::uint8_t>(0x49, 0xF7, 0xF8)));
}

TEST(IdivInstructions, Register32) {
    // idiv eax: F7 F8
    EXPECT_EQ(idiv(eax), (internal::make_array<std::uint8_t>(0xF7, 0xF8)));
    // idiv r8d: 41 F7 F8
    EXPECT_EQ(idiv(r8d), (internal::make_array<std::uint8_t>(0x41, 0xF7, 0xF8)));
}

TEST(IdivInstructions, Register8) {
    // idiv al: F6 F8
    EXPECT_EQ(idiv(al), (internal::make_array<std::uint8_t>(0xF6, 0xF8)));
}
