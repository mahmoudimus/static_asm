#include "static_asm.hpp"
#include <gtest/gtest.h>

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// LEA tests - load effective address
TEST(LeaInstructions, Register64WithMemory) {
    // lea rax, [rcx]: 48 8D 01
    EXPECT_EQ(lea(rax, ptr(rcx)), (internal::make_array<std::uint8_t>(0x48, 0x8D, 0x01)));
    // lea rcx, [rax]: 48 8D 08
    EXPECT_EQ(lea(rcx, ptr(rax)), (internal::make_array<std::uint8_t>(0x48, 0x8D, 0x08)));
    // lea r8, [rcx]: 4C 8D 01
    EXPECT_EQ(lea(r8, ptr(rcx)), (internal::make_array<std::uint8_t>(0x4C, 0x8D, 0x01)));
    // lea rax, [r8]: 49 8D 00
    EXPECT_EQ(lea(rax, ptr(r8)), (internal::make_array<std::uint8_t>(0x49, 0x8D, 0x00)));
    // lea r8, [r9]: 4D 8D 01
    EXPECT_EQ(lea(r8, ptr(r9)), (internal::make_array<std::uint8_t>(0x4D, 0x8D, 0x01)));
}

TEST(LeaInstructions, Register32WithMemory) {
    // lea eax, [ecx]: 67 8D 01
    EXPECT_EQ(lea(eax, ptr(ecx)), (internal::make_array<std::uint8_t>(0x67, 0x8D, 0x01)));
    // lea eax, [rcx]: 8D 01
    EXPECT_EQ(lea(eax, ptr(rcx)), (internal::make_array<std::uint8_t>(0x8D, 0x01)));
    // lea r8d, [rcx]: 44 8D 01
    EXPECT_EQ(lea(r8d, ptr(rcx)), (internal::make_array<std::uint8_t>(0x44, 0x8D, 0x01)));
}
