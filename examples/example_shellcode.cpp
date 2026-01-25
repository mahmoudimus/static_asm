#include "static_asm.hpp"

#include <cstdint>
#include <iomanip>
#include <iostream>

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// Example: Windows x64 DLL loader shellcode
// This demonstrates building position-independent shellcode at compile time.
//
// The shellcode:
//   1. Sets up the Windows x64 ABI shadow space
//   2. Loads DllMain parameters into rcx, rdx, r8
//   3. Calls the entry point
//   4. Loops forever (for demonstration)

// For jmp $ (infinite loop): EB FE = jmp rel8 -2
// The library doesn't have short jmp yet, so we use raw bytes
constexpr std::array<std::uint8_t, 2> jmp_self = {0xEB, 0xFE};

// Helper to concatenate arrays at compile time
template<typename T, std::size_t N1, std::size_t N2>
constexpr std::array<T, N1 + N2> concat(const std::array<T, N1>& a, const std::array<T, N2>& b) {
    std::array<T, N1 + N2> result{};
    for (std::size_t i = 0; i < N1; ++i) result[i] = a[i];
    for (std::size_t i = 0; i < N2; ++i) result[N1 + i] = b[i];
    return result;
}

// Build the complete shellcode at compile time
constexpr auto build_loader_shellcode(std::uint64_t dll_base, std::uint64_t entry_point) {
    // All instructions use the library's native encoding
    // mov() automatically detects when movabs encoding is needed for 64-bit immediates
    auto code = core::assemble(
        sub(rsp, 0x28),              // sub rsp, 0x28 - shadow space for Windows x64 ABI
        mov(rcx, dll_base),          // mov rcx, dllBase (hinstDLL parameter) - auto uses movabs
        mov(rdx, 1),                 // mov rdx, 1 (DLL_PROCESS_ATTACH)
        xor_(r8, r8),                // xor r8, r8 (lpvReserved = NULL)
        mov(rax, entry_point),       // mov rax, entryPoint - auto uses movabs
        call(rax)                    // call rax
    );

    // Append jmp $ (infinite loop) - not yet supported natively
    return concat(code, jmp_self);
}

int main() {
    std::cout << "=== Compile-time Shellcode Builder ===" << std::endl;
    std::cout << std::endl;

    // Build with example addresses
    constexpr std::uint64_t example_dll_base = 0x00007FF700000000;
    constexpr std::uint64_t example_entry = 0x00007FF700001000;

    constexpr auto shellcode = build_loader_shellcode(example_dll_base, example_entry);

    std::cout << "Shellcode (" << shellcode.size() << " bytes):" << std::endl;
    std::cout << std::hex << std::setfill('0');

    // Print as C array
    std::cout << "const uint8_t shellcode[] = {" << std::endl << "    ";
    for (size_t i = 0; i < shellcode.size(); ++i) {
        std::cout << "0x" << std::setw(2) << static_cast<int>(shellcode[i]);
        if (i < shellcode.size() - 1) std::cout << ", ";
        if ((i + 1) % 12 == 0 && i < shellcode.size() - 1) std::cout << std::endl << "    ";
    }
    std::cout << std::endl << "};" << std::endl;

    std::cout << std::dec << std::endl;
    std::cout << "Disassembly:" << std::endl;
    std::cout << "  sub rsp, 0x28              ; shadow space" << std::endl;
    std::cout << "  mov rcx, 0x" << std::hex << example_dll_base << "  ; hinstDLL (movabs auto-detected)" << std::endl;
    std::cout << "  mov rdx, 1                 ; DLL_PROCESS_ATTACH" << std::endl;
    std::cout << "  xor r8, r8                 ; lpvReserved = NULL" << std::endl;
    std::cout << "  mov rax, 0x" << example_entry << "  ; entryPoint (movabs auto-detected)" << std::endl;
    std::cout << "  call rax                   ; call DllMain" << std::endl;
    std::cout << "  jmp $                      ; infinite loop" << std::dec << std::endl;

    std::cout << std::endl;
    std::cout << "Offset table for runtime patching:" << std::endl;
    std::cout << "  dllBase (rcx imm64):    offset 9, 8 bytes" << std::endl;
    std::cout << "  entryPoint (rax imm64): offset 29, 8 bytes" << std::endl;

    return 0;
}
