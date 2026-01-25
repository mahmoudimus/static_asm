#include "static_asm.hpp"
#include <gtest/gtest.h>

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// XCHG tests - exchange register contents
// Note: XCHG is commutative, so both operand orderings produce valid x86 code.
// The encoder places first operand in reg field (REX.R) and second in r/m field (REX.B).
TEST(XchgInstructions, Register64) {
    // xchg rax, rcx: 48 87 C8 - rax in reg, rcx in r/m
    EXPECT_EQ(xchg(rax, rcx), (internal::make_array<std::uint8_t>(0x48, 0x87, 0xC8)));
    // xchg rcx, rax: 48 87 C1 - rcx in reg, rax in r/m
    EXPECT_EQ(xchg(rcx, rax), (internal::make_array<std::uint8_t>(0x48, 0x87, 0xC1)));
    // xchg r8, r9: 4D 87 C8 - r8 in reg (REX.R), r9 in r/m (REX.B)
    EXPECT_EQ(xchg(r8, r9), (internal::make_array<std::uint8_t>(0x4D, 0x87, 0xC8)));
    // xchg rax, r8: 4C 87 C0 - rax in reg, r8 in r/m (REX.WB)
    EXPECT_EQ(xchg(rax, r8), (internal::make_array<std::uint8_t>(0x4C, 0x87, 0xC0)));
    // xchg r8, rax: 49 87 C0 - r8 in reg (REX.WR), rax in r/m
    EXPECT_EQ(xchg(r8, rax), (internal::make_array<std::uint8_t>(0x49, 0x87, 0xC0)));
}

TEST(XchgInstructions, Register32) {
    // xchg eax, ecx: 87 C8
    EXPECT_EQ(xchg(eax, ecx), (internal::make_array<std::uint8_t>(0x87, 0xC8)));
    // xchg r8d, r9d: 45 87 C8 - r8d in reg (REX.R), r9d in r/m (REX.B)
    EXPECT_EQ(xchg(r8d, r9d), (internal::make_array<std::uint8_t>(0x45, 0x87, 0xC8)));
    // xchg eax, r8d: 44 87 C0 - eax in reg, r8d in r/m (REX.B)
    EXPECT_EQ(xchg(eax, r8d), (internal::make_array<std::uint8_t>(0x44, 0x87, 0xC0)));
}

TEST(XchgInstructions, Register8) {
    // xchg al, cl: 86 C8
    EXPECT_EQ(xchg(al, cl), (internal::make_array<std::uint8_t>(0x86, 0xC8)));
    // xchg cl, al: 86 C1
    EXPECT_EQ(xchg(cl, al), (internal::make_array<std::uint8_t>(0x86, 0xC1)));
}

TEST(XchgInstructions, Register16) {
    // xchg ax, cx: 66 87 C8
    EXPECT_EQ(xchg(ax, cx), (internal::make_array<std::uint8_t>(0x66, 0x87, 0xC8)));
}
