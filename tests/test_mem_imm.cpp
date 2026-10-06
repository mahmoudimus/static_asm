#include "static_asm.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <stdexcept>

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

TEST(MemDestImm, SignExtendedQwordBoundaries) {
    EXPECT_EQ(mov(qword_ptr(rbx + std::int8_t(0)), 0x7FFFFFFFLL),
        (internal::make_array<std::uint8_t>(0x48, 0xC7, 0x43, 0x00, 0xFF, 0xFF, 0xFF, 0x7F)));
    EXPECT_EQ(mov(qword_ptr(rbx + std::int8_t(0)), -0x80000000LL),
        (internal::make_array<std::uint8_t>(0x48, 0xC7, 0x43, 0x00, 0x00, 0x00, 0x00, 0x80)));
    EXPECT_EQ(mov(qword_ptr(rbx), 0xFFFFFFFF80000000ULL),
        (internal::make_array<std::uint8_t>(0x48, 0xC7, 0x03, 0x00, 0x00, 0x00, 0x80)));
    EXPECT_EQ(add(qword_ptr(rbx + std::int8_t(0)), -0x80000000LL),
        (internal::make_array<std::uint8_t>(0x48, 0x81, 0x43, 0x00, 0x00, 0x00, 0x00, 0x80)));
    EXPECT_EQ(add(qword_ptr(rbx), -1LL),
        (internal::make_array<std::uint8_t>(0x48, 0x81, 0x03, 0xFF, 0xFF, 0xFF, 0xFF)));
}

TEST(MemDestImm, RejectsUnrepresentableQwordImmediate) {
    const auto positive_sign_bit = 0x80000000ULL;
    const auto too_large = 0x100000000ULL;

    EXPECT_THROW((void)mov(qword_ptr(rbx + std::int8_t(0)), positive_sign_bit), std::out_of_range);
    EXPECT_THROW((void)mov(qword_ptr(rbx), too_large), std::out_of_range);
    EXPECT_THROW((void)mov(qword_ptr(rbx + std::int8_t(0)), imm64(too_large)), std::out_of_range);
    EXPECT_THROW((void)add(qword_ptr(rbx + std::int8_t(0)), too_large), std::out_of_range);
    EXPECT_THROW((void)add(qword_ptr(rbx), too_large), std::out_of_range);
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

TEST(XchgMemory, ExtendedBaseAllWidthsBothOrders) {
    EXPECT_TRUE(std::ranges::equal(xchg(al, byte_ptr(r8)), internal::make_array<std::uint8_t>(0x41, 0x86, 0x00)));
    EXPECT_TRUE(std::ranges::equal(xchg(byte_ptr(r8), al), internal::make_array<std::uint8_t>(0x41, 0x86, 0x00)));
    EXPECT_TRUE(std::ranges::equal(xchg(ax, word_ptr(r8)), internal::make_array<std::uint8_t>(0x66, 0x41, 0x87, 0x00)));
    EXPECT_TRUE(std::ranges::equal(xchg(word_ptr(r8), ax), internal::make_array<std::uint8_t>(0x66, 0x41, 0x87, 0x00)));
    EXPECT_TRUE(std::ranges::equal(xchg(eax, dword_ptr(r8)), internal::make_array<std::uint8_t>(0x41, 0x87, 0x00)));
    EXPECT_TRUE(std::ranges::equal(xchg(dword_ptr(r8), eax), internal::make_array<std::uint8_t>(0x41, 0x87, 0x00)));
    EXPECT_TRUE(std::ranges::equal(xchg(rax, qword_ptr(r8)), internal::make_array<std::uint8_t>(0x49, 0x87, 0x00)));
    EXPECT_TRUE(std::ranges::equal(xchg(qword_ptr(r8), rax), internal::make_array<std::uint8_t>(0x49, 0x87, 0x00)));
}

TEST(ExtendedBaseMemory, RexBWithoutRexWForDword) {
    EXPECT_EQ(mov(al, byte_ptr(r8)), (internal::make_array<std::uint8_t>(0x41, 0x8A, 0x00)));
    EXPECT_EQ(mov(ax, word_ptr(r8)), (internal::make_array<std::uint8_t>(0x66, 0x41, 0x8B, 0x00)));
    EXPECT_EQ(mov(dword_ptr(r8), eax), (internal::make_array<std::uint8_t>(0x41, 0x89, 0x00)));
    EXPECT_EQ(mov(eax, dword_ptr(r8)), (internal::make_array<std::uint8_t>(0x41, 0x8B, 0x00)));
    EXPECT_EQ(mov(rax, qword_ptr(r8)), (internal::make_array<std::uint8_t>(0x49, 0x8B, 0x00)));
    EXPECT_EQ(add(dword_ptr(r8), eax), (internal::make_array<std::uint8_t>(0x41, 0x01, 0x00)));
}

TEST(XchgMemory, SibAndDisp) {
    EXPECT_EQ(xchg(ah, byte_ptr(rbx + std::int8_t(0))), (internal::make_array<std::uint8_t>(0x86, 0x63, 0x00)));
    EXPECT_EQ(xchg(qword_ptr(rcx + std::int8_t(0x10)), rbx), (internal::make_array<std::uint8_t>(0x48, 0x87, 0x59, 0x10)));
    EXPECT_EQ(xchg(rbx, qword_ptr(rcx + std::int8_t(0x10))), (internal::make_array<std::uint8_t>(0x48, 0x87, 0x59, 0x10)));
    EXPECT_EQ(xchg(dword_ptr(rsp + std::int8_t(0x8)), ecx), (internal::make_array<std::uint8_t>(0x87, 0x4C, 0x24, 0x08)));
    EXPECT_EQ(xchg(r8, qword_ptr(rbx + std::int8_t(0x4))), (internal::make_array<std::uint8_t>(0x4C, 0x87, 0x43, 0x04)));
}
