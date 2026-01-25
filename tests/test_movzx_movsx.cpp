#include <gtest/gtest.h>
#include "static_asm.hpp"

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// =============================================================================
// MOVZX - Move with Zero Extension
// =============================================================================
// Encoding:
//   MOVZX r16, r/m8:  66 0F B6 /r
//   MOVZX r32, r/m8:  0F B6 /r
//   MOVZX r64, r/m8:  REX.W 0F B6 /r
//   MOVZX r32, r/m16: 0F B7 /r
//   MOVZX r64, r/m16: REX.W 0F B7 /r
// =============================================================================

// MOVZX r32, r8 - zero extend 8-bit to 32-bit
TEST(MovzxInstructions, R32_R8) {
    // movzx eax, bl -> 0F B6 C3
    EXPECT_EQ(movzx(eax, bl), (internal::make_array<std::uint8_t>(0x0F, 0xB6, 0xC3)));
    // movzx eax, cl -> 0F B6 C1
    EXPECT_EQ(movzx(eax, cl), (internal::make_array<std::uint8_t>(0x0F, 0xB6, 0xC1)));
    // movzx ecx, al -> 0F B6 C8
    EXPECT_EQ(movzx(ecx, al), (internal::make_array<std::uint8_t>(0x0F, 0xB6, 0xC8)));
    // movzx edx, bl -> 0F B6 D3
    EXPECT_EQ(movzx(edx, bl), (internal::make_array<std::uint8_t>(0x0F, 0xB6, 0xD3)));
}

// MOVZX r64, r8 - zero extend 8-bit to 64-bit
TEST(MovzxInstructions, R64_R8) {
    // movzx rax, bl -> 48 0F B6 C3
    EXPECT_EQ(movzx(rax, bl), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xB6, 0xC3)));
    // movzx rax, cl -> 48 0F B6 C1
    EXPECT_EQ(movzx(rax, cl), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xB6, 0xC1)));
    // movzx rcx, al -> 48 0F B6 C8
    EXPECT_EQ(movzx(rcx, al), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xB6, 0xC8)));
}

// MOVZX r32, r16 - zero extend 16-bit to 32-bit
TEST(MovzxInstructions, R32_R16) {
    // movzx eax, bx -> 0F B7 C3
    EXPECT_EQ(movzx(eax, bx), (internal::make_array<std::uint8_t>(0x0F, 0xB7, 0xC3)));
    // movzx ecx, ax -> 0F B7 C8
    EXPECT_EQ(movzx(ecx, ax), (internal::make_array<std::uint8_t>(0x0F, 0xB7, 0xC8)));
    // movzx edx, cx -> 0F B7 D1
    EXPECT_EQ(movzx(edx, cx), (internal::make_array<std::uint8_t>(0x0F, 0xB7, 0xD1)));
}

// MOVZX r64, r16 - zero extend 16-bit to 64-bit
TEST(MovzxInstructions, R64_R16) {
    // movzx rax, bx -> 48 0F B7 C3
    EXPECT_EQ(movzx(rax, bx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xB7, 0xC3)));
    // movzx rcx, ax -> 48 0F B7 C8
    EXPECT_EQ(movzx(rcx, ax), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xB7, 0xC8)));
}

// MOVZX r16, r8 - zero extend 8-bit to 16-bit (needs 66 prefix)
TEST(MovzxInstructions, R16_R8) {
    // movzx ax, bl -> 66 0F B6 C3
    EXPECT_EQ(movzx(ax, bl), (internal::make_array<std::uint8_t>(0x66, 0x0F, 0xB6, 0xC3)));
    // movzx cx, al -> 66 0F B6 C8
    EXPECT_EQ(movzx(cx, al), (internal::make_array<std::uint8_t>(0x66, 0x0F, 0xB6, 0xC8)));
}

// MOVZX with extended registers
TEST(MovzxInstructions, ExtendedRegisters) {
    // movzx r8d, cl -> 44 0F B6 C1 (REX.R)
    EXPECT_EQ(movzx(r8d, cl), (internal::make_array<std::uint8_t>(0x44, 0x0F, 0xB6, 0xC1)));
    // movzx r8, cl -> 4C 0F B6 C1 (REX.W + REX.R)
    EXPECT_EQ(movzx(r8, cl), (internal::make_array<std::uint8_t>(0x4C, 0x0F, 0xB6, 0xC1)));
    // movzx eax, r8b -> 41 0F B6 C0 (REX.B)
    EXPECT_EQ(movzx(eax, r8b), (internal::make_array<std::uint8_t>(0x41, 0x0F, 0xB6, 0xC0)));
    // movzx r9d, r10b -> 45 0F B6 CA (REX.R + REX.B)
    EXPECT_EQ(movzx(r9d, r10b), (internal::make_array<std::uint8_t>(0x45, 0x0F, 0xB6, 0xCA)));
    // movzx r8d, ax -> 44 0F B7 C0 (REX.R, 16-bit source)
    EXPECT_EQ(movzx(r8d, ax), (internal::make_array<std::uint8_t>(0x44, 0x0F, 0xB7, 0xC0)));
    // movzx r8d, r9w -> 45 0F B7 C1 (REX.R + REX.B, 16-bit source)
    EXPECT_EQ(movzx(r8d, r9w), (internal::make_array<std::uint8_t>(0x45, 0x0F, 0xB7, 0xC1)));
}

// =============================================================================
// MOVSX - Move with Sign Extension
// =============================================================================
// Encoding:
//   MOVSX r16, r/m8:  66 0F BE /r
//   MOVSX r32, r/m8:  0F BE /r
//   MOVSX r64, r/m8:  REX.W 0F BE /r
//   MOVSX r32, r/m16: 0F BF /r
//   MOVSX r64, r/m16: REX.W 0F BF /r
// =============================================================================

// MOVSX r32, r8 - sign extend 8-bit to 32-bit
TEST(MovsxInstructions, R32_R8) {
    // movsx eax, bl -> 0F BE C3
    EXPECT_EQ(movsx(eax, bl), (internal::make_array<std::uint8_t>(0x0F, 0xBE, 0xC3)));
    // movsx eax, cl -> 0F BE C1
    EXPECT_EQ(movsx(eax, cl), (internal::make_array<std::uint8_t>(0x0F, 0xBE, 0xC1)));
    // movsx ecx, al -> 0F BE C8
    EXPECT_EQ(movsx(ecx, al), (internal::make_array<std::uint8_t>(0x0F, 0xBE, 0xC8)));
}

// MOVSX r64, r8 - sign extend 8-bit to 64-bit
TEST(MovsxInstructions, R64_R8) {
    // movsx rax, bl -> 48 0F BE C3
    EXPECT_EQ(movsx(rax, bl), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xBE, 0xC3)));
    // movsx rax, cl -> 48 0F BE C1
    EXPECT_EQ(movsx(rax, cl), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xBE, 0xC1)));
}

// MOVSX r32, r16 - sign extend 16-bit to 32-bit
TEST(MovsxInstructions, R32_R16) {
    // movsx eax, bx -> 0F BF C3
    EXPECT_EQ(movsx(eax, bx), (internal::make_array<std::uint8_t>(0x0F, 0xBF, 0xC3)));
    // movsx ecx, ax -> 0F BF C8
    EXPECT_EQ(movsx(ecx, ax), (internal::make_array<std::uint8_t>(0x0F, 0xBF, 0xC8)));
}

// MOVSX r64, r16 - sign extend 16-bit to 64-bit
TEST(MovsxInstructions, R64_R16) {
    // movsx rax, bx -> 48 0F BF C3
    EXPECT_EQ(movsx(rax, bx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xBF, 0xC3)));
    // movsx rcx, ax -> 48 0F BF C8
    EXPECT_EQ(movsx(rcx, ax), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xBF, 0xC8)));
}

// MOVSX r16, r8 - sign extend 8-bit to 16-bit (needs 66 prefix)
TEST(MovsxInstructions, R16_R8) {
    // movsx ax, bl -> 66 0F BE C3
    EXPECT_EQ(movsx(ax, bl), (internal::make_array<std::uint8_t>(0x66, 0x0F, 0xBE, 0xC3)));
    // movsx cx, al -> 66 0F BE C8
    EXPECT_EQ(movsx(cx, al), (internal::make_array<std::uint8_t>(0x66, 0x0F, 0xBE, 0xC8)));
}

// MOVSX with extended registers
TEST(MovsxInstructions, ExtendedRegisters) {
    // movsx r8d, cl -> 44 0F BE C1
    EXPECT_EQ(movsx(r8d, cl), (internal::make_array<std::uint8_t>(0x44, 0x0F, 0xBE, 0xC1)));
    // movsx r8, cl -> 4C 0F BE C1
    EXPECT_EQ(movsx(r8, cl), (internal::make_array<std::uint8_t>(0x4C, 0x0F, 0xBE, 0xC1)));
    // movsx eax, r8b -> 41 0F BE C0
    EXPECT_EQ(movsx(eax, r8b), (internal::make_array<std::uint8_t>(0x41, 0x0F, 0xBE, 0xC0)));
}

// =============================================================================
// MOVSXD - Move with Sign Extension (Doubleword to Quadword)
// =============================================================================
// Encoding: REX.W + 63 /r
// Only valid for 64-bit destination with 32-bit source
// =============================================================================

// MOVSXD r64, r32 - sign extend 32-bit to 64-bit
TEST(MovsxdInstructions, R64_R32) {
    // movsxd rax, ebx -> 48 63 C3
    EXPECT_EQ(movsxd(rax, ebx), (internal::make_array<std::uint8_t>(0x48, 0x63, 0xC3)));
    // movsxd rax, ecx -> 48 63 C1
    EXPECT_EQ(movsxd(rax, ecx), (internal::make_array<std::uint8_t>(0x48, 0x63, 0xC1)));
    // movsxd rcx, eax -> 48 63 C8
    EXPECT_EQ(movsxd(rcx, eax), (internal::make_array<std::uint8_t>(0x48, 0x63, 0xC8)));
    // movsxd rdx, ebx -> 48 63 D3
    EXPECT_EQ(movsxd(rdx, ebx), (internal::make_array<std::uint8_t>(0x48, 0x63, 0xD3)));
}

// MOVSXD with extended registers
TEST(MovsxdInstructions, ExtendedRegisters) {
    // movsxd r8, ecx -> 4C 63 C1 (REX.W + REX.R)
    EXPECT_EQ(movsxd(r8, ecx), (internal::make_array<std::uint8_t>(0x4C, 0x63, 0xC1)));
    // movsxd rax, r8d -> 49 63 C0 (REX.W + REX.B)
    EXPECT_EQ(movsxd(rax, r8d), (internal::make_array<std::uint8_t>(0x49, 0x63, 0xC0)));
    // movsxd r8, r9d -> 4D 63 C1 (REX.W + REX.R + REX.B)
    EXPECT_EQ(movsxd(r8, r9d), (internal::make_array<std::uint8_t>(0x4D, 0x63, 0xC1)));
    // movsxd r10, r11d -> 4D 63 D3
    EXPECT_EQ(movsxd(r10, r11d), (internal::make_array<std::uint8_t>(0x4D, 0x63, 0xD3)));
}

// Test constexpr evaluation
TEST(MovzxMovsxInstructions, ConstexprEvaluation) {
    // Verify these can be evaluated at compile time
    constexpr auto movzx_eax_bl = movzx(eax, bl);
    constexpr auto movzx_rax_bx = movzx(rax, bx);
    constexpr auto movsx_eax_bl = movsx(eax, bl);
    constexpr auto movsxd_rax_ebx = movsxd(rax, ebx);

    // Verify the results
    EXPECT_EQ(movzx_eax_bl, (internal::make_array<std::uint8_t>(0x0F, 0xB6, 0xC3)));
    EXPECT_EQ(movzx_rax_bx, (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xB7, 0xC3)));
    EXPECT_EQ(movsx_eax_bl, (internal::make_array<std::uint8_t>(0x0F, 0xBE, 0xC3)));
    EXPECT_EQ(movsxd_rax_ebx, (internal::make_array<std::uint8_t>(0x48, 0x63, 0xC3)));
}

// Test with memory operands (byte_ptr)
TEST(MovzxInstructions, BytePtrMemory) {
    // movzx eax, byte ptr [rbx] -> 0F B6 03
    EXPECT_EQ(movzx(eax, byte_ptr(rbx)), (internal::make_array<std::uint8_t>(0x0F, 0xB6, 0x03)));
    // movzx rax, byte ptr [rcx] -> 48 0F B6 01
    EXPECT_EQ(movzx(rax, byte_ptr(rcx)), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xB6, 0x01)));
}

// Test with memory operands (word_ptr)
TEST(MovzxInstructions, WordPtrMemory) {
    // movzx eax, word ptr [rbx] -> 0F B7 03
    EXPECT_EQ(movzx(eax, word_ptr(rbx)), (internal::make_array<std::uint8_t>(0x0F, 0xB7, 0x03)));
    // movzx rax, word ptr [rcx] -> 48 0F B7 01
    EXPECT_EQ(movzx(rax, word_ptr(rcx)), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xB7, 0x01)));
}

// Test MOVSX with memory operands
TEST(MovsxInstructions, BytePtrMemory) {
    // movsx eax, byte ptr [rbx] -> 0F BE 03
    EXPECT_EQ(movsx(eax, byte_ptr(rbx)), (internal::make_array<std::uint8_t>(0x0F, 0xBE, 0x03)));
    // movsx rax, byte ptr [rcx] -> 48 0F BE 01
    EXPECT_EQ(movsx(rax, byte_ptr(rcx)), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xBE, 0x01)));
}

TEST(MovsxInstructions, WordPtrMemory) {
    // movsx eax, word ptr [rbx] -> 0F BF 03
    EXPECT_EQ(movsx(eax, word_ptr(rbx)), (internal::make_array<std::uint8_t>(0x0F, 0xBF, 0x03)));
    // movsx rax, word ptr [rcx] -> 48 0F BF 01
    EXPECT_EQ(movsx(rax, word_ptr(rcx)), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xBF, 0x01)));
}

// Test MOVSXD with memory operands
TEST(MovsxdInstructions, DwordPtrMemory) {
    // movsxd rax, dword ptr [rbx] -> 48 63 03
    EXPECT_EQ(movsxd(rax, dword_ptr(rbx)), (internal::make_array<std::uint8_t>(0x48, 0x63, 0x03)));
    // movsxd rcx, dword ptr [rax] -> 48 63 08
    EXPECT_EQ(movsxd(rcx, dword_ptr(rax)), (internal::make_array<std::uint8_t>(0x48, 0x63, 0x08)));
}
