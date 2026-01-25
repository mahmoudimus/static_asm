#include "static_asm.hpp"
#include <gtest/gtest.h>
// Using internal::make_array from static_asm

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// ADC Instructions
TEST(AdcInstructions, RegisterToRegister8) {
    EXPECT_EQ(adc(cl, bl), (internal::make_array<std::uint8_t>(0x10, 0xD9)));
    EXPECT_EQ(adc(r8b, r9b), (internal::make_array<std::uint8_t>(0x45, 0x10, 0xC8)));
}

TEST(AdcInstructions, RegisterToRegister16) {
    EXPECT_EQ(adc(cx, bx), (internal::make_array<std::uint8_t>(0x66, 0x11, 0xD9)));
    EXPECT_EQ(adc(r8w, r9w), (internal::make_array<std::uint8_t>(0x66, 0x45, 0x11, 0xC8)));
}

TEST(AdcInstructions, RegisterToRegister32) {
    EXPECT_EQ(adc(ecx, ebx), (internal::make_array<std::uint8_t>(0x11, 0xD9)));
    EXPECT_EQ(adc(r8d, r9d), (internal::make_array<std::uint8_t>(0x45, 0x11, 0xC8)));
}

TEST(AdcInstructions, RegisterToRegister64) {
    EXPECT_EQ(adc(rcx, rbx), (internal::make_array<std::uint8_t>(0x48, 0x11, 0xD9)));
    EXPECT_EQ(adc(r8, r9), (internal::make_array<std::uint8_t>(0x4D, 0x11, 0xC8)));
    EXPECT_EQ(adc(rdi, r9), (internal::make_array<std::uint8_t>(0x4C, 0x11, 0xCF)));
}

TEST(AdcInstructions, MemoryOperations) {
    EXPECT_EQ(adc(ptr(edi), ebx), (internal::make_array<std::uint8_t>(0x67, 0x11, 0x1F)));
    EXPECT_EQ(adc(ptr(edi), rbx), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x11, 0x1F)));
    EXPECT_EQ(adc(ptr(rdi), ebx), (internal::make_array<std::uint8_t>(0x11, 0x1F)));
    EXPECT_EQ(adc(ptr(rdi), rbx), (internal::make_array<std::uint8_t>(0x48, 0x11, 0x1F)));
    EXPECT_EQ(adc(ebx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x13, 0x1F)));
    EXPECT_EQ(adc(rbx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x13, 0x1F)));
    EXPECT_EQ(adc(ebx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x13, 0x1F)));
    EXPECT_EQ(adc(rbx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x48, 0x13, 0x1F)));
    EXPECT_EQ(adc(ptr(edi), bx), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x11, 0x1F)));
    EXPECT_EQ(adc(ptr(rdi), bx), (internal::make_array<std::uint8_t>(0x66, 0x11, 0x1F)));
    EXPECT_EQ(adc(bx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x13, 0x1F)));
    EXPECT_EQ(adc(bx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x66, 0x13, 0x1F)));
}

TEST(AdcInstructions, DisplacementMemory) {
    EXPECT_EQ(adc(dword_ptr(0x12121212), ebx), (internal::make_array<std::uint8_t>(0x11, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(adc(qword_ptr(0x12121212), rbx), (internal::make_array<std::uint8_t>(0x48, 0x11, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(adc(ebx, dword_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x13, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(adc(rbx, qword_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x48, 0x13, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(adc(word_ptr(0x12121212), bx), (internal::make_array<std::uint8_t>(0x66, 0x11, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(adc(bx, word_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x66, 0x13, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(adc(bl, byte_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x12, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(adc(r8b, byte_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x44, 0x12, 0x04, 0x25, 0x12, 0x12, 0x12, 0x12)));
}

TEST(AdcInstructions, Immediate) {
    EXPECT_EQ(adc(cl, 0x12), (internal::make_array<std::uint8_t>(0x80, 0xD1, 0x12)));
    EXPECT_EQ(adc(r9b, 0x12), (internal::make_array<std::uint8_t>(0x41, 0x80, 0xD1, 0x12)));
    EXPECT_EQ(adc(r9w, 0x1212), (internal::make_array<std::uint8_t>(0x66, 0x41, 0x81, 0xD1, 0x12, 0x12)));
    EXPECT_EQ(adc(r9d, 0x12121212), (internal::make_array<std::uint8_t>(0x41, 0x81, 0xD1, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(adc(r9, 0x12121212), (internal::make_array<std::uint8_t>(0x49, 0x81, 0xD1, 0x12, 0x12, 0x12, 0x12)));
}

TEST(AdcInstructions, ImmediateToMemory) {
    EXPECT_EQ(adc(byte_ptr(edi), 0x12), (internal::make_array<std::uint8_t>(0x67, 0x80, 0x17, 0x12)));
    EXPECT_EQ(adc(word_ptr(edi), 0x1212), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x81, 0x17, 0x12, 0x12)));
    EXPECT_EQ(adc(dword_ptr(edi), 0x12121212), (internal::make_array<std::uint8_t>(0x67, 0x81, 0x17, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(adc(qword_ptr(edi), 0x12121212), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x81, 0x17, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(adc(byte_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x80, 0x17, 0x12)));
    EXPECT_EQ(adc(word_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x66, 0x81, 0x17, 0x12, 0x12)));
    EXPECT_EQ(adc(dword_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x81, 0x17, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(adc(qword_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x48, 0x81, 0x17, 0x12, 0x12, 0x12, 0x12)));
}

// ADD Instructions
TEST(AddInstructions, RegisterToRegister8) {
    EXPECT_EQ(add(cl, bl), (internal::make_array<std::uint8_t>(0x00, 0xD9)));
    EXPECT_EQ(add(r8b, r9b), (internal::make_array<std::uint8_t>(0x45, 0x00, 0xC8)));
}

TEST(AddInstructions, RegisterToRegister16) {
    EXPECT_EQ(add(cx, bx), (internal::make_array<std::uint8_t>(0x66, 0x01, 0xD9)));
    EXPECT_EQ(add(r8w, r9w), (internal::make_array<std::uint8_t>(0x66, 0x45, 0x01, 0xC8)));
}

TEST(AddInstructions, RegisterToRegister32) {
    EXPECT_EQ(add(ecx, ebx), (internal::make_array<std::uint8_t>(0x01, 0xD9)));
    EXPECT_EQ(add(r8d, r9d), (internal::make_array<std::uint8_t>(0x45, 0x01, 0xC8)));
}

TEST(AddInstructions, RegisterToRegister64) {
    EXPECT_EQ(add(rcx, rbx), (internal::make_array<std::uint8_t>(0x48, 0x01, 0xD9)));
    EXPECT_EQ(add(r8, r9), (internal::make_array<std::uint8_t>(0x4D, 0x01, 0xC8)));
    EXPECT_EQ(add(rdi, r9), (internal::make_array<std::uint8_t>(0x4C, 0x01, 0xCF)));
}

TEST(AddInstructions, MemoryOperations) {
    EXPECT_EQ(add(ptr(edi), ebx), (internal::make_array<std::uint8_t>(0x67, 0x01, 0x1F)));
    EXPECT_EQ(add(ptr(edi), rbx), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x01, 0x1F)));
    EXPECT_EQ(add(ptr(rdi), ebx), (internal::make_array<std::uint8_t>(0x01, 0x1F)));
    EXPECT_EQ(add(ptr(rdi), rbx), (internal::make_array<std::uint8_t>(0x48, 0x01, 0x1F)));
    EXPECT_EQ(add(ebx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x03, 0x1F)));
    EXPECT_EQ(add(rbx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x03, 0x1F)));
    EXPECT_EQ(add(ebx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x03, 0x1F)));
    EXPECT_EQ(add(rbx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x48, 0x03, 0x1F)));
    EXPECT_EQ(add(ptr(edi), bx), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x01, 0x1F)));
    EXPECT_EQ(add(ptr(rdi), bx), (internal::make_array<std::uint8_t>(0x66, 0x01, 0x1F)));
    EXPECT_EQ(add(bx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x03, 0x1F)));
    EXPECT_EQ(add(bx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x66, 0x03, 0x1F)));
}

TEST(AddInstructions, DisplacementMemory) {
    EXPECT_EQ(add(dword_ptr(0x12121212), ebx), (internal::make_array<std::uint8_t>(0x01, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(add(qword_ptr(0x12121212), rbx), (internal::make_array<std::uint8_t>(0x48, 0x01, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(add(ebx, dword_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x03, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(add(rbx, qword_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x48, 0x03, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(add(word_ptr(0x12121212), bx), (internal::make_array<std::uint8_t>(0x66, 0x01, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(add(bx, word_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x66, 0x03, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(add(bl, byte_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x02, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(add(r8b, byte_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x44, 0x02, 0x04, 0x25, 0x12, 0x12, 0x12, 0x12)));
}

TEST(AddInstructions, Immediate) {
    EXPECT_EQ(add(cl, 0x12), (internal::make_array<std::uint8_t>(0x80, 0xC1, 0x12)));
    EXPECT_EQ(add(r9b, 0x12), (internal::make_array<std::uint8_t>(0x41, 0x80, 0xC1, 0x12)));
    EXPECT_EQ(add(r9w, 0x1212), (internal::make_array<std::uint8_t>(0x66, 0x41, 0x81, 0xC1, 0x12, 0x12)));
    EXPECT_EQ(add(r9d, 0x12121212), (internal::make_array<std::uint8_t>(0x41, 0x81, 0xC1, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(add(r9, 0x12121212), (internal::make_array<std::uint8_t>(0x49, 0x81, 0xC1, 0x12, 0x12, 0x12, 0x12)));
}

TEST(AddInstructions, ImmediateToMemory) {
    EXPECT_EQ(add(byte_ptr(edi), 0x12), (internal::make_array<std::uint8_t>(0x67, 0x80, 0x07, 0x12)));
    EXPECT_EQ(add(word_ptr(edi), 0x1212), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x81, 0x07, 0x12, 0x12)));
    EXPECT_EQ(add(dword_ptr(edi), 0x12121212), (internal::make_array<std::uint8_t>(0x67, 0x81, 0x07, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(add(qword_ptr(edi), 0x12121212), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x81, 0x07, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(add(byte_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x80, 0x07, 0x12)));
    EXPECT_EQ(add(word_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x66, 0x81, 0x07, 0x12, 0x12)));
    EXPECT_EQ(add(dword_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x81, 0x07, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(add(qword_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x48, 0x81, 0x07, 0x12, 0x12, 0x12, 0x12)));
}

// AND Instructions
TEST(AndInstructions, RegisterToRegister8) {
    EXPECT_EQ(and_(cl, bl), (internal::make_array<std::uint8_t>(0x20, 0xD9)));
    EXPECT_EQ(and_(r8b, r9b), (internal::make_array<std::uint8_t>(0x45, 0x20, 0xC8)));
}

TEST(AndInstructions, RegisterToRegister16) {
    EXPECT_EQ(and_(cx, bx), (internal::make_array<std::uint8_t>(0x66, 0x21, 0xD9)));
    EXPECT_EQ(and_(r8w, r9w), (internal::make_array<std::uint8_t>(0x66, 0x45, 0x21, 0xC8)));
}

TEST(AndInstructions, RegisterToRegister32) {
    EXPECT_EQ(and_(ecx, ebx), (internal::make_array<std::uint8_t>(0x21, 0xD9)));
    EXPECT_EQ(and_(r8d, r9d), (internal::make_array<std::uint8_t>(0x45, 0x21, 0xC8)));
}

TEST(AndInstructions, RegisterToRegister64) {
    EXPECT_EQ(and_(rcx, rbx), (internal::make_array<std::uint8_t>(0x48, 0x21, 0xD9)));
    EXPECT_EQ(and_(r8, r9), (internal::make_array<std::uint8_t>(0x4D, 0x21, 0xC8)));
    EXPECT_EQ(and_(rdi, r9), (internal::make_array<std::uint8_t>(0x4C, 0x21, 0xCF)));
}

TEST(AndInstructions, MemoryOperations) {
    EXPECT_EQ(and_(ptr(edi), ebx), (internal::make_array<std::uint8_t>(0x67, 0x21, 0x1F)));
    EXPECT_EQ(and_(ptr(edi), rbx), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x21, 0x1F)));
    EXPECT_EQ(and_(ptr(rdi), ebx), (internal::make_array<std::uint8_t>(0x21, 0x1F)));
    EXPECT_EQ(and_(ptr(rdi), rbx), (internal::make_array<std::uint8_t>(0x48, 0x21, 0x1F)));
    EXPECT_EQ(and_(ebx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x23, 0x1F)));
    EXPECT_EQ(and_(rbx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x23, 0x1F)));
    EXPECT_EQ(and_(ebx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x23, 0x1F)));
    EXPECT_EQ(and_(rbx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x48, 0x23, 0x1F)));
    EXPECT_EQ(and_(ptr(edi), bx), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x21, 0x1F)));
    EXPECT_EQ(and_(ptr(rdi), bx), (internal::make_array<std::uint8_t>(0x66, 0x21, 0x1F)));
    EXPECT_EQ(and_(bx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x23, 0x1F)));
    EXPECT_EQ(and_(bx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x66, 0x23, 0x1F)));
}

TEST(AndInstructions, DisplacementMemory) {
    EXPECT_EQ(and_(dword_ptr(0x12121212), ebx), (internal::make_array<std::uint8_t>(0x21, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(and_(qword_ptr(0x12121212), rbx), (internal::make_array<std::uint8_t>(0x48, 0x21, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(and_(ebx, dword_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x23, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(and_(rbx, qword_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x48, 0x23, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(and_(word_ptr(0x12121212), bx), (internal::make_array<std::uint8_t>(0x66, 0x21, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(and_(bx, word_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x66, 0x23, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(and_(bl, byte_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x22, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(and_(r8b, byte_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x44, 0x22, 0x04, 0x25, 0x12, 0x12, 0x12, 0x12)));
}

TEST(AndInstructions, Immediate) {
    EXPECT_EQ(and_(cl, 0x12), (internal::make_array<std::uint8_t>(0x80, 0xE1, 0x12)));
    EXPECT_EQ(and_(r9b, 0x12), (internal::make_array<std::uint8_t>(0x41, 0x80, 0xE1, 0x12)));
    EXPECT_EQ(and_(r9w, 0x1212), (internal::make_array<std::uint8_t>(0x66, 0x41, 0x81, 0xE1, 0x12, 0x12)));
    EXPECT_EQ(and_(r9d, 0x12121212), (internal::make_array<std::uint8_t>(0x41, 0x81, 0xE1, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(and_(r9, 0x12121212), (internal::make_array<std::uint8_t>(0x49, 0x81, 0xE1, 0x12, 0x12, 0x12, 0x12)));
}

TEST(AndInstructions, ImmediateToMemory) {
    EXPECT_EQ(and_(byte_ptr(edi), 0x12), (internal::make_array<std::uint8_t>(0x67, 0x80, 0x27, 0x12)));
    EXPECT_EQ(and_(word_ptr(edi), 0x1212), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x81, 0x27, 0x12, 0x12)));
    EXPECT_EQ(and_(dword_ptr(edi), 0x12121212), (internal::make_array<std::uint8_t>(0x67, 0x81, 0x27, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(and_(qword_ptr(edi), 0x12121212), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x81, 0x27, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(and_(byte_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x80, 0x27, 0x12)));
    EXPECT_EQ(and_(word_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x66, 0x81, 0x27, 0x12, 0x12)));
    EXPECT_EQ(and_(dword_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x81, 0x27, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(and_(qword_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x48, 0x81, 0x27, 0x12, 0x12, 0x12, 0x12)));
}

// CMP Instructions
TEST(CmpInstructions, RegisterToRegister8) {
    EXPECT_EQ(cmp(cl, bl), (internal::make_array<std::uint8_t>(0x38, 0xD9)));
    EXPECT_EQ(cmp(r8b, r9b), (internal::make_array<std::uint8_t>(0x45, 0x38, 0xC8)));
}

TEST(CmpInstructions, RegisterToRegister16) {
    EXPECT_EQ(cmp(cx, bx), (internal::make_array<std::uint8_t>(0x66, 0x39, 0xD9)));
    EXPECT_EQ(cmp(r8w, r9w), (internal::make_array<std::uint8_t>(0x66, 0x45, 0x39, 0xC8)));
}

TEST(CmpInstructions, RegisterToRegister32) {
    EXPECT_EQ(cmp(ecx, ebx), (internal::make_array<std::uint8_t>(0x39, 0xD9)));
    EXPECT_EQ(cmp(r8d, r9d), (internal::make_array<std::uint8_t>(0x45, 0x39, 0xC8)));
}

TEST(CmpInstructions, RegisterToRegister64) {
    EXPECT_EQ(cmp(rcx, rbx), (internal::make_array<std::uint8_t>(0x48, 0x39, 0xD9)));
    EXPECT_EQ(cmp(r8, r9), (internal::make_array<std::uint8_t>(0x4D, 0x39, 0xC8)));
    EXPECT_EQ(cmp(rdi, r9), (internal::make_array<std::uint8_t>(0x4C, 0x39, 0xCF)));
}

TEST(CmpInstructions, MemoryOperations) {
    EXPECT_EQ(cmp(ptr(edi), ebx), (internal::make_array<std::uint8_t>(0x67, 0x39, 0x1F)));
    EXPECT_EQ(cmp(ptr(edi), rbx), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x39, 0x1F)));
    EXPECT_EQ(cmp(ptr(rdi), ebx), (internal::make_array<std::uint8_t>(0x39, 0x1F)));
    EXPECT_EQ(cmp(ptr(rdi), rbx), (internal::make_array<std::uint8_t>(0x48, 0x39, 0x1F)));
    EXPECT_EQ(cmp(ebx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x3B, 0x1F)));
    EXPECT_EQ(cmp(rbx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x3B, 0x1F)));
    EXPECT_EQ(cmp(ebx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x3B, 0x1F)));
    EXPECT_EQ(cmp(rbx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x48, 0x3B, 0x1F)));
    EXPECT_EQ(cmp(ptr(edi), bx), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x39, 0x1F)));
    EXPECT_EQ(cmp(ptr(rdi), bx), (internal::make_array<std::uint8_t>(0x66, 0x39, 0x1F)));
    EXPECT_EQ(cmp(bx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x3B, 0x1F)));
    EXPECT_EQ(cmp(bx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x66, 0x3B, 0x1F)));
}

TEST(CmpInstructions, DisplacementMemory) {
    EXPECT_EQ(cmp(dword_ptr(0x12121212), ebx), (internal::make_array<std::uint8_t>(0x39, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(cmp(qword_ptr(0x12121212), rbx), (internal::make_array<std::uint8_t>(0x48, 0x39, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(cmp(ebx, dword_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x3B, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(cmp(rbx, qword_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x48, 0x3B, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(cmp(word_ptr(0x12121212), bx), (internal::make_array<std::uint8_t>(0x66, 0x39, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(cmp(bx, word_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x66, 0x3B, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(cmp(bl, byte_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x3A, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(cmp(r8b, byte_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x44, 0x3A, 0x04, 0x25, 0x12, 0x12, 0x12, 0x12)));
}

TEST(CmpInstructions, Immediate) {
    EXPECT_EQ(cmp(cl, 0x12), (internal::make_array<std::uint8_t>(0x80, 0xF9, 0x12)));
    EXPECT_EQ(cmp(r9b, 0x12), (internal::make_array<std::uint8_t>(0x41, 0x80, 0xF9, 0x12)));
    EXPECT_EQ(cmp(r9w, 0x1212), (internal::make_array<std::uint8_t>(0x66, 0x41, 0x81, 0xF9, 0x12, 0x12)));
    EXPECT_EQ(cmp(r9d, 0x12121212), (internal::make_array<std::uint8_t>(0x41, 0x81, 0xF9, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(cmp(r9, 0x12121212), (internal::make_array<std::uint8_t>(0x49, 0x81, 0xF9, 0x12, 0x12, 0x12, 0x12)));
}

TEST(CmpInstructions, ImmediateToMemory) {
    EXPECT_EQ(cmp(byte_ptr(edi), 0x12), (internal::make_array<std::uint8_t>(0x67, 0x80, 0x3F, 0x12)));
    EXPECT_EQ(cmp(word_ptr(edi), 0x1212), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x81, 0x3F, 0x12, 0x12)));
    EXPECT_EQ(cmp(dword_ptr(edi), 0x12121212), (internal::make_array<std::uint8_t>(0x67, 0x81, 0x3F, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(cmp(qword_ptr(edi), 0x12121212), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x81, 0x3F, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(cmp(byte_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x80, 0x3F, 0x12)));
    EXPECT_EQ(cmp(word_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x66, 0x81, 0x3F, 0x12, 0x12)));
    EXPECT_EQ(cmp(dword_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x81, 0x3F, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(cmp(qword_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x48, 0x81, 0x3F, 0x12, 0x12, 0x12, 0x12)));
}

// OR Instructions
TEST(OrInstructions, RegisterToRegister8) {
    EXPECT_EQ(or_(cl, bl), (internal::make_array<std::uint8_t>(0x08, 0xD9)));
    EXPECT_EQ(or_(r8b, r9b), (internal::make_array<std::uint8_t>(0x45, 0x08, 0xC8)));
}

TEST(OrInstructions, RegisterToRegister16) {
    EXPECT_EQ(or_(cx, bx), (internal::make_array<std::uint8_t>(0x66, 0x09, 0xD9)));
    EXPECT_EQ(or_(r8w, r9w), (internal::make_array<std::uint8_t>(0x66, 0x45, 0x09, 0xC8)));
}

TEST(OrInstructions, RegisterToRegister32) {
    EXPECT_EQ(or_(ecx, ebx), (internal::make_array<std::uint8_t>(0x09, 0xD9)));
    EXPECT_EQ(or_(r8d, r9d), (internal::make_array<std::uint8_t>(0x45, 0x09, 0xC8)));
}

TEST(OrInstructions, RegisterToRegister64) {
    EXPECT_EQ(or_(rcx, rbx), (internal::make_array<std::uint8_t>(0x48, 0x09, 0xD9)));
    EXPECT_EQ(or_(r8, r9), (internal::make_array<std::uint8_t>(0x4D, 0x09, 0xC8)));
    EXPECT_EQ(or_(rdi, r9), (internal::make_array<std::uint8_t>(0x4C, 0x09, 0xCF)));
}

TEST(OrInstructions, MemoryOperations) {
    EXPECT_EQ(or_(ptr(edi), ebx), (internal::make_array<std::uint8_t>(0x67, 0x09, 0x1F)));
    EXPECT_EQ(or_(ptr(edi), rbx), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x09, 0x1F)));
    EXPECT_EQ(or_(ptr(rdi), ebx), (internal::make_array<std::uint8_t>(0x09, 0x1F)));
    EXPECT_EQ(or_(ptr(rdi), rbx), (internal::make_array<std::uint8_t>(0x48, 0x09, 0x1F)));
    EXPECT_EQ(or_(ebx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x0B, 0x1F)));
    EXPECT_EQ(or_(rbx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x0B, 0x1F)));
    EXPECT_EQ(or_(ebx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x0B, 0x1F)));
    EXPECT_EQ(or_(rbx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x48, 0x0B, 0x1F)));
    EXPECT_EQ(or_(ptr(edi), bx), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x09, 0x1F)));
    EXPECT_EQ(or_(ptr(rdi), bx), (internal::make_array<std::uint8_t>(0x66, 0x09, 0x1F)));
    EXPECT_EQ(or_(bx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x0B, 0x1F)));
    EXPECT_EQ(or_(bx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x66, 0x0B, 0x1F)));
}

TEST(OrInstructions, DisplacementMemory) {
    EXPECT_EQ(or_(dword_ptr(0x12121212), ebx), (internal::make_array<std::uint8_t>(0x09, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(or_(qword_ptr(0x12121212), rbx), (internal::make_array<std::uint8_t>(0x48, 0x09, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(or_(ebx, dword_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x0B, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(or_(rbx, qword_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x48, 0x0B, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(or_(word_ptr(0x12121212), bx), (internal::make_array<std::uint8_t>(0x66, 0x09, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(or_(bx, word_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x66, 0x0B, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(or_(bl, byte_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x0A, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(or_(r8b, byte_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x44, 0x0A, 0x04, 0x25, 0x12, 0x12, 0x12, 0x12)));
}

TEST(OrInstructions, Immediate) {
    EXPECT_EQ(or_(cl, 0x12), (internal::make_array<std::uint8_t>(0x80, 0xC9, 0x12)));
    EXPECT_EQ(or_(r9b, 0x12), (internal::make_array<std::uint8_t>(0x41, 0x80, 0xC9, 0x12)));
    EXPECT_EQ(or_(r9w, 0x1212), (internal::make_array<std::uint8_t>(0x66, 0x41, 0x81, 0xC9, 0x12, 0x12)));
    EXPECT_EQ(or_(r9d, 0x12121212), (internal::make_array<std::uint8_t>(0x41, 0x81, 0xC9, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(or_(r9, 0x12121212), (internal::make_array<std::uint8_t>(0x49, 0x81, 0xC9, 0x12, 0x12, 0x12, 0x12)));
}

TEST(OrInstructions, ImmediateToMemory) {
    EXPECT_EQ(or_(byte_ptr(edi), 0x12), (internal::make_array<std::uint8_t>(0x67, 0x80, 0x0F, 0x12)));
    EXPECT_EQ(or_(word_ptr(edi), 0x1212), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x81, 0x0F, 0x12, 0x12)));
    EXPECT_EQ(or_(dword_ptr(edi), 0x12121212), (internal::make_array<std::uint8_t>(0x67, 0x81, 0x0F, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(or_(qword_ptr(edi), 0x12121212), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x81, 0x0F, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(or_(byte_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x80, 0x0F, 0x12)));
    EXPECT_EQ(or_(word_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x66, 0x81, 0x0F, 0x12, 0x12)));
    EXPECT_EQ(or_(dword_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x81, 0x0F, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(or_(qword_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x48, 0x81, 0x0F, 0x12, 0x12, 0x12, 0x12)));
}

// SBB Instructions
TEST(SbbInstructions, RegisterToRegister8) {
    EXPECT_EQ(sbb(cl, bl), (internal::make_array<std::uint8_t>(0x18, 0xD9)));
    EXPECT_EQ(sbb(r8b, r9b), (internal::make_array<std::uint8_t>(0x45, 0x18, 0xC8)));
}

TEST(SbbInstructions, RegisterToRegister16) {
    EXPECT_EQ(sbb(cx, bx), (internal::make_array<std::uint8_t>(0x66, 0x19, 0xD9)));
    EXPECT_EQ(sbb(r8w, r9w), (internal::make_array<std::uint8_t>(0x66, 0x45, 0x19, 0xC8)));
}

TEST(SbbInstructions, RegisterToRegister32) {
    EXPECT_EQ(sbb(ecx, ebx), (internal::make_array<std::uint8_t>(0x19, 0xD9)));
    EXPECT_EQ(sbb(r8d, r9d), (internal::make_array<std::uint8_t>(0x45, 0x19, 0xC8)));
}

TEST(SbbInstructions, RegisterToRegister64) {
    EXPECT_EQ(sbb(rcx, rbx), (internal::make_array<std::uint8_t>(0x48, 0x19, 0xD9)));
    EXPECT_EQ(sbb(r8, r9), (internal::make_array<std::uint8_t>(0x4D, 0x19, 0xC8)));
    EXPECT_EQ(sbb(rdi, r9), (internal::make_array<std::uint8_t>(0x4C, 0x19, 0xCF)));
}

TEST(SbbInstructions, MemoryOperations) {
    EXPECT_EQ(sbb(ptr(edi), ebx), (internal::make_array<std::uint8_t>(0x67, 0x19, 0x1F)));
    EXPECT_EQ(sbb(ptr(edi), rbx), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x19, 0x1F)));
    EXPECT_EQ(sbb(ptr(rdi), ebx), (internal::make_array<std::uint8_t>(0x19, 0x1F)));
    EXPECT_EQ(sbb(ptr(rdi), rbx), (internal::make_array<std::uint8_t>(0x48, 0x19, 0x1F)));
    EXPECT_EQ(sbb(ebx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x1B, 0x1F)));
    EXPECT_EQ(sbb(rbx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x1B, 0x1F)));
    EXPECT_EQ(sbb(ebx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x1B, 0x1F)));
    EXPECT_EQ(sbb(rbx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x48, 0x1B, 0x1F)));
    EXPECT_EQ(sbb(ptr(edi), bx), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x19, 0x1F)));
    EXPECT_EQ(sbb(ptr(rdi), bx), (internal::make_array<std::uint8_t>(0x66, 0x19, 0x1F)));
    EXPECT_EQ(sbb(bx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x1B, 0x1F)));
    EXPECT_EQ(sbb(bx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x66, 0x1B, 0x1F)));
}

TEST(SbbInstructions, DisplacementMemory) {
    EXPECT_EQ(sbb(dword_ptr(0x12121212), ebx), (internal::make_array<std::uint8_t>(0x19, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sbb(qword_ptr(0x12121212), rbx), (internal::make_array<std::uint8_t>(0x48, 0x19, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sbb(ebx, dword_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x1B, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sbb(rbx, qword_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x48, 0x1B, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sbb(word_ptr(0x12121212), bx), (internal::make_array<std::uint8_t>(0x66, 0x19, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sbb(bx, word_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x66, 0x1B, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sbb(bl, byte_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x1A, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sbb(r8b, byte_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x44, 0x1A, 0x04, 0x25, 0x12, 0x12, 0x12, 0x12)));
}

TEST(SbbInstructions, Immediate) {
    EXPECT_EQ(sbb(cl, 0x12), (internal::make_array<std::uint8_t>(0x80, 0xD9, 0x12)));
    EXPECT_EQ(sbb(r9b, 0x12), (internal::make_array<std::uint8_t>(0x41, 0x80, 0xD9, 0x12)));
    EXPECT_EQ(sbb(r9w, 0x1212), (internal::make_array<std::uint8_t>(0x66, 0x41, 0x81, 0xD9, 0x12, 0x12)));
    EXPECT_EQ(sbb(r9d, 0x12121212), (internal::make_array<std::uint8_t>(0x41, 0x81, 0xD9, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sbb(r9, 0x12121212), (internal::make_array<std::uint8_t>(0x49, 0x81, 0xD9, 0x12, 0x12, 0x12, 0x12)));
}

TEST(SbbInstructions, ImmediateToMemory) {
    EXPECT_EQ(sbb(byte_ptr(edi), 0x12), (internal::make_array<std::uint8_t>(0x67, 0x80, 0x1F, 0x12)));
    EXPECT_EQ(sbb(word_ptr(edi), 0x1212), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x81, 0x1F, 0x12, 0x12)));
    EXPECT_EQ(sbb(dword_ptr(edi), 0x12121212), (internal::make_array<std::uint8_t>(0x67, 0x81, 0x1F, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sbb(qword_ptr(edi), 0x12121212), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x81, 0x1F, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sbb(byte_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x80, 0x1F, 0x12)));
    EXPECT_EQ(sbb(word_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x66, 0x81, 0x1F, 0x12, 0x12)));
    EXPECT_EQ(sbb(dword_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x81, 0x1F, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sbb(qword_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x48, 0x81, 0x1F, 0x12, 0x12, 0x12, 0x12)));
}

// SUB Instructions
TEST(SubInstructions, RegisterToRegister8) {
    EXPECT_EQ(sub(cl, bl), (internal::make_array<std::uint8_t>(0x28, 0xD9)));
    EXPECT_EQ(sub(r8b, r9b), (internal::make_array<std::uint8_t>(0x45, 0x28, 0xC8)));
}

TEST(SubInstructions, RegisterToRegister16) {
    EXPECT_EQ(sub(cx, bx), (internal::make_array<std::uint8_t>(0x66, 0x29, 0xD9)));
    EXPECT_EQ(sub(r8w, r9w), (internal::make_array<std::uint8_t>(0x66, 0x45, 0x29, 0xC8)));
}

TEST(SubInstructions, RegisterToRegister32) {
    EXPECT_EQ(sub(ecx, ebx), (internal::make_array<std::uint8_t>(0x29, 0xD9)));
    EXPECT_EQ(sub(r8d, r9d), (internal::make_array<std::uint8_t>(0x45, 0x29, 0xC8)));
}

TEST(SubInstructions, RegisterToRegister64) {
    EXPECT_EQ(sub(rcx, rbx), (internal::make_array<std::uint8_t>(0x48, 0x29, 0xD9)));
    EXPECT_EQ(sub(r8, r9), (internal::make_array<std::uint8_t>(0x4D, 0x29, 0xC8)));
    EXPECT_EQ(sub(rdi, r9), (internal::make_array<std::uint8_t>(0x4C, 0x29, 0xCF)));
}

TEST(SubInstructions, MemoryOperations) {
    EXPECT_EQ(sub(ptr(edi), ebx), (internal::make_array<std::uint8_t>(0x67, 0x29, 0x1F)));
    EXPECT_EQ(sub(ptr(edi), rbx), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x29, 0x1F)));
    EXPECT_EQ(sub(ptr(rdi), ebx), (internal::make_array<std::uint8_t>(0x29, 0x1F)));
    EXPECT_EQ(sub(ptr(rdi), rbx), (internal::make_array<std::uint8_t>(0x48, 0x29, 0x1F)));
    EXPECT_EQ(sub(ebx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x2B, 0x1F)));
    EXPECT_EQ(sub(rbx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x2B, 0x1F)));
    EXPECT_EQ(sub(ebx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x2B, 0x1F)));
    EXPECT_EQ(sub(rbx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x48, 0x2B, 0x1F)));
    EXPECT_EQ(sub(ptr(edi), bx), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x29, 0x1F)));
    EXPECT_EQ(sub(ptr(rdi), bx), (internal::make_array<std::uint8_t>(0x66, 0x29, 0x1F)));
    EXPECT_EQ(sub(bx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x2B, 0x1F)));
    EXPECT_EQ(sub(bx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x66, 0x2B, 0x1F)));
}

TEST(SubInstructions, DisplacementMemory) {
    EXPECT_EQ(sub(dword_ptr(0x12121212), ebx), (internal::make_array<std::uint8_t>(0x29, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sub(qword_ptr(0x12121212), rbx), (internal::make_array<std::uint8_t>(0x48, 0x29, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sub(ebx, dword_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x2B, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sub(rbx, qword_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x48, 0x2B, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sub(word_ptr(0x12121212), bx), (internal::make_array<std::uint8_t>(0x66, 0x29, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sub(bx, word_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x66, 0x2B, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sub(bl, byte_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x2A, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sub(r8b, byte_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x44, 0x2A, 0x04, 0x25, 0x12, 0x12, 0x12, 0x12)));
}

TEST(SubInstructions, Immediate) {
    EXPECT_EQ(sub(cl, 0x12), (internal::make_array<std::uint8_t>(0x80, 0xE9, 0x12)));
    EXPECT_EQ(sub(r9b, 0x12), (internal::make_array<std::uint8_t>(0x41, 0x80, 0xE9, 0x12)));
    EXPECT_EQ(sub(r9w, 0x1212), (internal::make_array<std::uint8_t>(0x66, 0x41, 0x81, 0xE9, 0x12, 0x12)));
    EXPECT_EQ(sub(r9d, 0x12121212), (internal::make_array<std::uint8_t>(0x41, 0x81, 0xE9, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sub(r9, 0x12121212), (internal::make_array<std::uint8_t>(0x49, 0x81, 0xE9, 0x12, 0x12, 0x12, 0x12)));
}

TEST(SubInstructions, ImmediateToMemory) {
    EXPECT_EQ(sub(byte_ptr(edi), 0x12), (internal::make_array<std::uint8_t>(0x67, 0x80, 0x2F, 0x12)));
    EXPECT_EQ(sub(word_ptr(edi), 0x1212), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x81, 0x2F, 0x12, 0x12)));
    EXPECT_EQ(sub(dword_ptr(edi), 0x12121212), (internal::make_array<std::uint8_t>(0x67, 0x81, 0x2F, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sub(qword_ptr(edi), 0x12121212), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x81, 0x2F, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sub(byte_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x80, 0x2F, 0x12)));
    EXPECT_EQ(sub(word_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x66, 0x81, 0x2F, 0x12, 0x12)));
    EXPECT_EQ(sub(dword_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x81, 0x2F, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(sub(qword_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x48, 0x81, 0x2F, 0x12, 0x12, 0x12, 0x12)));
}

// XOR Instructions
TEST(XorInstructions, RegisterToRegister8) {
    EXPECT_EQ(xor_(cl, bl), (internal::make_array<std::uint8_t>(0x30, 0xD9)));
    EXPECT_EQ(xor_(r8b, r9b), (internal::make_array<std::uint8_t>(0x45, 0x30, 0xC8)));
}

TEST(XorInstructions, RegisterToRegister16) {
    EXPECT_EQ(xor_(cx, bx), (internal::make_array<std::uint8_t>(0x66, 0x31, 0xD9)));
    EXPECT_EQ(xor_(r8w, r9w), (internal::make_array<std::uint8_t>(0x66, 0x45, 0x31, 0xC8)));
}

TEST(XorInstructions, RegisterToRegister32) {
    EXPECT_EQ(xor_(ecx, ebx), (internal::make_array<std::uint8_t>(0x31, 0xD9)));
    EXPECT_EQ(xor_(r8d, r9d), (internal::make_array<std::uint8_t>(0x45, 0x31, 0xC8)));
}

TEST(XorInstructions, RegisterToRegister64) {
    EXPECT_EQ(xor_(rcx, rbx), (internal::make_array<std::uint8_t>(0x48, 0x31, 0xD9)));
    EXPECT_EQ(xor_(r8, r9), (internal::make_array<std::uint8_t>(0x4D, 0x31, 0xC8)));
    EXPECT_EQ(xor_(rdi, r9), (internal::make_array<std::uint8_t>(0x4C, 0x31, 0xCF)));
}

TEST(XorInstructions, MemoryOperations) {
    EXPECT_EQ(xor_(ptr(edi), ebx), (internal::make_array<std::uint8_t>(0x67, 0x31, 0x1F)));
    EXPECT_EQ(xor_(ptr(edi), rbx), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x31, 0x1F)));
    EXPECT_EQ(xor_(ptr(rdi), ebx), (internal::make_array<std::uint8_t>(0x31, 0x1F)));
    EXPECT_EQ(xor_(ptr(rdi), rbx), (internal::make_array<std::uint8_t>(0x48, 0x31, 0x1F)));
    EXPECT_EQ(xor_(ebx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x33, 0x1F)));
    EXPECT_EQ(xor_(rbx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x33, 0x1F)));
    EXPECT_EQ(xor_(ebx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x33, 0x1F)));
    EXPECT_EQ(xor_(rbx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x48, 0x33, 0x1F)));
    EXPECT_EQ(xor_(ptr(edi), bx), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x31, 0x1F)));
    EXPECT_EQ(xor_(ptr(rdi), bx), (internal::make_array<std::uint8_t>(0x66, 0x31, 0x1F)));
    EXPECT_EQ(xor_(bx, ptr(edi)), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x33, 0x1F)));
    EXPECT_EQ(xor_(bx, ptr(rdi)), (internal::make_array<std::uint8_t>(0x66, 0x33, 0x1F)));
}

TEST(XorInstructions, DisplacementMemory) {
    EXPECT_EQ(xor_(dword_ptr(0x12121212), ebx), (internal::make_array<std::uint8_t>(0x31, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(xor_(qword_ptr(0x12121212), rbx), (internal::make_array<std::uint8_t>(0x48, 0x31, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(xor_(ebx, dword_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x33, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(xor_(rbx, qword_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x48, 0x33, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(xor_(word_ptr(0x12121212), bx), (internal::make_array<std::uint8_t>(0x66, 0x31, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(xor_(bx, word_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x66, 0x33, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(xor_(bl, byte_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x32, 0x1C, 0x25, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(xor_(r8b, byte_ptr(0x12121212)), (internal::make_array<std::uint8_t>(0x44, 0x32, 0x04, 0x25, 0x12, 0x12, 0x12, 0x12)));
}

TEST(XorInstructions, Immediate) {
    EXPECT_EQ(xor_(cl, 0x12), (internal::make_array<std::uint8_t>(0x80, 0xF1, 0x12)));
    EXPECT_EQ(xor_(r9b, 0x12), (internal::make_array<std::uint8_t>(0x41, 0x80, 0xF1, 0x12)));
    EXPECT_EQ(xor_(r9w, 0x1212), (internal::make_array<std::uint8_t>(0x66, 0x41, 0x81, 0xF1, 0x12, 0x12)));
    EXPECT_EQ(xor_(r9d, 0x12121212), (internal::make_array<std::uint8_t>(0x41, 0x81, 0xF1, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(xor_(r9, 0x12121212), (internal::make_array<std::uint8_t>(0x49, 0x81, 0xF1, 0x12, 0x12, 0x12, 0x12)));
}

TEST(XorInstructions, ImmediateToMemory) {
    EXPECT_EQ(xor_(byte_ptr(edi), 0x12), (internal::make_array<std::uint8_t>(0x67, 0x80, 0x37, 0x12)));
    EXPECT_EQ(xor_(word_ptr(edi), 0x1212), (internal::make_array<std::uint8_t>(0x67, 0x66, 0x81, 0x37, 0x12, 0x12)));
    EXPECT_EQ(xor_(dword_ptr(edi), 0x12121212), (internal::make_array<std::uint8_t>(0x67, 0x81, 0x37, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(xor_(qword_ptr(edi), 0x12121212), (internal::make_array<std::uint8_t>(0x67, 0x48, 0x81, 0x37, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(xor_(byte_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x80, 0x37, 0x12)));
    EXPECT_EQ(xor_(word_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x66, 0x81, 0x37, 0x12, 0x12)));
    EXPECT_EQ(xor_(dword_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x81, 0x37, 0x12, 0x12, 0x12, 0x12)));
    EXPECT_EQ(xor_(qword_ptr(rdi), 0x12121212), (internal::make_array<std::uint8_t>(0x48, 0x81, 0x37, 0x12, 0x12, 0x12, 0x12)));
}
