#include "static_asm.hpp"
#include <gtest/gtest.h>

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// Helper to concatenate arrays at compile time
template<typename T, std::size_t N1, std::size_t N2>
constexpr std::array<T, N1 + N2> concat(const std::array<T, N1>& a, const std::array<T, N2>& b) {
    std::array<T, N1 + N2> result{};
    for (std::size_t i = 0; i < N1; ++i)
        result[i] = a[i];
    for (std::size_t i = 0; i < N2; ++i)
        result[N1 + i] = b[i];
    return result;
}

// Windows x64 DLL loader shellcode pattern
// Tests: sub, mov with auto-movabs, xor with extended registers, call
constexpr auto build_loader_shellcode(std::uint64_t dll_base, std::uint64_t entry_point) {
    return core::assemble(
        sub(rsp, 0x28), // sub rsp, 0x28 - shadow space
        mov(rcx, dll_base), // mov rcx, imm64 (auto movabs)
        mov(rdx, 1), // mov rdx, 1
        xor_(r8, r8), // xor r8, r8
        mov(rax, entry_point), // mov rax, imm64 (auto movabs)
        call(rax), // call rax
        jmp(here) // jmp $ (infinite loop)
    );
}

TEST(ShellcodeTests, DllLoaderStructure) {
    constexpr std::uint64_t dll_base = 0x00007FF700000000;
    constexpr std::uint64_t entry_point = 0x00007FF700001000;

    constexpr auto shellcode = build_loader_shellcode(dll_base, entry_point);

    // Verify total size: 7 + 10 + 7 + 3 + 10 + 2 + 2 = 41 bytes
    EXPECT_EQ(shellcode.size(), 41u);

    // Verify sub rsp, 0x28 (7 bytes: 48 81 EC 28 00 00 00)
    EXPECT_EQ(shellcode[0], 0x48); // REX.W
    EXPECT_EQ(shellcode[1], 0x81); // opcode
    EXPECT_EQ(shellcode[2], 0xEC); // ModR/M
    EXPECT_EQ(shellcode[3], 0x28); // imm32 low byte

    // Verify mov rcx, imm64 starts at offset 7 (10 bytes: 48 B9 ...)
    EXPECT_EQ(shellcode[7], 0x48); // REX.W
    EXPECT_EQ(shellcode[8], 0xB9); // mov rcx, imm64 opcode (B8 + 1)

    // Verify mov rdx, 1 starts at offset 17 (7 bytes: 48 C7 C2 01 00 00 00)
    EXPECT_EQ(shellcode[17], 0x48); // REX.W
    EXPECT_EQ(shellcode[18], 0xC7); // opcode
    EXPECT_EQ(shellcode[19], 0xC2); // ModR/M (rdx)
    EXPECT_EQ(shellcode[20], 0x01); // imm32 = 1

    // Verify xor r8, r8 starts at offset 24 (3 bytes: 4D 31 C0)
    EXPECT_EQ(shellcode[24], 0x4D); // REX.WRB
    EXPECT_EQ(shellcode[25], 0x31); // xor opcode
    EXPECT_EQ(shellcode[26], 0xC0); // ModR/M

    // Verify mov rax, imm64 starts at offset 27 (10 bytes: 48 B8 ...)
    EXPECT_EQ(shellcode[27], 0x48); // REX.W
    EXPECT_EQ(shellcode[28], 0xB8); // mov rax, imm64 opcode

    // Verify call rax at offset 37 (2 bytes: FF D0)
    EXPECT_EQ(shellcode[37], 0xFF); // call opcode
    EXPECT_EQ(shellcode[38], 0xD0); // ModR/M (rax)

    // Verify jmp $ at offset 39 (2 bytes: EB FE)
    EXPECT_EQ(shellcode[39], 0xEB); // jmp rel8 opcode
    EXPECT_EQ(shellcode[40], 0xFE); // -2 offset
}

TEST(ShellcodeTests, JmpExtendedRegister) {
    // Test jmp r8 encoding
    constexpr auto code = jmp(r8);
    EXPECT_EQ(code.size(), 3u);
    EXPECT_EQ(code[0], 0x41); // REX.B
    EXPECT_EQ(code[1], 0xFF); // jmp opcode
    EXPECT_EQ(code[2], 0xE0); // ModR/M (r8)
}

TEST(ShellcodeTests, CallExtendedRegister) {
    // Test call r8 encoding
    constexpr auto code = call(r8);
    EXPECT_EQ(code.size(), 3u);
    EXPECT_EQ(code[0], 0x41); // REX.B
    EXPECT_EQ(code[1], 0xFF); // call opcode
    EXPECT_EQ(code[2], 0xD0); // ModR/M (r8)
}

TEST(ShellcodeTests, MovAutoMovabs) {
    // Test that mov with 64-bit immediate automatically uses movabs
    constexpr std::uint64_t large_value = 0x00007FF700000000;
    constexpr auto code = mov(rcx, large_value);

    // Should be movabs encoding: REX.W + B9 + 8 bytes = 10 bytes
    EXPECT_EQ(code.size(), 10u);
    EXPECT_EQ(code[0], 0x48); // REX.W
    EXPECT_EQ(code[1], 0xB9); // mov rcx, imm64 (B8 + 1)

    // Verify the immediate value is encoded correctly (little-endian)
    EXPECT_EQ(code[2], 0x00);
    EXPECT_EQ(code[3], 0x00);
    EXPECT_EQ(code[4], 0x00);
    EXPECT_EQ(code[5], 0x00);
    EXPECT_EQ(code[6], 0xF7);
    EXPECT_EQ(code[7], 0x7F);
    EXPECT_EQ(code[8], 0x00);
    EXPECT_EQ(code[9], 0x00);
}
