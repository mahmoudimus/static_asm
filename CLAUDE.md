# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Configure (from repo root); tests build by default when top-level
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build -j$(nproc)

# Run all tests (includes the clang-format `format-check` test if clang-format is installed)
ctest --test-dir build --output-on-failure

# Run a single test suite (GoogleTest suite name, e.g. TEST(AdcInstructions, ...))
ctest --test-dir build -R "AdcInstructions" --output-on-failure

# Format check only
ctest --test-dir build -R format-check --output-on-failure

# Build with examples (requires Clang, uses inline assembly)
cmake -B build -DCMAKE_BUILD_TYPE=Release -DSTATIC_ASM_BUILD_EXAMPLES=ON

# Single-header amalgamation -> single_include/static_asm.hpp (requires `quom`)
scripts/amalgamate.sh
```

Tests compile with `-O2 -Werror` (GCC/Clang) or `/WX` (MSVC), so new warnings break the build. CI runs GCC 13/14, Clang 17/18, Apple Clang, MSVC, clang-cl and MinGW in Release and Debug, plus clang-format-19 and clang-tidy-19.

## Architecture

Header-only C++20 library for compile-time x86/x86-64 instruction encoding. Instruction functions are `constexpr` and return `std::array<std::uint8_t, N>`; `core::assemble()` concatenates them at compile time.

- **`include/static_asm.hpp`** - umbrella header that includes everything
- **`include/static_asm/core.hpp`** - `assemble()` and `emit()`. `emit()` injects bytes via GCC-style inline asm (Clang/GCC with -O2); on MSVC it is a stub because MSVC has no x64 inline asm.

### x86 Encoding Layer (`include/static_asm/x86/`)

- **`operands.hpp`** - register, immediate, and memory operand types (`qword_ptr(rbx + rcx * s4 + 0x10)`), C++20 concepts, and the `registers` namespace
- **`encoder.hpp`** - encoding functions (`encode_alu`, `encode_mov`, `encode_jmp`, etc.)
- **`modrm.hpp`**, **`rex.hpp`**, **`sib.hpp`**, **`opcode_extension.hpp`** - byte-level encoding helpers
- **`instruction_db.hpp`** - instruction DB types/enum
- **`gen/instruction_db.g.hpp`** - instruction DB data, generated
- **`gen/instruction.g.hpp`** - user-facing instruction functions (`add()`, `mov()`, `jmp()`)

Instruction names that collide with C++ keywords or common macros take a trailing underscore: `and_`, `or_`, `xor_`, `not_`, `int_`, `syscall_`, `cmpsd_`, `movsd_`.

### Namespaces

```cpp
static_asm::core               // assemble(), emit()
static_asm::x86                // low-level encoding, operand types
static_asm::x86::registers     // rax, rbx, r8, r8d, etc.
static_asm::x86::instructions  // add(), mov(), jmp(), etc.
```

### Code generation

`scripts/gen_from_x86ref.py` (run with `uv run`) reads `scripts/x86reference.xml`:

- `--generate-db` writes `gen/instruction_db.g.hpp` and `x86/instruction_db.gen.hpp` (note: not `instruction_db.hpp`, which is the checked-in version)
- `--generate-tests` overwrites `tests/test_generated.cpp`
- `-i <mnemonic>` shows the XML details for one instruction

`gen/instruction.g.hpp` carries a "GENERATED - DO NOT EDIT" banner, but the script does not write it; it is maintained by hand.

## Testing

GoogleTest (fetched via FetchContent). Tests assert encoded bytes against `internal::make_array<std::uint8_t>(...)`.

- Test sources are listed explicitly in `TEST_SOURCES` in `CMakeLists.txt`. A new `tests/test_*.cpp` file is not built until it is added there (`tests/test_displacement.cpp` is currently not listed).
- `tests/main.cpp` and `tests/tests_*.cpp` are legacy Catch2 tests from the original project and are not built.

## Adding New Instructions

1. Add the DB entry in `gen/instruction_db.g.hpp` (`instdb`, `prefix_db`, `prefix_0fdb` arrays), or regenerate it with the script
2. Add or extend an encoder in `encoder.hpp`
3. Add the instruction function in `gen/instruction.g.hpp`
4. Add tests to a `tests/test_*.cpp` file listed in `TEST_SOURCES`
