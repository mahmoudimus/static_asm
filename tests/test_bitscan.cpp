#include <gtest/gtest.h>
#include "static_asm.hpp"
// Using internal::make_array from static_asm

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// ============================================================================
// BSF (Bit Scan Forward) Tests
// Encoding: 0F BC /r
// ============================================================================

TEST(BsfInstructions, Register32ToRegister32) {
    // BSF EAX, EBX -> 0F BC C3
    // ModR/M: C3 = 11 000 011 (mod=11, reg=eax(0), r/m=ebx(3))
    EXPECT_EQ(bsf(eax, ebx), (internal::make_array<std::uint8_t>(0x0F, 0xBC, 0xC3)));

    // BSF ECX, EDX -> 0F BC CA
    // ModR/M: CA = 11 001 010 (mod=11, reg=ecx(1), r/m=edx(2))
    EXPECT_EQ(bsf(ecx, edx), (internal::make_array<std::uint8_t>(0x0F, 0xBC, 0xCA)));
}

TEST(BsfInstructions, Register64ToRegister64) {
    // BSF RAX, RBX -> 48 0F BC C3
    // REX.W (48) for 64-bit operands
    EXPECT_EQ(bsf(rax, rbx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xBC, 0xC3)));

    // BSF RCX, RDX -> 48 0F BC CA
    EXPECT_EQ(bsf(rcx, rdx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xBC, 0xCA)));
}

TEST(BsfInstructions, ExtendedRegisters) {
    // BSF R8D, R9D -> 45 0F BC C1
    // REX: 45 = 0100 0101 (REX.R=1 for R8, REX.B=1 for R9)
    EXPECT_EQ(bsf(r8d, r9d), (internal::make_array<std::uint8_t>(0x45, 0x0F, 0xBC, 0xC1)));

    // BSF R8, R9 -> 4D 0F BC C1
    // REX: 4D = 0100 1101 (REX.W=1, REX.R=1, REX.B=1)
    EXPECT_EQ(bsf(r8, r9), (internal::make_array<std::uint8_t>(0x4D, 0x0F, 0xBC, 0xC1)));

    // BSF EAX, R8D -> 41 0F BC C0
    // REX: 41 = 0100 0001 (REX.B=1 for R8)
    EXPECT_EQ(bsf(eax, r8d), (internal::make_array<std::uint8_t>(0x41, 0x0F, 0xBC, 0xC0)));

    // BSF R8D, EAX -> 44 0F BC C0
    // REX: 44 = 0100 0100 (REX.R=1 for R8)
    EXPECT_EQ(bsf(r8d, eax), (internal::make_array<std::uint8_t>(0x44, 0x0F, 0xBC, 0xC0)));
}

// ============================================================================
// BSR (Bit Scan Reverse) Tests
// Encoding: 0F BD /r
// ============================================================================

TEST(BsrInstructions, Register32ToRegister32) {
    // BSR EAX, EBX -> 0F BD C3
    EXPECT_EQ(bsr(eax, ebx), (internal::make_array<std::uint8_t>(0x0F, 0xBD, 0xC3)));

    // BSR ECX, EDX -> 0F BD CA
    EXPECT_EQ(bsr(ecx, edx), (internal::make_array<std::uint8_t>(0x0F, 0xBD, 0xCA)));
}

TEST(BsrInstructions, Register64ToRegister64) {
    // BSR RAX, RBX -> 48 0F BD C3
    EXPECT_EQ(bsr(rax, rbx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xBD, 0xC3)));

    // BSR RCX, RDX -> 48 0F BD CA
    EXPECT_EQ(bsr(rcx, rdx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xBD, 0xCA)));
}

TEST(BsrInstructions, ExtendedRegisters) {
    // BSR R8D, R9D -> 45 0F BD C1
    EXPECT_EQ(bsr(r8d, r9d), (internal::make_array<std::uint8_t>(0x45, 0x0F, 0xBD, 0xC1)));

    // BSR R8, R9 -> 4D 0F BD C1
    EXPECT_EQ(bsr(r8, r9), (internal::make_array<std::uint8_t>(0x4D, 0x0F, 0xBD, 0xC1)));
}

// ============================================================================
// BSWAP (Byte Swap) Tests
// Encoding: 0F C8+rd (32-bit), REX.W + 0F C8+rd (64-bit)
// ============================================================================

TEST(BswapInstructions, Register32) {
    // BSWAP EAX -> 0F C8
    EXPECT_EQ(bswap(eax), (internal::make_array<std::uint8_t>(0x0F, 0xC8)));

    // BSWAP EBX -> 0F CB
    EXPECT_EQ(bswap(ebx), (internal::make_array<std::uint8_t>(0x0F, 0xCB)));

    // BSWAP ECX -> 0F C9
    EXPECT_EQ(bswap(ecx), (internal::make_array<std::uint8_t>(0x0F, 0xC9)));

    // BSWAP EDX -> 0F CA
    EXPECT_EQ(bswap(edx), (internal::make_array<std::uint8_t>(0x0F, 0xCA)));

    // BSWAP ESI -> 0F CE
    EXPECT_EQ(bswap(esi), (internal::make_array<std::uint8_t>(0x0F, 0xCE)));

    // BSWAP EDI -> 0F CF
    EXPECT_EQ(bswap(edi), (internal::make_array<std::uint8_t>(0x0F, 0xCF)));

    // BSWAP ESP -> 0F CC
    EXPECT_EQ(bswap(esp), (internal::make_array<std::uint8_t>(0x0F, 0xCC)));

    // BSWAP EBP -> 0F CD
    EXPECT_EQ(bswap(ebp), (internal::make_array<std::uint8_t>(0x0F, 0xCD)));
}

TEST(BswapInstructions, Register64) {
    // BSWAP RAX -> 48 0F C8
    EXPECT_EQ(bswap(rax), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xC8)));

    // BSWAP RBX -> 48 0F CB
    EXPECT_EQ(bswap(rbx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xCB)));

    // BSWAP RCX -> 48 0F C9
    EXPECT_EQ(bswap(rcx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xC9)));

    // BSWAP RDX -> 48 0F CA
    EXPECT_EQ(bswap(rdx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xCA)));

    // BSWAP RSI -> 48 0F CE
    EXPECT_EQ(bswap(rsi), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xCE)));

    // BSWAP RDI -> 48 0F CF
    EXPECT_EQ(bswap(rdi), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xCF)));

    // BSWAP RSP -> 48 0F CC
    EXPECT_EQ(bswap(rsp), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xCC)));

    // BSWAP RBP -> 48 0F CD
    EXPECT_EQ(bswap(rbp), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xCD)));
}

TEST(BswapInstructions, ExtendedRegisters32) {
    // BSWAP R8D -> 41 0F C8
    // REX: 41 = 0100 0001 (REX.B=1 for R8)
    EXPECT_EQ(bswap(r8d), (internal::make_array<std::uint8_t>(0x41, 0x0F, 0xC8)));

    // BSWAP R9D -> 41 0F C9
    EXPECT_EQ(bswap(r9d), (internal::make_array<std::uint8_t>(0x41, 0x0F, 0xC9)));

    // BSWAP R15D -> 41 0F CF
    EXPECT_EQ(bswap(r15d), (internal::make_array<std::uint8_t>(0x41, 0x0F, 0xCF)));
}

TEST(BswapInstructions, ExtendedRegisters64) {
    // BSWAP R8 -> 49 0F C8
    // REX: 49 = 0100 1001 (REX.W=1, REX.B=1)
    EXPECT_EQ(bswap(r8), (internal::make_array<std::uint8_t>(0x49, 0x0F, 0xC8)));

    // BSWAP R9 -> 49 0F C9
    EXPECT_EQ(bswap(r9), (internal::make_array<std::uint8_t>(0x49, 0x0F, 0xC9)));

    // BSWAP R15 -> 49 0F CF
    EXPECT_EQ(bswap(r15), (internal::make_array<std::uint8_t>(0x49, 0x0F, 0xCF)));
}

// Verify constexpr evaluation
TEST(BswapInstructions, ConstexprEvaluation) {
    // Verify that bswap is evaluated at compile time
    constexpr auto bswap_eax = bswap(eax);
    constexpr auto bswap_rax = bswap(rax);
    constexpr auto bswap_r8d = bswap(r8d);
    constexpr auto bswap_r8 = bswap(r8);

    // Check sizes
    static_assert(bswap_eax.size() == 2, "BSWAP EAX should be 2 bytes");
    static_assert(bswap_rax.size() == 3, "BSWAP RAX should be 3 bytes");
    static_assert(bswap_r8d.size() == 3, "BSWAP R8D should be 3 bytes (with REX.B)");
    static_assert(bswap_r8.size() == 3, "BSWAP R8 should be 3 bytes (with REX.W+B)");

    // Verify contents at compile time
    static_assert(bswap_eax[0] == 0x0F, "BSWAP EAX: first byte should be 0x0F");
    static_assert(bswap_eax[1] == 0xC8, "BSWAP EAX: second byte should be 0xC8");

    static_assert(bswap_rax[0] == 0x48, "BSWAP RAX: REX.W prefix should be 0x48");
    static_assert(bswap_rax[1] == 0x0F, "BSWAP RAX: second byte should be 0x0F");
    static_assert(bswap_rax[2] == 0xC8, "BSWAP RAX: third byte should be 0xC8");

    EXPECT_TRUE(true);  // Test passes if compilation succeeds
}
