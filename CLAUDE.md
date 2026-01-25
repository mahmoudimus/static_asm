# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Configure (from repo root)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build -j$(nproc)

# Run all tests
ctest --test-dir build --output-on-failure

# Run a single test (by name pattern)
ctest --test-dir build -R "AdcInstructions" --output-on-failure

# Build with examples (requires Clang, uses inline assembly)
cmake -B build -DCMAKE_BUILD_TYPE=Release -DSTATIC_ASM_BUILD_EXAMPLES=ON
```

## Architecture

This is a **header-only C++20 library** for compile-time x86 assembly encoding. All code executes at compile time using `consteval` functions.

### Key Components

- **`include/static_asm.hpp`** - Main include file (amalgamated header)
- **`include/static_asm/core/assembler.hpp`** - `assemble()` function to concatenate instruction byte arrays
- **`include/static_asm/core/emitter.hpp`** - `emit()` for Clang inline assembly (Clang-only, requires -O2)

### x86 Encoding Layer (`include/static_asm/x86/`)

- **`operands.hpp`** - Register, immediate, and memory operand types with C++20 concepts
- **`encoder.hpp`** - Instruction encoding functions (`encode_alu`, `encode_mov`, `encode_jmp`, etc.)
- **`gen/instruction.g.hpp`** - High-level instruction functions (e.g., `add()`, `mov()`, `jmp()`)
- **`gen/instruction_db.g.hpp`** - Generated instruction database
- **`modrm.hpp`**, **`rex.hpp`**, **`sib.hpp`** - ModR/M, REX prefix, and SIB byte encoding

### Namespaces

```cpp
static_asm::core          // assemble(), emit()
static_asm::x86           // Low-level encoding, operand types
static_asm::x86::registers // rax, rbx, r8, r8d, etc.
static_asm::x86::instructions // add(), mov(), jmp(), etc.
```

### Usage Pattern

Instructions return `std::array<std::uint8_t, N>` with encoded bytes:

```cpp
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

constexpr auto code = core::assemble(
    mov(rax, rbx),
    add(rcx, 0x10),
    ret()
);
```

## Testing

Tests use Google Test and verify instruction encoding against expected byte sequences using `internal::make_array`. Test files are in `tests/test_*.cpp` (e.g., `test_alu.cpp`, `test_mov.cpp`).

## Adding New Instructions

1. Add to instruction database in `gen/instruction_db.g.hpp` (`instdb`, `prefix_db`, `prefix_0fdb` arrays)
2. Add encoder in `encoder.hpp` or extend existing encoder
3. Add instruction function in `gen/instruction.g.hpp`
4. Add tests
