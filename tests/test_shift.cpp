#include "static_asm.hpp"
#include <gtest/gtest.h>

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// ============================================================================
// SHL - Shift Left (Logical)
// ============================================================================

TEST(ShlInstructions, ShiftByOne) {
    // SHL r/m8, 1 -> D0 /4
    EXPECT_EQ(shl(al), (internal::make_array<std::uint8_t>(0xD0, 0xE0)));
    EXPECT_EQ(shl(cl), (internal::make_array<std::uint8_t>(0xD0, 0xE1)));
    EXPECT_EQ(shl(bl), (internal::make_array<std::uint8_t>(0xD0, 0xE3)));

    // SHL r/m16, 1 -> D1 /4 (with 66 prefix)
    // Note: 16-bit not currently handled by encode_shift_by_one

    // SHL r/m32, 1 -> D1 /4
    EXPECT_EQ(shl(eax), (internal::make_array<std::uint8_t>(0xD1, 0xE0)));
    EXPECT_EQ(shl(ecx), (internal::make_array<std::uint8_t>(0xD1, 0xE1)));
    EXPECT_EQ(shl(ebx), (internal::make_array<std::uint8_t>(0xD1, 0xE3)));
    EXPECT_EQ(shl(r8d), (internal::make_array<std::uint8_t>(0x41, 0xD1, 0xE0)));
    EXPECT_EQ(shl(r9d), (internal::make_array<std::uint8_t>(0x41, 0xD1, 0xE1)));

    // SHL r/m64, 1 -> REX.W D1 /4
    EXPECT_EQ(shl(rax), (internal::make_array<std::uint8_t>(0x48, 0xD1, 0xE0)));
    EXPECT_EQ(shl(rcx), (internal::make_array<std::uint8_t>(0x48, 0xD1, 0xE1)));
    EXPECT_EQ(shl(rbx), (internal::make_array<std::uint8_t>(0x48, 0xD1, 0xE3)));
    EXPECT_EQ(shl(r8), (internal::make_array<std::uint8_t>(0x49, 0xD1, 0xE0)));
    EXPECT_EQ(shl(r9), (internal::make_array<std::uint8_t>(0x49, 0xD1, 0xE1)));
}

TEST(ShlInstructions, ShiftByCL) {
    // SHL r/m8, CL -> D2 /4
    EXPECT_EQ(shl(al, cl), (internal::make_array<std::uint8_t>(0xD2, 0xE0)));
    EXPECT_EQ(shl(bl, cl), (internal::make_array<std::uint8_t>(0xD2, 0xE3)));

    // SHL r/m32, CL -> D3 /4
    EXPECT_EQ(shl(eax, cl), (internal::make_array<std::uint8_t>(0xD3, 0xE0)));
    EXPECT_EQ(shl(ecx, cl), (internal::make_array<std::uint8_t>(0xD3, 0xE1)));
    EXPECT_EQ(shl(ebx, cl), (internal::make_array<std::uint8_t>(0xD3, 0xE3)));
    EXPECT_EQ(shl(r8d, cl), (internal::make_array<std::uint8_t>(0x41, 0xD3, 0xE0)));
    EXPECT_EQ(shl(r9d, cl), (internal::make_array<std::uint8_t>(0x41, 0xD3, 0xE1)));

    // SHL r/m64, CL -> REX.W D3 /4
    EXPECT_EQ(shl(rax, cl), (internal::make_array<std::uint8_t>(0x48, 0xD3, 0xE0)));
    EXPECT_EQ(shl(rcx, cl), (internal::make_array<std::uint8_t>(0x48, 0xD3, 0xE1)));
    EXPECT_EQ(shl(rbx, cl), (internal::make_array<std::uint8_t>(0x48, 0xD3, 0xE3)));
    EXPECT_EQ(shl(r8, cl), (internal::make_array<std::uint8_t>(0x49, 0xD3, 0xE0)));
    EXPECT_EQ(shl(r9, cl), (internal::make_array<std::uint8_t>(0x49, 0xD3, 0xE1)));
}

TEST(ShlInstructions, ShiftByImm8) {
    // SHL r/m8, imm8 -> C0 /4 ib
    EXPECT_EQ(shl(al, 4), (internal::make_array<std::uint8_t>(0xC0, 0xE0, 0x04)));
    EXPECT_EQ(shl(bl, 7), (internal::make_array<std::uint8_t>(0xC0, 0xE3, 0x07)));

    // SHL r/m32, imm8 -> C1 /4 ib
    EXPECT_EQ(shl(eax, 4), (internal::make_array<std::uint8_t>(0xC1, 0xE0, 0x04)));
    EXPECT_EQ(shl(ecx, 8), (internal::make_array<std::uint8_t>(0xC1, 0xE1, 0x08)));
    EXPECT_EQ(shl(r8d, 5), (internal::make_array<std::uint8_t>(0x41, 0xC1, 0xE0, 0x05)));

    // SHL r/m64, imm8 -> REX.W C1 /4 ib
    EXPECT_EQ(shl(rax, 4), (internal::make_array<std::uint8_t>(0x48, 0xC1, 0xE0, 0x04)));
    EXPECT_EQ(shl(rcx, 16), (internal::make_array<std::uint8_t>(0x48, 0xC1, 0xE1, 0x10)));
    EXPECT_EQ(shl(r8, 32), (internal::make_array<std::uint8_t>(0x49, 0xC1, 0xE0, 0x20)));
}

// ============================================================================
// SHR - Shift Right (Logical)
// ============================================================================

TEST(ShrInstructions, ShiftByOne) {
    // SHR r/m8, 1 -> D0 /5
    EXPECT_EQ(shr(al), (internal::make_array<std::uint8_t>(0xD0, 0xE8)));
    EXPECT_EQ(shr(cl), (internal::make_array<std::uint8_t>(0xD0, 0xE9)));

    // SHR r/m32, 1 -> D1 /5
    EXPECT_EQ(shr(eax), (internal::make_array<std::uint8_t>(0xD1, 0xE8)));
    EXPECT_EQ(shr(ecx), (internal::make_array<std::uint8_t>(0xD1, 0xE9)));
    EXPECT_EQ(shr(r8d), (internal::make_array<std::uint8_t>(0x41, 0xD1, 0xE8)));

    // SHR r/m64, 1 -> REX.W D1 /5
    EXPECT_EQ(shr(rax), (internal::make_array<std::uint8_t>(0x48, 0xD1, 0xE8)));
    EXPECT_EQ(shr(rcx), (internal::make_array<std::uint8_t>(0x48, 0xD1, 0xE9)));
    EXPECT_EQ(shr(r8), (internal::make_array<std::uint8_t>(0x49, 0xD1, 0xE8)));
}

TEST(ShrInstructions, ShiftByCL) {
    // SHR r/m32, CL -> D3 /5
    EXPECT_EQ(shr(eax, cl), (internal::make_array<std::uint8_t>(0xD3, 0xE8)));
    EXPECT_EQ(shr(r8d, cl), (internal::make_array<std::uint8_t>(0x41, 0xD3, 0xE8)));

    // SHR r/m64, CL -> REX.W D3 /5
    EXPECT_EQ(shr(rax, cl), (internal::make_array<std::uint8_t>(0x48, 0xD3, 0xE8)));
    EXPECT_EQ(shr(r8, cl), (internal::make_array<std::uint8_t>(0x49, 0xD3, 0xE8)));
}

TEST(ShrInstructions, ShiftByImm8) {
    // SHR r/m32, imm8 -> C1 /5 ib
    EXPECT_EQ(shr(eax, 4), (internal::make_array<std::uint8_t>(0xC1, 0xE8, 0x04)));
    EXPECT_EQ(shr(r8d, 5), (internal::make_array<std::uint8_t>(0x41, 0xC1, 0xE8, 0x05)));

    // SHR r/m64, imm8 -> REX.W C1 /5 ib
    EXPECT_EQ(shr(rax, 8), (internal::make_array<std::uint8_t>(0x48, 0xC1, 0xE8, 0x08)));
    EXPECT_EQ(shr(r8, 16), (internal::make_array<std::uint8_t>(0x49, 0xC1, 0xE8, 0x10)));
}

// ============================================================================
// SAL - Shift Arithmetic Left (same encoding as SHL)
// ============================================================================

TEST(SalInstructions, ShiftByOne) {
    // SAL has the same encoding as SHL (opcode extension 4)
    EXPECT_EQ(sal(eax), (internal::make_array<std::uint8_t>(0xD1, 0xE0)));
    EXPECT_EQ(sal(rax), (internal::make_array<std::uint8_t>(0x48, 0xD1, 0xE0)));
}

TEST(SalInstructions, ShiftByCL) {
    EXPECT_EQ(sal(eax, cl), (internal::make_array<std::uint8_t>(0xD3, 0xE0)));
    EXPECT_EQ(sal(rax, cl), (internal::make_array<std::uint8_t>(0x48, 0xD3, 0xE0)));
}

TEST(SalInstructions, ShiftByImm8) {
    EXPECT_EQ(sal(eax, 4), (internal::make_array<std::uint8_t>(0xC1, 0xE0, 0x04)));
    EXPECT_EQ(sal(rax, 4), (internal::make_array<std::uint8_t>(0x48, 0xC1, 0xE0, 0x04)));
}

// ============================================================================
// SAR - Shift Arithmetic Right
// ============================================================================

TEST(SarInstructions, ShiftByOne) {
    // SAR r/m8, 1 -> D0 /7
    EXPECT_EQ(sar(al), (internal::make_array<std::uint8_t>(0xD0, 0xF8)));
    EXPECT_EQ(sar(cl), (internal::make_array<std::uint8_t>(0xD0, 0xF9)));

    // SAR r/m32, 1 -> D1 /7
    EXPECT_EQ(sar(eax), (internal::make_array<std::uint8_t>(0xD1, 0xF8)));
    EXPECT_EQ(sar(ecx), (internal::make_array<std::uint8_t>(0xD1, 0xF9)));
    EXPECT_EQ(sar(r8d), (internal::make_array<std::uint8_t>(0x41, 0xD1, 0xF8)));

    // SAR r/m64, 1 -> REX.W D1 /7
    EXPECT_EQ(sar(rax), (internal::make_array<std::uint8_t>(0x48, 0xD1, 0xF8)));
    EXPECT_EQ(sar(r8), (internal::make_array<std::uint8_t>(0x49, 0xD1, 0xF8)));
}

TEST(SarInstructions, ShiftByCL) {
    // SAR r/m32, CL -> D3 /7
    EXPECT_EQ(sar(eax, cl), (internal::make_array<std::uint8_t>(0xD3, 0xF8)));
    EXPECT_EQ(sar(r8d, cl), (internal::make_array<std::uint8_t>(0x41, 0xD3, 0xF8)));

    // SAR r/m64, CL -> REX.W D3 /7
    EXPECT_EQ(sar(rax, cl), (internal::make_array<std::uint8_t>(0x48, 0xD3, 0xF8)));
    EXPECT_EQ(sar(r8, cl), (internal::make_array<std::uint8_t>(0x49, 0xD3, 0xF8)));
}

TEST(SarInstructions, ShiftByImm8) {
    // SAR r/m32, imm8 -> C1 /7 ib
    EXPECT_EQ(sar(eax, 4), (internal::make_array<std::uint8_t>(0xC1, 0xF8, 0x04)));
    EXPECT_EQ(sar(r8d, 5), (internal::make_array<std::uint8_t>(0x41, 0xC1, 0xF8, 0x05)));

    // SAR r/m64, imm8 -> REX.W C1 /7 ib
    EXPECT_EQ(sar(rax, 8), (internal::make_array<std::uint8_t>(0x48, 0xC1, 0xF8, 0x08)));
    EXPECT_EQ(sar(r8, 16), (internal::make_array<std::uint8_t>(0x49, 0xC1, 0xF8, 0x10)));
}

// ============================================================================
// ROL - Rotate Left
// ============================================================================

TEST(RolInstructions, RotateByOne) {
    // ROL r/m8, 1 -> D0 /0
    EXPECT_EQ(rol(al), (internal::make_array<std::uint8_t>(0xD0, 0xC0)));
    EXPECT_EQ(rol(cl), (internal::make_array<std::uint8_t>(0xD0, 0xC1)));

    // ROL r/m32, 1 -> D1 /0
    EXPECT_EQ(rol(eax), (internal::make_array<std::uint8_t>(0xD1, 0xC0)));
    EXPECT_EQ(rol(r8d), (internal::make_array<std::uint8_t>(0x41, 0xD1, 0xC0)));

    // ROL r/m64, 1 -> REX.W D1 /0
    EXPECT_EQ(rol(rax), (internal::make_array<std::uint8_t>(0x48, 0xD1, 0xC0)));
    EXPECT_EQ(rol(r8), (internal::make_array<std::uint8_t>(0x49, 0xD1, 0xC0)));
}

TEST(RolInstructions, RotateByCL) {
    // ROL r/m32, CL -> D3 /0
    EXPECT_EQ(rol(eax, cl), (internal::make_array<std::uint8_t>(0xD3, 0xC0)));
    EXPECT_EQ(rol(r8d, cl), (internal::make_array<std::uint8_t>(0x41, 0xD3, 0xC0)));

    // ROL r/m64, CL -> REX.W D3 /0
    EXPECT_EQ(rol(rax, cl), (internal::make_array<std::uint8_t>(0x48, 0xD3, 0xC0)));
    EXPECT_EQ(rol(r8, cl), (internal::make_array<std::uint8_t>(0x49, 0xD3, 0xC0)));
}

TEST(RolInstructions, RotateByImm8) {
    // ROL r/m32, imm8 -> C1 /0 ib
    EXPECT_EQ(rol(eax, 4), (internal::make_array<std::uint8_t>(0xC1, 0xC0, 0x04)));
    EXPECT_EQ(rol(r8d, 5), (internal::make_array<std::uint8_t>(0x41, 0xC1, 0xC0, 0x05)));

    // ROL r/m64, imm8 -> REX.W C1 /0 ib
    EXPECT_EQ(rol(rax, 8), (internal::make_array<std::uint8_t>(0x48, 0xC1, 0xC0, 0x08)));
    EXPECT_EQ(rol(r8, 16), (internal::make_array<std::uint8_t>(0x49, 0xC1, 0xC0, 0x10)));
}

// ============================================================================
// ROR - Rotate Right
// ============================================================================

TEST(RorInstructions, RotateByOne) {
    // ROR r/m8, 1 -> D0 /1
    EXPECT_EQ(ror(al), (internal::make_array<std::uint8_t>(0xD0, 0xC8)));
    EXPECT_EQ(ror(cl), (internal::make_array<std::uint8_t>(0xD0, 0xC9)));

    // ROR r/m32, 1 -> D1 /1
    EXPECT_EQ(ror(eax), (internal::make_array<std::uint8_t>(0xD1, 0xC8)));
    EXPECT_EQ(ror(r8d), (internal::make_array<std::uint8_t>(0x41, 0xD1, 0xC8)));

    // ROR r/m64, 1 -> REX.W D1 /1
    EXPECT_EQ(ror(rax), (internal::make_array<std::uint8_t>(0x48, 0xD1, 0xC8)));
    EXPECT_EQ(ror(r8), (internal::make_array<std::uint8_t>(0x49, 0xD1, 0xC8)));
}

TEST(RorInstructions, RotateByCL) {
    // ROR r/m32, CL -> D3 /1
    EXPECT_EQ(ror(eax, cl), (internal::make_array<std::uint8_t>(0xD3, 0xC8)));
    EXPECT_EQ(ror(r8d, cl), (internal::make_array<std::uint8_t>(0x41, 0xD3, 0xC8)));

    // ROR r/m64, CL -> REX.W D3 /1
    EXPECT_EQ(ror(rax, cl), (internal::make_array<std::uint8_t>(0x48, 0xD3, 0xC8)));
    EXPECT_EQ(ror(r8, cl), (internal::make_array<std::uint8_t>(0x49, 0xD3, 0xC8)));
}

TEST(RorInstructions, RotateByImm8) {
    // ROR r/m32, imm8 -> C1 /1 ib
    EXPECT_EQ(ror(eax, 4), (internal::make_array<std::uint8_t>(0xC1, 0xC8, 0x04)));
    EXPECT_EQ(ror(r8d, 5), (internal::make_array<std::uint8_t>(0x41, 0xC1, 0xC8, 0x05)));

    // ROR r/m64, imm8 -> REX.W C1 /1 ib
    EXPECT_EQ(ror(rax, 8), (internal::make_array<std::uint8_t>(0x48, 0xC1, 0xC8, 0x08)));
    EXPECT_EQ(ror(r8, 16), (internal::make_array<std::uint8_t>(0x49, 0xC1, 0xC8, 0x10)));
}

// ============================================================================
// RCL - Rotate through Carry Left
// ============================================================================

TEST(RclInstructions, RotateByOne) {
    // RCL r/m8, 1 -> D0 /2
    EXPECT_EQ(rcl(al), (internal::make_array<std::uint8_t>(0xD0, 0xD0)));
    EXPECT_EQ(rcl(cl), (internal::make_array<std::uint8_t>(0xD0, 0xD1)));

    // RCL r/m32, 1 -> D1 /2
    EXPECT_EQ(rcl(eax), (internal::make_array<std::uint8_t>(0xD1, 0xD0)));
    EXPECT_EQ(rcl(r8d), (internal::make_array<std::uint8_t>(0x41, 0xD1, 0xD0)));

    // RCL r/m64, 1 -> REX.W D1 /2
    EXPECT_EQ(rcl(rax), (internal::make_array<std::uint8_t>(0x48, 0xD1, 0xD0)));
    EXPECT_EQ(rcl(r8), (internal::make_array<std::uint8_t>(0x49, 0xD1, 0xD0)));
}

TEST(RclInstructions, RotateByCL) {
    // RCL r/m32, CL -> D3 /2
    EXPECT_EQ(rcl(eax, cl), (internal::make_array<std::uint8_t>(0xD3, 0xD0)));
    EXPECT_EQ(rcl(r8d, cl), (internal::make_array<std::uint8_t>(0x41, 0xD3, 0xD0)));

    // RCL r/m64, CL -> REX.W D3 /2
    EXPECT_EQ(rcl(rax, cl), (internal::make_array<std::uint8_t>(0x48, 0xD3, 0xD0)));
    EXPECT_EQ(rcl(r8, cl), (internal::make_array<std::uint8_t>(0x49, 0xD3, 0xD0)));
}

TEST(RclInstructions, RotateByImm8) {
    // RCL r/m32, imm8 -> C1 /2 ib
    EXPECT_EQ(rcl(eax, 4), (internal::make_array<std::uint8_t>(0xC1, 0xD0, 0x04)));
    EXPECT_EQ(rcl(r8d, 5), (internal::make_array<std::uint8_t>(0x41, 0xC1, 0xD0, 0x05)));

    // RCL r/m64, imm8 -> REX.W C1 /2 ib
    EXPECT_EQ(rcl(rax, 8), (internal::make_array<std::uint8_t>(0x48, 0xC1, 0xD0, 0x08)));
    EXPECT_EQ(rcl(r8, 16), (internal::make_array<std::uint8_t>(0x49, 0xC1, 0xD0, 0x10)));
}

// ============================================================================
// RCR - Rotate through Carry Right
// ============================================================================

TEST(RcrInstructions, RotateByOne) {
    // RCR r/m8, 1 -> D0 /3
    EXPECT_EQ(rcr(al), (internal::make_array<std::uint8_t>(0xD0, 0xD8)));
    EXPECT_EQ(rcr(cl), (internal::make_array<std::uint8_t>(0xD0, 0xD9)));

    // RCR r/m32, 1 -> D1 /3
    EXPECT_EQ(rcr(eax), (internal::make_array<std::uint8_t>(0xD1, 0xD8)));
    EXPECT_EQ(rcr(r8d), (internal::make_array<std::uint8_t>(0x41, 0xD1, 0xD8)));

    // RCR r/m64, 1 -> REX.W D1 /3
    EXPECT_EQ(rcr(rax), (internal::make_array<std::uint8_t>(0x48, 0xD1, 0xD8)));
    EXPECT_EQ(rcr(r8), (internal::make_array<std::uint8_t>(0x49, 0xD1, 0xD8)));
}

TEST(RcrInstructions, RotateByCL) {
    // RCR r/m32, CL -> D3 /3
    EXPECT_EQ(rcr(eax, cl), (internal::make_array<std::uint8_t>(0xD3, 0xD8)));
    EXPECT_EQ(rcr(r8d, cl), (internal::make_array<std::uint8_t>(0x41, 0xD3, 0xD8)));

    // RCR r/m64, CL -> REX.W D3 /3
    EXPECT_EQ(rcr(rax, cl), (internal::make_array<std::uint8_t>(0x48, 0xD3, 0xD8)));
    EXPECT_EQ(rcr(r8, cl), (internal::make_array<std::uint8_t>(0x49, 0xD3, 0xD8)));
}

TEST(RcrInstructions, RotateByImm8) {
    // RCR r/m32, imm8 -> C1 /3 ib
    EXPECT_EQ(rcr(eax, 4), (internal::make_array<std::uint8_t>(0xC1, 0xD8, 0x04)));
    EXPECT_EQ(rcr(r8d, 5), (internal::make_array<std::uint8_t>(0x41, 0xC1, 0xD8, 0x05)));

    // RCR r/m64, imm8 -> REX.W C1 /3 ib
    EXPECT_EQ(rcr(rax, 8), (internal::make_array<std::uint8_t>(0x48, 0xC1, 0xD8, 0x08)));
    EXPECT_EQ(rcr(r8, 16), (internal::make_array<std::uint8_t>(0x49, 0xC1, 0xD8, 0x10)));
}
