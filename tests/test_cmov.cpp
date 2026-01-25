#include <gtest/gtest.h>
#include "static_asm.hpp"

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// =============================================================================
// CMOV - Conditional Move Instructions
// =============================================================================
// Encoding: 0F 4x /r
// Form: cmovCC r16/32/64, r/m16/32/64
// All CMOV instructions use the 0F prefix followed by 4x opcode
// =============================================================================

// CMOVO (0F 40) - Move if overflow (OF=1)
TEST(CmovInstructions, CMOVO) {
    // cmovo eax, ecx -> 0F 40 C1
    EXPECT_EQ(cmovo(eax, ecx), (internal::make_array<std::uint8_t>(0x0F, 0x40, 0xC1)));
    // cmovo rax, rcx -> 48 0F 40 C1 (REX.W for 64-bit)
    EXPECT_EQ(cmovo(rax, rcx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0x40, 0xC1)));
    // cmovo r8, r9 -> 4D 0F 40 C1 (REX.W + REX.R + REX.B)
    EXPECT_EQ(cmovo(r8, r9), (internal::make_array<std::uint8_t>(0x4D, 0x0F, 0x40, 0xC1)));
}

// CMOVNO (0F 41) - Move if not overflow (OF=0)
TEST(CmovInstructions, CMOVNO) {
    EXPECT_EQ(cmovno(eax, ecx), (internal::make_array<std::uint8_t>(0x0F, 0x41, 0xC1)));
    EXPECT_EQ(cmovno(rax, rcx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0x41, 0xC1)));
}

// CMOVB/CMOVC/CMOVNAE (0F 42) - Move if below/carry (CF=1)
TEST(CmovInstructions, CMOVB) {
    EXPECT_EQ(cmovb(eax, ecx), (internal::make_array<std::uint8_t>(0x0F, 0x42, 0xC1)));
    EXPECT_EQ(cmovb(rax, rcx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0x42, 0xC1)));
    // Aliases should produce the same encoding
    EXPECT_EQ(cmovc(eax, ecx), cmovb(eax, ecx));
    EXPECT_EQ(cmovnae(eax, ecx), cmovb(eax, ecx));
}

// CMOVAE/CMOVNB/CMOVNC (0F 43) - Move if above or equal (CF=0)
TEST(CmovInstructions, CMOVAE) {
    EXPECT_EQ(cmovae(eax, ecx), (internal::make_array<std::uint8_t>(0x0F, 0x43, 0xC1)));
    EXPECT_EQ(cmovae(rax, rcx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0x43, 0xC1)));
    // Aliases
    EXPECT_EQ(cmovnb(eax, ecx), cmovae(eax, ecx));
    EXPECT_EQ(cmovnc(eax, ecx), cmovae(eax, ecx));
}

// CMOVE/CMOVZ (0F 44) - Move if equal/zero (ZF=1)
TEST(CmovInstructions, CMOVE) {
    EXPECT_EQ(cmove(eax, ecx), (internal::make_array<std::uint8_t>(0x0F, 0x44, 0xC1)));
    EXPECT_EQ(cmove(rax, rcx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0x44, 0xC1)));
    // Alias
    EXPECT_EQ(cmovz(eax, ecx), cmove(eax, ecx));
}

// CMOVNE/CMOVNZ (0F 45) - Move if not equal/not zero (ZF=0)
TEST(CmovInstructions, CMOVNE) {
    EXPECT_EQ(cmovne(eax, ecx), (internal::make_array<std::uint8_t>(0x0F, 0x45, 0xC1)));
    EXPECT_EQ(cmovne(rax, rcx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0x45, 0xC1)));
    // Alias
    EXPECT_EQ(cmovnz(eax, ecx), cmovne(eax, ecx));
}

// CMOVBE/CMOVNA (0F 46) - Move if below or equal (CF=1 or ZF=1)
TEST(CmovInstructions, CMOVBE) {
    EXPECT_EQ(cmovbe(eax, ecx), (internal::make_array<std::uint8_t>(0x0F, 0x46, 0xC1)));
    EXPECT_EQ(cmovbe(rax, rcx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0x46, 0xC1)));
    // Alias
    EXPECT_EQ(cmovna(eax, ecx), cmovbe(eax, ecx));
}

// CMOVA/CMOVNBE (0F 47) - Move if above (CF=0 and ZF=0)
TEST(CmovInstructions, CMOVA) {
    EXPECT_EQ(cmova(eax, ecx), (internal::make_array<std::uint8_t>(0x0F, 0x47, 0xC1)));
    EXPECT_EQ(cmova(rax, rcx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0x47, 0xC1)));
    // Alias
    EXPECT_EQ(cmovnbe(eax, ecx), cmova(eax, ecx));
}

// CMOVS (0F 48) - Move if sign (SF=1)
TEST(CmovInstructions, CMOVS) {
    EXPECT_EQ(cmovs(eax, ecx), (internal::make_array<std::uint8_t>(0x0F, 0x48, 0xC1)));
    EXPECT_EQ(cmovs(rax, rcx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0x48, 0xC1)));
}

// CMOVNS (0F 49) - Move if not sign (SF=0)
TEST(CmovInstructions, CMOVNS) {
    EXPECT_EQ(cmovns(eax, ecx), (internal::make_array<std::uint8_t>(0x0F, 0x49, 0xC1)));
    EXPECT_EQ(cmovns(rax, rcx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0x49, 0xC1)));
}

// CMOVP/CMOVPE (0F 4A) - Move if parity (PF=1)
TEST(CmovInstructions, CMOVP) {
    EXPECT_EQ(cmovp(eax, ecx), (internal::make_array<std::uint8_t>(0x0F, 0x4A, 0xC1)));
    EXPECT_EQ(cmovp(rax, rcx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0x4A, 0xC1)));
    // Alias
    EXPECT_EQ(cmovpe(eax, ecx), cmovp(eax, ecx));
}

// CMOVNP/CMOVPO (0F 4B) - Move if not parity (PF=0)
TEST(CmovInstructions, CMOVNP) {
    EXPECT_EQ(cmovnp(eax, ecx), (internal::make_array<std::uint8_t>(0x0F, 0x4B, 0xC1)));
    EXPECT_EQ(cmovnp(rax, rcx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0x4B, 0xC1)));
    // Alias
    EXPECT_EQ(cmovpo(eax, ecx), cmovnp(eax, ecx));
}

// CMOVL/CMOVNGE (0F 4C) - Move if less (SF!=OF)
TEST(CmovInstructions, CMOVL) {
    EXPECT_EQ(cmovl(eax, ecx), (internal::make_array<std::uint8_t>(0x0F, 0x4C, 0xC1)));
    EXPECT_EQ(cmovl(rax, rcx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0x4C, 0xC1)));
    // Alias
    EXPECT_EQ(cmovnge(eax, ecx), cmovl(eax, ecx));
}

// CMOVGE/CMOVNL (0F 4D) - Move if greater or equal (SF=OF)
TEST(CmovInstructions, CMOVGE) {
    EXPECT_EQ(cmovge(eax, ecx), (internal::make_array<std::uint8_t>(0x0F, 0x4D, 0xC1)));
    EXPECT_EQ(cmovge(rax, rcx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0x4D, 0xC1)));
    // Alias
    EXPECT_EQ(cmovnl(eax, ecx), cmovge(eax, ecx));
}

// CMOVLE/CMOVNG (0F 4E) - Move if less or equal (ZF=1 or SF!=OF)
TEST(CmovInstructions, CMOVLE) {
    EXPECT_EQ(cmovle(eax, ecx), (internal::make_array<std::uint8_t>(0x0F, 0x4E, 0xC1)));
    EXPECT_EQ(cmovle(rax, rcx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0x4E, 0xC1)));
    // Alias
    EXPECT_EQ(cmovng(eax, ecx), cmovle(eax, ecx));
}

// CMOVG/CMOVNLE (0F 4F) - Move if greater (ZF=0 and SF=OF)
TEST(CmovInstructions, CMOVG) {
    EXPECT_EQ(cmovg(eax, ecx), (internal::make_array<std::uint8_t>(0x0F, 0x4F, 0xC1)));
    EXPECT_EQ(cmovg(rax, rcx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0x4F, 0xC1)));
    // Alias
    EXPECT_EQ(cmovnle(eax, ecx), cmovg(eax, ecx));
}

// Test with 16-bit registers (requires 66 prefix)
TEST(CmovInstructions, Register16Bit) {
    // cmove ax, cx -> 66 0F 44 C1
    EXPECT_EQ(cmove(ax, cx), (internal::make_array<std::uint8_t>(0x66, 0x0F, 0x44, 0xC1)));
    // cmovne bx, dx -> 66 0F 45 DA
    EXPECT_EQ(cmovne(bx, dx), (internal::make_array<std::uint8_t>(0x66, 0x0F, 0x45, 0xDA)));
}

// Test with extended registers (r8-r15)
TEST(CmovInstructions, ExtendedRegisters) {
    // cmove r8d, r9d -> 45 0F 44 C1 (REX.R + REX.B)
    EXPECT_EQ(cmove(r8d, r9d), (internal::make_array<std::uint8_t>(0x45, 0x0F, 0x44, 0xC1)));
    // cmove r8, r9 -> 4D 0F 44 C1 (REX.W + REX.R + REX.B)
    EXPECT_EQ(cmove(r8, r9), (internal::make_array<std::uint8_t>(0x4D, 0x0F, 0x44, 0xC1)));
    // cmove rax, r8 -> 49 0F 44 C0 (REX.W + REX.B)
    EXPECT_EQ(cmove(rax, r8), (internal::make_array<std::uint8_t>(0x49, 0x0F, 0x44, 0xC0)));
    // cmove r8, rax -> 4C 0F 44 C0 (REX.W + REX.R)
    EXPECT_EQ(cmove(r8, rax), (internal::make_array<std::uint8_t>(0x4C, 0x0F, 0x44, 0xC0)));
}

// Test various register combinations
TEST(CmovInstructions, VariousRegisters) {
    // cmove ebx, edx -> 0F 44 DA
    EXPECT_EQ(cmove(ebx, edx), (internal::make_array<std::uint8_t>(0x0F, 0x44, 0xDA)));
    // cmove esi, edi -> 0F 44 F7
    EXPECT_EQ(cmove(esi, edi), (internal::make_array<std::uint8_t>(0x0F, 0x44, 0xF7)));
    // cmove rbx, rdx -> 48 0F 44 DA
    EXPECT_EQ(cmove(rbx, rdx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0x44, 0xDA)));
}
