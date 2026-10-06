#include "static_asm.hpp"
#include <gtest/gtest.h>

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// Memory destination + immediate for base/displaced/SIB addressing.
// Byte sequences verified against ndisasm.
TEST(MemDestImm, Mov) {
    EXPECT_EQ(mov(dword_ptr(rbp - std::int8_t(0x20)), 0x100),
        (internal::make_array<std::uint8_t>(0xC7, 0x45, 0xE0, 0x00, 0x01, 0x00, 0x00)));
    EXPECT_EQ(mov(qword_ptr(rcx + std::int8_t(0x10)), 1),
        (internal::make_array<std::uint8_t>(0x48, 0xC7, 0x41, 0x10, 0x01, 0x00, 0x00, 0x00)));
    EXPECT_EQ(mov(byte_ptr(rax + std::int8_t(0x4)), 0x7F),
        (internal::make_array<std::uint8_t>(0xC6, 0x40, 0x04, 0x7F)));
    EXPECT_EQ(mov(word_ptr(rcx + std::int8_t(0x2)), 0x1234),
        (internal::make_array<std::uint8_t>(0x66, 0xC7, 0x41, 0x02, 0x34, 0x12)));
}

TEST(MemDestImm, Alu) {
    EXPECT_EQ(add(qword_ptr(rbx + std::int8_t(0x8)), 0x10),
        (internal::make_array<std::uint8_t>(0x48, 0x81, 0x43, 0x08, 0x10, 0x00, 0x00, 0x00)));
    EXPECT_EQ(sub(dword_ptr(rsp + std::int8_t(0x8)), 1),
        (internal::make_array<std::uint8_t>(0x81, 0x6C, 0x24, 0x08, 0x01, 0x00, 0x00, 0x00)));
    EXPECT_EQ(and_(qword_ptr(r8 + std::int8_t(0x10)), 0xF),
        (internal::make_array<std::uint8_t>(0x49, 0x81, 0x60, 0x10, 0x0F, 0x00, 0x00, 0x00)));
}

// XCHG with a memory operand (symmetric; register always in the reg field).
// Verified against ndisasm.
TEST(XchgMemory, RegisterIndirect) {
    EXPECT_EQ(xchg(qword_ptr(rcx), rax), (internal::make_array<std::uint8_t>(0x48, 0x87, 0x01)));
    EXPECT_EQ(xchg(rax, qword_ptr(rcx)), (internal::make_array<std::uint8_t>(0x48, 0x87, 0x01)));
    EXPECT_EQ(xchg(byte_ptr(rax), cl), (internal::make_array<std::uint8_t>(0x86, 0x08)));
}

TEST(XchgMemory, SibAndDisp) {
    EXPECT_EQ(xchg(qword_ptr(rcx + std::int8_t(0x10)), rbx), (internal::make_array<std::uint8_t>(0x48, 0x87, 0x59, 0x10)));
    EXPECT_EQ(xchg(rbx, qword_ptr(rcx + std::int8_t(0x10))), (internal::make_array<std::uint8_t>(0x48, 0x87, 0x59, 0x10)));
    EXPECT_EQ(xchg(dword_ptr(rsp + std::int8_t(0x8)), ecx), (internal::make_array<std::uint8_t>(0x87, 0x4C, 0x24, 0x08)));
    EXPECT_EQ(xchg(r8, qword_ptr(rbx + std::int8_t(0x4))), (internal::make_array<std::uint8_t>(0x4C, 0x87, 0x43, 0x04)));
}
