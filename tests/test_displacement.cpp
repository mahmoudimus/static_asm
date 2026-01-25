#include <gtest/gtest.h>
#include "static_asm.hpp"

using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// Helper from instruction_db.g.hpp
using static_asm::x86::internal::make_array;

// Test displacement with MOV instruction
TEST(DisplacementTests, MovRegFromMemDisp8) {
    // mov eax, [rbx + 0x10]
    // Expected: 8B 43 10 (opcode, modrm with mod=01, disp8)
    auto result = mov(eax, dword_ptr(rbx, std::int8_t(0x10)));
    EXPECT_EQ(result, (make_array<std::uint8_t>(0x8B, 0x43, 0x10)));
}

TEST(DisplacementTests, MovRegFromMemDisp32) {
    // mov eax, [rbx + 0x12345678]
    // Expected: 8B 83 78 56 34 12 (opcode, modrm with mod=10, disp32)
    auto result = mov(eax, dword_ptr(rbx, 0x12345678));
    EXPECT_EQ(result, (make_array<std::uint8_t>(0x8B, 0x83, 0x78, 0x56, 0x34, 0x12)));
}

TEST(DisplacementTests, MovMemDisp8ToReg) {
    // mov [rbx + 0x10], eax
    // Expected: 89 43 10 (opcode, modrm with mod=01, disp8)
    auto result = mov(dword_ptr(rbx, std::int8_t(0x10)), eax);
    EXPECT_EQ(result, (make_array<std::uint8_t>(0x89, 0x43, 0x10)));
}

TEST(DisplacementTests, AddRegMemDisp8) {
    // add eax, [rcx + 0x20]
    // Expected: 03 41 20
    auto result = add(eax, dword_ptr(rcx, std::int8_t(0x20)));
    EXPECT_EQ(result, (make_array<std::uint8_t>(0x03, 0x41, 0x20)));
}

TEST(DisplacementTests, AddRegMemDisp32) {
    // add rax, [rcx + 0x1000]
    // Expected: 48 03 81 00 10 00 00
    auto result = add(rax, qword_ptr(rcx, 0x1000));
    EXPECT_EQ(result, (make_array<std::uint8_t>(0x48, 0x03, 0x81, 0x00, 0x10, 0x00, 0x00)));
}

TEST(DisplacementTests, LeaWithDisp8) {
    // lea rax, [rbx + 0x10]
    // Expected: 48 8D 43 10
    auto result = lea(rax, ptr(rbx, std::int8_t(0x10)));
    EXPECT_EQ(result, (make_array<std::uint8_t>(0x48, 0x8D, 0x43, 0x10)));
}

TEST(DisplacementTests, LeaWithDisp32) {
    // lea rax, [rbx + 0x12345678]
    // Expected: 48 8D 83 78 56 34 12
    auto result = lea(rax, ptr(rbx, 0x12345678));
    EXPECT_EQ(result, (make_array<std::uint8_t>(0x48, 0x8D, 0x83, 0x78, 0x56, 0x34, 0x12)));
}

TEST(DisplacementTests, NegativeDisp8) {
    // mov eax, [rbx - 0x10] (which is [rbx + 0xF0] as signed byte)
    // Expected: 8B 43 F0
    auto result = mov(eax, dword_ptr(rbx, std::int8_t(-0x10)));
    EXPECT_EQ(result, (make_array<std::uint8_t>(0x8B, 0x43, 0xF0)));
}

TEST(DisplacementTests, ExtendedRegisterWithDisp8) {
    // mov r8d, [r9 + 0x10]
    // Expected: 45 8B 41 10 (REX.RB, opcode, modrm, disp8)
    auto result = mov(r8d, dword_ptr(r9, std::int8_t(0x10)));
    EXPECT_EQ(result, (make_array<std::uint8_t>(0x45, 0x8B, 0x41, 0x10)));
}
