#include <gtest/gtest.h>
#include "static_asm.hpp"

// Auto-generated exhaustive tests from x86reference.xml

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;


TEST(GeneratedAluTests, ADD_Reg32_Reg32) {
    // ADD eax, ecx
    auto result = add(eax, ecx);
    EXPECT_GE(result.size(), 2u);
}

TEST(GeneratedAluTests, ADD_Reg64_Reg64) {
    // ADD rax, rcx (needs REX.W)
    auto result = add(rax, rcx);
    EXPECT_GE(result.size(), 3u);
    EXPECT_EQ(result[0] & 0xF8, 0x48); // REX.W prefix
}

TEST(GeneratedAluTests, ADD_Reg64_Imm32) {
    // ADD rax, 0x12345678
    auto result = add(rax, 0x12345678);
    EXPECT_GE(result.size(), 6u);
}


TEST(GeneratedAluTests, SUB_Reg32_Reg32) {
    // SUB eax, ecx
    auto result = sub(eax, ecx);
    EXPECT_GE(result.size(), 2u);
}

TEST(GeneratedAluTests, SUB_Reg64_Reg64) {
    // SUB rax, rcx (needs REX.W)
    auto result = sub(rax, rcx);
    EXPECT_GE(result.size(), 3u);
    EXPECT_EQ(result[0] & 0xF8, 0x48); // REX.W prefix
}

TEST(GeneratedAluTests, SUB_Reg64_Imm32) {
    // SUB rax, 0x12345678
    auto result = sub(rax, 0x12345678);
    EXPECT_GE(result.size(), 6u);
}


TEST(GeneratedAluTests, AND_Reg32_Reg32) {
    // AND eax, ecx
    auto result = and_(eax, ecx);
    EXPECT_GE(result.size(), 2u);
}

TEST(GeneratedAluTests, AND_Reg64_Reg64) {
    // AND rax, rcx (needs REX.W)
    auto result = and_(rax, rcx);
    EXPECT_GE(result.size(), 3u);
    EXPECT_EQ(result[0] & 0xF8, 0x48); // REX.W prefix
}

TEST(GeneratedAluTests, AND_Reg64_Imm32) {
    // AND rax, 0x12345678
    auto result = and_(rax, 0x12345678);
    EXPECT_GE(result.size(), 6u);
}


TEST(GeneratedAluTests, OR_Reg32_Reg32) {
    // OR eax, ecx
    auto result = or_(eax, ecx);
    EXPECT_GE(result.size(), 2u);
}

TEST(GeneratedAluTests, OR_Reg64_Reg64) {
    // OR rax, rcx (needs REX.W)
    auto result = or_(rax, rcx);
    EXPECT_GE(result.size(), 3u);
    EXPECT_EQ(result[0] & 0xF8, 0x48); // REX.W prefix
}

TEST(GeneratedAluTests, OR_Reg64_Imm32) {
    // OR rax, 0x12345678
    auto result = or_(rax, 0x12345678);
    EXPECT_GE(result.size(), 6u);
}


TEST(GeneratedAluTests, XOR_Reg32_Reg32) {
    // XOR eax, ecx
    auto result = xor_(eax, ecx);
    EXPECT_GE(result.size(), 2u);
}

TEST(GeneratedAluTests, XOR_Reg64_Reg64) {
    // XOR rax, rcx (needs REX.W)
    auto result = xor_(rax, rcx);
    EXPECT_GE(result.size(), 3u);
    EXPECT_EQ(result[0] & 0xF8, 0x48); // REX.W prefix
}

TEST(GeneratedAluTests, XOR_Reg64_Imm32) {
    // XOR rax, 0x12345678
    auto result = xor_(rax, 0x12345678);
    EXPECT_GE(result.size(), 6u);
}


TEST(GeneratedAluTests, CMP_Reg32_Reg32) {
    // CMP eax, ecx
    auto result = cmp(eax, ecx);
    EXPECT_GE(result.size(), 2u);
}

TEST(GeneratedAluTests, CMP_Reg64_Reg64) {
    // CMP rax, rcx (needs REX.W)
    auto result = cmp(rax, rcx);
    EXPECT_GE(result.size(), 3u);
    EXPECT_EQ(result[0] & 0xF8, 0x48); // REX.W prefix
}

TEST(GeneratedAluTests, CMP_Reg64_Imm32) {
    // CMP rax, 0x12345678
    auto result = cmp(rax, 0x12345678);
    EXPECT_GE(result.size(), 6u);
}


TEST(GeneratedAluTests, ADC_Reg32_Reg32) {
    // ADC eax, ecx
    auto result = adc(eax, ecx);
    EXPECT_GE(result.size(), 2u);
}

TEST(GeneratedAluTests, ADC_Reg64_Reg64) {
    // ADC rax, rcx (needs REX.W)
    auto result = adc(rax, rcx);
    EXPECT_GE(result.size(), 3u);
    EXPECT_EQ(result[0] & 0xF8, 0x48); // REX.W prefix
}

TEST(GeneratedAluTests, ADC_Reg64_Imm32) {
    // ADC rax, 0x12345678
    auto result = adc(rax, 0x12345678);
    EXPECT_GE(result.size(), 6u);
}


TEST(GeneratedAluTests, SBB_Reg32_Reg32) {
    // SBB eax, ecx
    auto result = sbb(eax, ecx);
    EXPECT_GE(result.size(), 2u);
}

TEST(GeneratedAluTests, SBB_Reg64_Reg64) {
    // SBB rax, rcx (needs REX.W)
    auto result = sbb(rax, rcx);
    EXPECT_GE(result.size(), 3u);
    EXPECT_EQ(result[0] & 0xF8, 0x48); // REX.W prefix
}

TEST(GeneratedAluTests, SBB_Reg64_Imm32) {
    // SBB rax, 0x12345678
    auto result = sbb(rax, 0x12345678);
    EXPECT_GE(result.size(), 6u);
}


TEST(GeneratedMovTests, MOV_Reg32_Reg32) {
    auto result = mov(eax, ecx);
    EXPECT_EQ(result.size(), 2u);
}

TEST(GeneratedMovTests, MOV_Reg64_Reg64) {
    auto result = mov(rax, rcx);
    EXPECT_GE(result.size(), 3u);
    EXPECT_EQ(result[0] & 0xF8, 0x48); // REX.W
}

TEST(GeneratedMovTests, MOV_Reg64_Imm64) {
    // Should auto-detect movabs for 64-bit immediates
    constexpr std::uint64_t large_val = 0x123456789ABCDEF0ULL;
    auto result = mov(rax, large_val);
    EXPECT_EQ(result.size(), 10u); // REX.W + B8 + 8 bytes
}

TEST(GeneratedMovTests, MOV_ExtendedReg) {
    auto result = mov(r8, r9);
    EXPECT_GE(result.size(), 3u);
    // Should have REX prefix with R and B bits
}


TEST(GeneratedJmpTests, JMP_Rel32) {
    auto result = jmp(0x12345678);
    EXPECT_EQ(result.size(), 5u);
    EXPECT_EQ(result[0], 0xE9);
}

TEST(GeneratedJmpTests, JMP_Reg64) {
    auto result = jmp(rax);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0], 0xFF);
}

TEST(GeneratedJmpTests, JMP_ExtendedReg) {
    auto result = jmp(r8);
    EXPECT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], 0x41); // REX.B
    EXPECT_EQ(result[1], 0xFF);
}


TEST(GeneratedCallTests, CALL_Rel32) {
    auto result = call(0x12345678);
    EXPECT_EQ(result.size(), 5u);
    EXPECT_EQ(result[0], 0xE8);
}

TEST(GeneratedCallTests, CALL_Reg64) {
    auto result = call(rax);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0], 0xFF);
}

TEST(GeneratedCallTests, CALL_ExtendedReg) {
    auto result = call(r8);
    EXPECT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], 0x41); // REX.B
}


TEST(GeneratedStackTests, PUSH_Reg64) {
    auto result = push(rax);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], 0x50);
}

TEST(GeneratedStackTests, PUSH_ExtendedReg) {
    auto result = push(r8);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0], 0x41); // REX.B
    EXPECT_EQ(result[1], 0x50);
}


TEST(GeneratedStackTests, POP_Reg64) {
    auto result = pop(rax);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], 0x58);
}

TEST(GeneratedStackTests, POP_ExtendedReg) {
    auto result = pop(r8);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0], 0x41); // REX.B
}


TEST(GeneratedJccTests, JZ_Rel8) {
    auto result = jz(0x10);
    EXPECT_EQ(result.size(), 2u);
}


TEST(GeneratedJccTests, JNZ_Rel8) {
    auto result = jnz(0x10);
    EXPECT_EQ(result.size(), 2u);
}


TEST(GeneratedJccTests, JB_Rel8) {
    auto result = jb(0x10);
    EXPECT_EQ(result.size(), 2u);
}


TEST(GeneratedJccTests, JNB_Rel8) {
    auto result = jnb(0x10);
    EXPECT_EQ(result.size(), 2u);
}


TEST(GeneratedJccTests, JBE_Rel8) {
    auto result = jbe(0x10);
    EXPECT_EQ(result.size(), 2u);
}


TEST(GeneratedJccTests, JNBE_Rel8) {
    auto result = jnbe(0x10);
    EXPECT_EQ(result.size(), 2u);
}


TEST(GeneratedJccTests, JL_Rel8) {
    auto result = jl(0x10);
    EXPECT_EQ(result.size(), 2u);
}


TEST(GeneratedJccTests, JNL_Rel8) {
    auto result = jnl(0x10);
    EXPECT_EQ(result.size(), 2u);
}


TEST(GeneratedJccTests, JLE_Rel8) {
    auto result = jle(0x10);
    EXPECT_EQ(result.size(), 2u);
}


TEST(GeneratedJccTests, JNLE_Rel8) {
    auto result = jnle(0x10);
    EXPECT_EQ(result.size(), 2u);
}


TEST(GeneratedNopTests, NOP_Single) {
    auto result = nop();
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], 0x90);
}
