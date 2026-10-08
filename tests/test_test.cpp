#include "static_asm.hpp"
#include <gtest/gtest.h>

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// TEST reg, reg
TEST(TestInstructions, RegisterToRegister64) {
    // test rax, rbx: 48 85 D8
    EXPECT_EQ(test(rax, rbx), (internal::make_array<std::uint8_t>(0x48, 0x85, 0xD8)));
    // test rcx, rdx: 48 85 D1
    EXPECT_EQ(test(rcx, rdx), (internal::make_array<std::uint8_t>(0x48, 0x85, 0xD1)));
    // test r8, r9: 4D 85 C8
    EXPECT_EQ(test(r8, r9), (internal::make_array<std::uint8_t>(0x4D, 0x85, 0xC8)));
}

TEST(TestInstructions, RegisterToRegister32) {
    // test eax, ebx: 85 D8
    EXPECT_EQ(test(eax, ebx), (internal::make_array<std::uint8_t>(0x85, 0xD8)));
    // test ecx, edx: 85 D1
    EXPECT_EQ(test(ecx, edx), (internal::make_array<std::uint8_t>(0x85, 0xD1)));
}

TEST(TestInstructions, RegisterToRegister8) {
    // test al, bl: 84 D8
    EXPECT_EQ(test(al, bl), (internal::make_array<std::uint8_t>(0x84, 0xD8)));
    // test cl, dl: 84 D1
    EXPECT_EQ(test(cl, dl), (internal::make_array<std::uint8_t>(0x84, 0xD1)));
}

// TEST reg, imm
TEST(TestInstructions, RegisterImmediate32) {
    // test eax, 0x12345678: F7 C0 78 56 34 12
    EXPECT_EQ(test(eax, 0x12345678), (internal::make_array<std::uint8_t>(0xF7, 0xC0, 0x78, 0x56, 0x34, 0x12)));
    // test ecx, 1: F7 C1 01 00 00 00
    EXPECT_EQ(test(ecx, 1), (internal::make_array<std::uint8_t>(0xF7, 0xC1, 0x01, 0x00, 0x00, 0x00)));
}

TEST(TestInstructions, RegisterImmediate64) {
    // test rax, 0x12345678: 48 F7 C0 78 56 34 12
    EXPECT_EQ(test(rax, 0x12345678), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0xC0, 0x78, 0x56, 0x34, 0x12)));
}

TEST(TestInstructions, RipRelativeRegisterAndMemory) {
    constexpr auto reg_first = test(rax, qword_ptr(rip + 0x10));
    constexpr auto mem_first = test(qword_ptr(rip + 0x10), rax);
    constexpr auto expected = internal::make_array<std::uint8_t>(0x48, 0x85, 0x05, 0x10, 0x00, 0x00, 0x00);
    static_assert(reg_first == expected);
    static_assert(mem_first == expected);
}

TEST(TestInstructions, RipRelativeMemoryWidths) {
    static_assert(test(al, byte_ptr(rip + 0)) ==
                  internal::make_array<std::uint8_t>(0x84, 0x05, 0x00, 0x00, 0x00, 0x00));
    static_assert(test(ax, word_ptr(rip + 0)) ==
                  internal::make_array<std::uint8_t>(0x66, 0x85, 0x05, 0x00, 0x00, 0x00, 0x00));
    static_assert(test(eax, dword_ptr(rip + 0)) ==
                  internal::make_array<std::uint8_t>(0x85, 0x05, 0x00, 0x00, 0x00, 0x00));
    static_assert(test(r9, qword_ptr(rip + 0)) ==
                  internal::make_array<std::uint8_t>(0x4C, 0x85, 0x0D, 0x00, 0x00, 0x00, 0x00));
    static_assert(test(spl, byte_ptr(rip + 0)) ==
                  internal::make_array<std::uint8_t>(0x40, 0x84, 0x25, 0x00, 0x00, 0x00, 0x00));
    static_assert(test(ah, byte_ptr(rip + 0)) ==
                  internal::make_array<std::uint8_t>(0x84, 0x25, 0x00, 0x00, 0x00, 0x00));
}

TEST(TestInstructions, IndexedAndPlainMemory) {
    static_assert(test(r10d, dword_ptr(r8 + r9 * s4 + disp<0x20>)) ==
                  internal::make_array<std::uint8_t>(0x47, 0x85, 0x54, 0x88, 0x20));
    static_assert(test(eax, dword_ptr(rsp)) ==
                  internal::make_array<std::uint8_t>(0x85, 0x04, 0x24));
    static_assert(test(dword_ptr(r13), eax) ==
                  internal::make_array<std::uint8_t>(0x41, 0x85, 0x45, 0x00));
    static_assert(test(eax, dword_ptr(esp)) ==
                  internal::make_array<std::uint8_t>(0x67, 0x85, 0x04, 0x24));
    static_assert(test(dword_ptr(rsp), 0x12345678) ==
                  internal::make_array<std::uint8_t>(0xF7, 0x04, 0x24, 0x78, 0x56, 0x34, 0x12));
}

TEST(TestInstructions, RipRelativeMemoryImmediate) {
    static_assert(test(qword_ptr(rip + 0), 0x7FFFFFFF) ==
                  internal::make_array<std::uint8_t>(0x48, 0xF7, 0x05, 0x00, 0x00, 0x00, 0x00,
                                                     0xFF, 0xFF, 0xFF, 0x7F));
    static_assert(test(byte_ptr(rip + 0), 0x80) ==
                  internal::make_array<std::uint8_t>(0xF6, 0x05, 0x00, 0x00, 0x00, 0x00, 0x80));
    static_assert(test(word_ptr(rip + 0), 0x1234) ==
                  internal::make_array<std::uint8_t>(0x66, 0xF7, 0x05, 0x00, 0x00, 0x00, 0x00, 0x34, 0x12));
    static_assert(test(qword_ptr(rip + 0), -1) ==
                  internal::make_array<std::uint8_t>(0x48, 0xF7, 0x05, 0x00, 0x00, 0x00, 0x00,
                                                     0xFF, 0xFF, 0xFF, 0xFF));
}
