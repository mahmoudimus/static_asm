#include <gtest/gtest.h>
#include "static_asm.hpp"
// Using internal::make_array from static_asm

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

TEST(MovInstructions, RegisterToRegister) {
    EXPECT_EQ(mov(cl, bl), (internal::make_array<std::uint8_t>(0x88, 0xD9)));
    EXPECT_EQ(mov(cx, bx), (internal::make_array<std::uint8_t>(0x66, 0x89, 0xD9)));
    EXPECT_EQ(mov(ebx, ecx), (internal::make_array<std::uint8_t>(0x89, 0xCB)));
    EXPECT_EQ(mov(rbx, rcx), (internal::make_array<std::uint8_t>(0x48, 0x89, 0xCB)));
}

TEST(MovInstructions, ImmediateToMemory) {
    EXPECT_EQ(mov(byte_ptr(rbx), 0x12345678), (internal::make_array<std::uint8_t>(0xC6, 0x03, 0x78)));
    EXPECT_EQ(mov(word_ptr(rbx), 0x12345678), (internal::make_array<std::uint8_t>(0x66, 0xC7, 0x03, 0x78, 0x56)));
    EXPECT_EQ(mov(dword_ptr(rbx), 0x12345678), (internal::make_array<std::uint8_t>(0xC7, 0x03, 0x78, 0x56, 0x34, 0x12)));
    EXPECT_EQ(mov(qword_ptr(rbx), 0x12345678), (internal::make_array<std::uint8_t>(0x48, 0xC7, 0x03, 0x78, 0x56, 0x34, 0x12)));
}

TEST(MovInstructions, RegisterToMemory) {
    EXPECT_EQ(mov(byte_ptr(rbx), cl), (internal::make_array<std::uint8_t>(0x88, 0x0B)));
    EXPECT_EQ(mov(word_ptr(rbx), cx), (internal::make_array<std::uint8_t>(0x66, 0x89, 0x0B)));
    EXPECT_EQ(mov(dword_ptr(rbx), ecx), (internal::make_array<std::uint8_t>(0x89, 0x0B)));
    EXPECT_EQ(mov(qword_ptr(rbx), rcx), (internal::make_array<std::uint8_t>(0x48, 0x89, 0x0B)));
}

TEST(MovInstructions, ImmediateToRegister) {
    // Uses optimized B0+rb/B8+rd encoding (shorter than C6/C7 ModR/M form)
    EXPECT_EQ(mov(cl, 12), (internal::make_array<std::uint8_t>(0xB1, 0x0C)));                         // B0+1 ib
    EXPECT_EQ(mov(cx, 12), (internal::make_array<std::uint8_t>(0x66, 0xB9, 0x0C, 0x00)));             // 66 B8+1 iw
    EXPECT_EQ(mov(ecx, 12), (internal::make_array<std::uint8_t>(0xB9, 0x0C, 0x00, 0x00, 0x00)));      // B8+1 id
    // 64-bit still uses C7 form (sign-extended imm32 is shorter than 10-byte movabs)
    EXPECT_EQ(mov(rcx, 12), (internal::make_array<std::uint8_t>(0x48, 0xC7, 0xC1, 0x0C, 0x00, 0x00, 0x00)));
}

TEST(MovInstructions, ExtendedRegisters) {
    // Extended registers use REX prefix with B0+rb/B8+rd encoding
    EXPECT_EQ(mov(r8b, 12), (internal::make_array<std::uint8_t>(0x41, 0xB0, 0x0C)));                  // REX.B B0 ib
    EXPECT_EQ(mov(r8w, 12), (internal::make_array<std::uint8_t>(0x66, 0x41, 0xB8, 0x0C, 0x00)));      // 66 REX.B B8 iw
    EXPECT_EQ(mov(r8d, 12), (internal::make_array<std::uint8_t>(0x41, 0xB8, 0x0C, 0x00, 0x00, 0x00))); // REX.B B8 id
    // 64-bit still uses C7 form
    EXPECT_EQ(mov(r8, 12), (internal::make_array<std::uint8_t>(0x49, 0xC7, 0xC0, 0x0C, 0x00, 0x00, 0x00)));
}

TEST(MovabsInstructions, Immediate64) {
    EXPECT_EQ(movabs(rbx, 0x1234567890ABCDEF), (internal::make_array<std::uint8_t>(0x48, 0xBB, 0xEF, 0xCD, 0xAB, 0x90, 0x78, 0x56, 0x34, 0x12)));
}
