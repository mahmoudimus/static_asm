#include "static_asm.hpp"
#include <gtest/gtest.h>

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// RIP-relative addressing: [rip + disp32], ModR/M mod=00 r/m=101, no SIB,
// mandatory disp32. Verified against ndisasm.
TEST(RipRelative, MovLea) {
    EXPECT_EQ(mov(rax, qword_ptr(rip + 0x10)), (internal::make_array<std::uint8_t>(0x48, 0x8B, 0x05, 0x10, 0x00, 0x00, 0x00)));
    EXPECT_EQ(mov(qword_ptr(rip + 0x10), rax), (internal::make_array<std::uint8_t>(0x48, 0x89, 0x05, 0x10, 0x00, 0x00, 0x00)));
    EXPECT_EQ(lea(rax, qword_ptr(rip + 0x100)), (internal::make_array<std::uint8_t>(0x48, 0x8D, 0x05, 0x00, 0x01, 0x00, 0x00)));
    EXPECT_EQ(lea(rcx, qword_ptr(rip - 0x8)), (internal::make_array<std::uint8_t>(0x48, 0x8D, 0x0D, 0xF8, 0xFF, 0xFF, 0xFF)));
    EXPECT_EQ(mov(eax, dword_ptr(rip + 0x10)), (internal::make_array<std::uint8_t>(0x8B, 0x05, 0x10, 0x00, 0x00, 0x00)));
}

TEST(RipRelative, AluUnaryMulImm) {
    EXPECT_EQ(add(rax, qword_ptr(rip + 0x20)), (internal::make_array<std::uint8_t>(0x48, 0x03, 0x05, 0x20, 0x00, 0x00, 0x00)));
    EXPECT_EQ(add(qword_ptr(rip + 0x20), rbx), (internal::make_array<std::uint8_t>(0x48, 0x01, 0x1D, 0x20, 0x00, 0x00, 0x00)));
    EXPECT_EQ(inc(qword_ptr(rip + 0x40)), (internal::make_array<std::uint8_t>(0x48, 0xFF, 0x05, 0x40, 0x00, 0x00, 0x00)));
    EXPECT_EQ(mul(qword_ptr(rip + 0x8)), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0x25, 0x08, 0x00, 0x00, 0x00)));
    EXPECT_EQ(mov(dword_ptr(rip + 0x10), 0x7B),
        (internal::make_array<std::uint8_t>(0xC7, 0x05, 0x10, 0x00, 0x00, 0x00, 0x7B, 0x00, 0x00, 0x00)));
}
