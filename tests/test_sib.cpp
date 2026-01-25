#include <gtest/gtest.h>
#include "static_asm.hpp"

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// =============================================================================
// SIB Addressing Tests: mov reg, [base + index*scale]
// =============================================================================

TEST(SIBTests, MovRegFromSIB_Scale1) {
    // mov eax, [rbx + rcx]
    // Verified with defuse.ca: 8B 04 0B
    auto result = mov(eax, dword_ptr(rbx + rcx));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x8B, 0x04, 0x0B)));
}

TEST(SIBTests, MovRegFromSIB_Scale2) {
    // mov eax, [rbx + rcx*2]
    // Verified: 8B 04 4B
    auto result = mov(eax, dword_ptr(rbx + rcx * s2));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x8B, 0x04, 0x4B)));
}

TEST(SIBTests, MovRegFromSIB_Scale4) {
    // mov eax, [rbx + rcx*4]
    // Verified: 8B 04 8B
    auto result = mov(eax, dword_ptr(rbx + rcx * s4));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x8B, 0x04, 0x8B)));
}

TEST(SIBTests, MovRegFromSIB_Scale8) {
    // mov eax, [rbx + rcx*8]
    // Verified: 8B 04 CB
    auto result = mov(eax, dword_ptr(rbx + rcx * s8));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x8B, 0x04, 0xCB)));
}

// =============================================================================
// SIB with Displacement Tests
// =============================================================================

TEST(SIBTests, MovRegFromSIB_Scale1_Disp8) {
    // mov eax, [rbx + rcx + 0x10]
    // Verified: 8B 44 0B 10
    auto result = mov(eax, dword_ptr(rbx + rcx + std::int8_t(0x10)));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x8B, 0x44, 0x0B, 0x10)));
}

TEST(SIBTests, MovRegFromSIB_Scale4_Disp8) {
    // mov eax, [rbx + rcx*4 + 0x10]
    // Verified: 8B 44 8B 10
    auto result = mov(eax, dword_ptr(rbx + rcx * s4 + std::int8_t(0x10)));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x8B, 0x44, 0x8B, 0x10)));
}

TEST(SIBTests, MovRegFromSIB_Scale8_Disp8) {
    // mov eax, [rbx + rcx*8 + 0x10]
    // Verified: 8B 44 CB 10
    auto result = mov(eax, dword_ptr(rbx + rcx * s8 + std::int8_t(0x10)));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x8B, 0x44, 0xCB, 0x10)));
}

TEST(SIBTests, MovRegFromSIB_Scale4_Disp32) {
    // mov eax, [rbx + rcx*4 + 0x12345678]
    // Verified: 8B 84 8B 78 56 34 12
    auto result = mov(eax, dword_ptr(rbx + rcx * s4 + 0x12345678));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x8B, 0x84, 0x8B, 0x78, 0x56, 0x34, 0x12)));
}

// =============================================================================
// SIB with 64-bit Registers (REX.W)
// =============================================================================

TEST(SIBTests, MovReg64FromSIB_Scale4) {
    // mov rax, [rbx + rcx*4]
    // Verified: 48 8B 04 8B
    auto result = mov(rax, qword_ptr(rbx + rcx * s4));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x48, 0x8B, 0x04, 0x8B)));
}

TEST(SIBTests, MovReg64FromSIB_Scale4_Disp8) {
    // mov rax, [rbx + rcx*4 + 0x10]
    // Verified: 48 8B 44 8B 10
    auto result = mov(rax, qword_ptr(rbx + rcx * s4 + std::int8_t(0x10)));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x48, 0x8B, 0x44, 0x8B, 0x10)));
}

TEST(SIBTests, MovReg64FromSIB_Scale4_Disp32) {
    // mov rax, [rbx + rcx*4 + 0x12345678]
    // Verified: 48 8B 84 8B 78 56 34 12
    auto result = mov(rax, qword_ptr(rbx + rcx * s4 + 0x12345678));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x48, 0x8B, 0x84, 0x8B, 0x78, 0x56, 0x34, 0x12)));
}

// =============================================================================
// SIB with Extended Registers (REX.R, REX.X, REX.B)
// =============================================================================

TEST(SIBTests, MovExtendedRegFromSIB) {
    // mov r8d, [rbx + rcx*4]
    // REX.R=1 for r8d: 44 8B 04 8B
    auto result = mov(r8d, dword_ptr(rbx + rcx * s4));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x44, 0x8B, 0x04, 0x8B)));
}

TEST(SIBTests, MovRegFromSIB_ExtendedBase) {
    // mov eax, [r9 + rcx*4]
    // REX.B=1 for r9: 41 8B 04 89
    auto result = mov(eax, dword_ptr(r9 + rcx * s4));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x41, 0x8B, 0x04, 0x89)));
}

TEST(SIBTests, MovRegFromSIB_ExtendedIndex) {
    // mov eax, [rbx + r10*4]
    // REX.X=1 for r10: 42 8B 04 93
    auto result = mov(eax, dword_ptr(rbx + r10 * s4));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x42, 0x8B, 0x04, 0x93)));
}

TEST(SIBTests, MovRegFromSIB_AllExtended) {
    // mov r8d, [r9 + r10*4]
    // REX.RXB=111: 47 8B 04 91
    auto result = mov(r8d, dword_ptr(r9 + r10 * s4));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x47, 0x8B, 0x04, 0x91)));
}

TEST(SIBTests, MovReg64FromSIB_AllExtended) {
    // mov r8, [r9 + r10*4]
    // REX.WRXB=1111: 4F 8B 04 91
    auto result = mov(r8, qword_ptr(r9 + r10 * s4));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x4F, 0x8B, 0x04, 0x91)));
}

// =============================================================================
// MOV to SIB Memory (Memory as Destination)
// =============================================================================

TEST(SIBTests, MovToSIB_Scale4) {
    // mov [rbx + rcx*4], eax
    // Verified: 89 04 8B
    auto result = mov(dword_ptr(rbx + rcx * s4), eax);
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x89, 0x04, 0x8B)));
}

TEST(SIBTests, MovToSIB_Scale4_Disp8) {
    // mov [rbx + rcx*4 + 0x10], eax
    // Verified: 89 44 8B 10
    auto result = mov(dword_ptr(rbx + rcx * s4 + std::int8_t(0x10)), eax);
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x89, 0x44, 0x8B, 0x10)));
}

TEST(SIBTests, MovToSIB_Reg64) {
    // mov [rbx + rcx*4], rax
    // Verified: 48 89 04 8B
    auto result = mov(qword_ptr(rbx + rcx * s4), rax);
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x48, 0x89, 0x04, 0x8B)));
}

// =============================================================================
// LEA with SIB Addressing
// =============================================================================

TEST(SIBTests, LeaWithSIB_Scale1) {
    // lea rax, [rbx + rcx]
    // Verified: 48 8D 04 0B
    auto result = lea(rax, ptr(rbx + rcx));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x48, 0x8D, 0x04, 0x0B)));
}

TEST(SIBTests, LeaWithSIB_Scale4) {
    // lea rax, [rbx + rcx*4]
    // Verified: 48 8D 04 8B
    auto result = lea(rax, ptr(rbx + rcx * s4));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x48, 0x8D, 0x04, 0x8B)));
}

TEST(SIBTests, LeaWithSIB_Scale4_Disp8) {
    // lea rax, [rbx + rcx*4 + 0x10]
    // Verified: 48 8D 44 8B 10
    auto result = lea(rax, ptr(rbx + rcx * s4 + std::int8_t(0x10)));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x48, 0x8D, 0x44, 0x8B, 0x10)));
}

TEST(SIBTests, LeaWithSIB_Scale8_Disp32) {
    // lea rax, [rbx + rcx*8 + 0x12345678]
    // Verified: 48 8D 84 CB 78 56 34 12
    auto result = lea(rax, ptr(rbx + rcx * s8 + 0x12345678));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x48, 0x8D, 0x84, 0xCB, 0x78, 0x56, 0x34, 0x12)));
}

TEST(SIBTests, LeaWithSIB_32bit) {
    // lea eax, [rbx + rcx*4]
    // Verified: 8D 04 8B
    auto result = lea(eax, ptr(rbx + rcx * s4));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x8D, 0x04, 0x8B)));
}

// =============================================================================
// ALU Instructions with SIB Addressing
// =============================================================================

TEST(SIBTests, AddRegFromSIB) {
    // add eax, [rbx + rcx*4]
    // Verified: 03 04 8B
    auto result = add(eax, dword_ptr(rbx + rcx * s4));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x03, 0x04, 0x8B)));
}

TEST(SIBTests, AddRegFromSIB_Disp8) {
    // add eax, [rbx + rcx*4 + 0x10]
    // Verified: 03 44 8B 10
    auto result = add(eax, dword_ptr(rbx + rcx * s4 + std::int8_t(0x10)));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x03, 0x44, 0x8B, 0x10)));
}

TEST(SIBTests, AddReg64FromSIB) {
    // add rax, [rbx + rcx*4]
    // Verified: 48 03 04 8B
    auto result = add(rax, qword_ptr(rbx + rcx * s4));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x48, 0x03, 0x04, 0x8B)));
}

TEST(SIBTests, SubRegFromSIB) {
    // sub eax, [rbx + rcx*4]
    // Verified: 2B 04 8B
    auto result = sub(eax, dword_ptr(rbx + rcx * s4));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x2B, 0x04, 0x8B)));
}

TEST(SIBTests, CmpRegFromSIB) {
    // cmp eax, [rbx + rcx*4]
    // Verified: 3B 04 8B
    auto result = cmp(eax, dword_ptr(rbx + rcx * s4));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x3B, 0x04, 0x8B)));
}

TEST(SIBTests, AndRegFromSIB) {
    // and eax, [rbx + rcx*4]
    // Verified: 23 04 8B
    auto result = and_(eax, dword_ptr(rbx + rcx * s4));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x23, 0x04, 0x8B)));
}

TEST(SIBTests, OrRegFromSIB) {
    // or eax, [rbx + rcx*4]
    // Verified: 0B 04 8B
    auto result = or_(eax, dword_ptr(rbx + rcx * s4));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x0B, 0x04, 0x8B)));
}

TEST(SIBTests, XorRegFromSIB) {
    // xor eax, [rbx + rcx*4]
    // Verified: 33 04 8B
    auto result = xor_(eax, dword_ptr(rbx + rcx * s4));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x33, 0x04, 0x8B)));
}

// =============================================================================
// ALU to SIB Memory (Memory as Destination)
// =============================================================================

TEST(SIBTests, AddToSIB) {
    // add [rbx + rcx*4], eax
    // Verified: 01 04 8B
    auto result = add(dword_ptr(rbx + rcx * s4), eax);
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x01, 0x04, 0x8B)));
}

TEST(SIBTests, AddToSIB_Disp8) {
    // add [rbx + rcx*4 + 0x10], eax
    // Verified: 01 44 8B 10
    auto result = add(dword_ptr(rbx + rcx * s4 + std::int8_t(0x10)), eax);
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x01, 0x44, 0x8B, 0x10)));
}

TEST(SIBTests, SubToSIB) {
    // sub [rbx + rcx*4], eax
    // Verified: 29 04 8B
    auto result = sub(dword_ptr(rbx + rcx * s4), eax);
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x29, 0x04, 0x8B)));
}

// =============================================================================
// Edge Cases: RBP/R13 as Base (Special Encoding)
// =============================================================================

// When RBP/R13 is used as base with mod=00, it means "no base + disp32"
// So for correct encoding, users should use explicit displacement with RBP/R13

TEST(SIBTests, MovRegFromSIB_RBPBase_Disp8) {
    // mov eax, [rbp + rcx*4 + 0x10]
    // Using explicit disp8 for RBP base
    // Verified: 8B 44 8D 10
    auto result = mov(eax, dword_ptr(rbp + rcx * s4 + std::int8_t(0x10)));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x8B, 0x44, 0x8D, 0x10)));
}

TEST(SIBTests, MovRegFromSIB_R13Base_Disp8) {
    // mov eax, [r13 + rcx*4 + 0x10]
    // REX.B=1 for r13, explicit disp8
    // Verified: 41 8B 44 8D 10
    auto result = mov(eax, dword_ptr(r13 + rcx * s4 + std::int8_t(0x10)));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x41, 0x8B, 0x44, 0x8D, 0x10)));
}

TEST(SIBTests, MovRegFromSIB_RBPBase_ZeroDisp) {
    // mov eax, [rbp + rcx*4 + 0]
    // For RBP with zero displacement, we encode as disp8=0
    // Verified: 8B 44 8D 00
    auto result = mov(eax, dword_ptr(rbp + rcx * s4 + std::int8_t(0)));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x8B, 0x44, 0x8D, 0x00)));
}

// =============================================================================
// 16-bit Operations with SIB
// =============================================================================

TEST(SIBTests, MovReg16FromSIB) {
    // mov ax, [rbx + rcx*4]
    // 66 prefix for 16-bit operand
    // Verified: 66 8B 04 8B
    auto result = mov(ax, word_ptr(rbx + rcx * s4));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x66, 0x8B, 0x04, 0x8B)));
}

TEST(SIBTests, MovToSIB_Reg16) {
    // mov [rbx + rcx*4], ax
    // Verified: 66 89 04 8B
    auto result = mov(word_ptr(rbx + rcx * s4), ax);
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x66, 0x89, 0x04, 0x8B)));
}

// =============================================================================
// 8-bit Operations with SIB
// =============================================================================

TEST(SIBTests, MovReg8FromSIB) {
    // mov al, [rbx + rcx*4]
    // 8A for 8-bit mov
    // Verified: 8A 04 8B
    auto result = mov(al, byte_ptr(rbx + rcx * s4));
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x8A, 0x04, 0x8B)));
}

TEST(SIBTests, MovToSIB_Reg8) {
    // mov [rbx + rcx*4], al
    // Verified: 88 04 8B
    auto result = mov(byte_ptr(rbx + rcx * s4), al);
    EXPECT_EQ(result, (internal::make_array<std::uint8_t>(0x88, 0x04, 0x8B)));
}

// =============================================================================
// Compile-time Evaluation Tests
// =============================================================================

TEST(SIBTests, ConstexprEvaluation) {
    // Verify that all SIB encoding can be evaluated at compile time
    constexpr auto mov_sib = mov(eax, dword_ptr(rbx + rcx * s4));
    constexpr auto lea_sib = lea(rax, ptr(rbx + rcx * s4));
    constexpr auto add_sib = add(eax, dword_ptr(rbx + rcx * s4));

    EXPECT_EQ(mov_sib.size(), 3u);
    EXPECT_EQ(lea_sib.size(), 4u);  // REX.W prefix
    EXPECT_EQ(add_sib.size(), 3u);
}
