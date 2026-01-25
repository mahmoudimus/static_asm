#include "static_asm.hpp"
#include <gtest/gtest.h>

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// =============================================================================
// System Call Instructions
// =============================================================================

TEST(SystemInstructions, Syscall) {
    // SYSCALL: 0F 05
    EXPECT_EQ(syscall_(), (internal::make_array<std::uint8_t>(0x0F, 0x05)));
}

TEST(SystemInstructions, Sysenter) {
    // SYSENTER: 0F 34
    EXPECT_EQ(sysenter(), (internal::make_array<std::uint8_t>(0x0F, 0x34)));
}

TEST(SystemInstructions, Sysexit) {
    // SYSEXIT: 0F 35
    EXPECT_EQ(sysexit(), (internal::make_array<std::uint8_t>(0x0F, 0x35)));
}

// =============================================================================
// Interrupt Instructions
// =============================================================================

TEST(SystemInstructions, Int3) {
    // INT3: CC
    EXPECT_EQ(int3(), (internal::make_array<std::uint8_t>(0xCC)));
}

TEST(SystemInstructions, IntImm8) {
    // INT 0x80: CD 80 (Linux 32-bit syscall)
    EXPECT_EQ(int_(0x80), (internal::make_array<std::uint8_t>(0xCD, 0x80)));
}

TEST(SystemInstructions, Int0x2E) {
    // INT 0x2E: CD 2E (Windows syscall - legacy)
    EXPECT_EQ(int_0x2e(), (internal::make_array<std::uint8_t>(0xCD, 0x2E)));
}

TEST(SystemInstructions, Int0x80) {
    // INT 0x80: CD 80 (Linux 32-bit syscall)
    EXPECT_EQ(int_0x80(), (internal::make_array<std::uint8_t>(0xCD, 0x80)));
}

TEST(SystemInstructions, IntArbitrary) {
    // INT 0x03: CD 03
    EXPECT_EQ(int_(0x03), (internal::make_array<std::uint8_t>(0xCD, 0x03)));
    // INT 0x21: CD 21 (DOS syscall)
    EXPECT_EQ(int_(0x21), (internal::make_array<std::uint8_t>(0xCD, 0x21)));
}

TEST(SystemInstructions, Into) {
    // INTO: CE (interrupt on overflow - invalid in 64-bit mode)
    EXPECT_EQ(into(), (internal::make_array<std::uint8_t>(0xCE)));
}

// =============================================================================
// Interrupt Return Instructions
// =============================================================================

TEST(SystemInstructions, Iret) {
    // IRET: CF (16-bit)
    EXPECT_EQ(iret(), (internal::make_array<std::uint8_t>(0xCF)));
}

TEST(SystemInstructions, Iretd) {
    // IRETD: CF (32-bit, same opcode)
    EXPECT_EQ(iretd(), (internal::make_array<std::uint8_t>(0xCF)));
}

TEST(SystemInstructions, Iretq) {
    // IRETQ: 48 CF (64-bit, needs REX.W)
    EXPECT_EQ(iretq(), (internal::make_array<std::uint8_t>(0x48, 0xCF)));
}

// =============================================================================
// Privilege Level Instructions
// =============================================================================

TEST(SystemInstructions, Cli) {
    // CLI: FA (clear interrupt flag)
    EXPECT_EQ(cli(), (internal::make_array<std::uint8_t>(0xFA)));
}

TEST(SystemInstructions, Sti) {
    // STI: FB (set interrupt flag)
    EXPECT_EQ(sti(), (internal::make_array<std::uint8_t>(0xFB)));
}

TEST(SystemInstructions, Hlt) {
    // HLT: F4 (halt)
    EXPECT_EQ(hlt(), (internal::make_array<std::uint8_t>(0xF4)));
}

// =============================================================================
// CPU Information Instructions
// =============================================================================

TEST(SystemInstructions, Cpuid) {
    // CPUID: 0F A2
    EXPECT_EQ(cpuid(), (internal::make_array<std::uint8_t>(0x0F, 0xA2)));
}

TEST(SystemInstructions, Rdtsc) {
    // RDTSC: 0F 31
    EXPECT_EQ(rdtsc(), (internal::make_array<std::uint8_t>(0x0F, 0x31)));
}

TEST(SystemInstructions, Rdtscp) {
    // RDTSCP: 0F 01 F9
    EXPECT_EQ(rdtscp(), (internal::make_array<std::uint8_t>(0x0F, 0x01, 0xF9)));
}

// =============================================================================
// Convenience Instructions
// =============================================================================

TEST(SystemInstructions, JmpHere) {
    // JMP $ (infinite loop): EB FE
    // Using jmp(here) - the idiomatic way to express "jump to current instruction"
    EXPECT_EQ(jmp(here), (internal::make_array<std::uint8_t>(0xEB, 0xFE)));
}

// =============================================================================
// Constexpr Evaluation Tests
// =============================================================================

TEST(SystemInstructions, ConstexprEvaluation) {
    // Verify these can all be evaluated at compile time
    constexpr auto syscall_bytes = syscall_();
    constexpr auto sysenter_bytes = sysenter();
    constexpr auto int3_bytes = int3();
    constexpr auto int80_bytes = int_(0x80);
    constexpr auto iretq_bytes = iretq();
    constexpr auto cpuid_bytes = cpuid();
    constexpr auto rdtsc_bytes = rdtsc();
    constexpr auto jmp_here_bytes = jmp(here);

    static_assert(syscall_bytes[0] == 0x0F);
    static_assert(syscall_bytes[1] == 0x05);
    static_assert(int3_bytes[0] == 0xCC);
    static_assert(int80_bytes[0] == 0xCD);
    static_assert(int80_bytes[1] == 0x80);
    static_assert(jmp_here_bytes[0] == 0xEB);
    static_assert(jmp_here_bytes[1] == 0xFE);

    // Suppress unused variable warnings
    (void)sysenter_bytes;
    (void)iretq_bytes;
    (void)cpuid_bytes;
    (void)rdtsc_bytes;
}

// =============================================================================
// x86-32 Specific Encoding Tests
// =============================================================================

// Test 32-bit register encodings (no REX prefix)
TEST(X86_32Encoding, MovReg32) {
    // mov eax, ebx: 89 D8
    EXPECT_EQ(mov(eax, ebx), (internal::make_array<std::uint8_t>(0x89, 0xD8)));
}

TEST(X86_32Encoding, AddReg32) {
    // add ecx, edx: 01 D1
    EXPECT_EQ(add(ecx, edx), (internal::make_array<std::uint8_t>(0x01, 0xD1)));
}

TEST(X86_32Encoding, SubReg32Imm) {
    // sub esp, 0x20: 81 EC 20 00 00 00 (32-bit immediate form)
    // Note: Library uses the 32-bit immediate form for all immediate values
    EXPECT_EQ(sub(esp, 0x20), (internal::make_array<std::uint8_t>(0x81, 0xEC, 0x20, 0x00, 0x00, 0x00)));
}

TEST(X86_32Encoding, PushReg32) {
    // push ebp: 55
    EXPECT_EQ(push(ebp), (internal::make_array<std::uint8_t>(0x55)));
    // push eax: 50
    EXPECT_EQ(push(eax), (internal::make_array<std::uint8_t>(0x50)));
}

TEST(X86_32Encoding, PopReg32) {
    // pop ebp: 5D
    EXPECT_EQ(pop(ebp), (internal::make_array<std::uint8_t>(0x5D)));
    // pop eax: 58
    EXPECT_EQ(pop(eax), (internal::make_array<std::uint8_t>(0x58)));
}

TEST(X86_32Encoding, XorReg32Self) {
    // xor eax, eax: 31 C0 (common idiom to zero register)
    EXPECT_EQ(xor_(eax, eax), (internal::make_array<std::uint8_t>(0x31, 0xC0)));
}

TEST(X86_32Encoding, MovReg32Imm32) {
    // mov eax, 0xDEADBEEF: B8 EF BE AD DE (optimized B8+rd form)
    EXPECT_EQ(mov(eax, 0xDEADBEEF), (internal::make_array<std::uint8_t>(0xB8, 0xEF, 0xBE, 0xAD, 0xDE)));
}

TEST(X86_32Encoding, CallReg32) {
    // call eax: FF D0
    EXPECT_EQ(call(eax), (internal::make_array<std::uint8_t>(0xFF, 0xD0)));
}

TEST(X86_32Encoding, JmpReg32) {
    // jmp eax: FF E0
    EXPECT_EQ(jmp(eax), (internal::make_array<std::uint8_t>(0xFF, 0xE0)));
}

TEST(X86_32Encoding, IncDecReg32) {
    // In 64-bit mode, INC/DEC r32 use ModR/M form: FF /0 and FF /1
    // inc eax: FF C0
    EXPECT_EQ(inc(eax), (internal::make_array<std::uint8_t>(0xFF, 0xC0)));
    // dec ecx: FF C9
    EXPECT_EQ(dec(ecx), (internal::make_array<std::uint8_t>(0xFF, 0xC9)));
}

TEST(X86_32Encoding, ShiftReg32) {
    // shl eax, 1: C1 E0 01 (uses general imm8 form)
    // Note: Library uses C1 /4 ib encoding instead of optimized D1 /4 (shift by 1)
    EXPECT_EQ(shl(eax, 1), (internal::make_array<std::uint8_t>(0xC1, 0xE0, 0x01)));
    // shr ecx, 4: C1 E9 04
    EXPECT_EQ(shr(ecx, 4), (internal::make_array<std::uint8_t>(0xC1, 0xE9, 0x04)));
}

TEST(X86_32Encoding, TestReg32) {
    // test eax, eax: 85 C0
    EXPECT_EQ(test(eax, eax), (internal::make_array<std::uint8_t>(0x85, 0xC0)));
}

TEST(X86_32Encoding, CmpReg32Imm) {
    // cmp eax, 0: 81 F8 00 00 00 00 (32-bit immediate form)
    // Note: Library uses the 32-bit immediate form for all immediate values
    EXPECT_EQ(cmp(eax, 0), (internal::make_array<std::uint8_t>(0x81, 0xF8, 0x00, 0x00, 0x00, 0x00)));
}

// =============================================================================
// Common Shellcode Patterns (32-bit style)
// =============================================================================

TEST(ShellcodePatterns, LinuxSyscall32) {
    // Typical Linux 32-bit syscall pattern
    // mov eax, 1    ; syscall number (exit)
    // mov ebx, 0    ; exit code
    // int 0x80      ; trigger syscall
    constexpr auto shellcode = core::assemble(
        mov(eax, 1),
        mov(ebx, 0),
        int_0x80());

    // mov eax, 1: B8 01 00 00 00 (5 bytes, optimized B8+rd form)
    // mov ebx, 0: BB 00 00 00 00 (5 bytes, optimized B8+rd form)
    // int 0x80:   CD 80 (2 bytes)
    EXPECT_EQ(shellcode.size(), 12u);
    EXPECT_EQ(shellcode[0], 0xB8); // mov eax opcode
    EXPECT_EQ(shellcode[10], 0xCD); // int opcode
    EXPECT_EQ(shellcode[11], 0x80); // interrupt vector
}

TEST(ShellcodePatterns, LinuxSyscall64) {
    // Typical Linux 64-bit syscall pattern
    // mov rax, 60   ; syscall number (exit)
    // xor rdi, rdi  ; exit code 0
    // syscall
    constexpr auto shellcode = core::assemble(
        mov(rax, 60),
        xor_(rdi, rdi),
        syscall_());

    // mov rax, 60: 48 C7 C0 3C 00 00 00
    // xor rdi, rdi: 48 31 FF
    // syscall: 0F 05
    EXPECT_EQ(shellcode.size(), 12u);
    EXPECT_EQ(shellcode[10], 0x0F);
    EXPECT_EQ(shellcode[11], 0x05);
}

TEST(ShellcodePatterns, InfiniteLoop) {
    // Simple infinite loop: jmp $ using jmp(here)
    constexpr auto loop = jmp(here);

    // EB FE
    EXPECT_EQ(loop.size(), 2u);
    EXPECT_EQ(loop[0], 0xEB);
    EXPECT_EQ(loop[1], 0xFE);
}

TEST(ShellcodePatterns, NopSled) {
    // NOP sled followed by breakpoint
    constexpr auto sled = core::assemble(
        nop(),
        nop(),
        nop(),
        nop(),
        int3());

    // 90 90 90 90 CC
    EXPECT_EQ(sled.size(), 5u);
    EXPECT_EQ(sled[0], 0x90);
    EXPECT_EQ(sled[1], 0x90);
    EXPECT_EQ(sled[2], 0x90);
    EXPECT_EQ(sled[3], 0x90);
    EXPECT_EQ(sled[4], 0xCC);
}
