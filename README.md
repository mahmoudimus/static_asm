# static_asm

[![CI](https://github.com/mahmoudimus/static_asm/actions/workflows/ci.yml/badge.svg)](https://github.com/mahmoudimus/static_asm/actions/workflows/ci.yml)
[![License](https://img.shields.io/badge/License-BSL_1.0_OR_MIT-blue.svg)](LICENSE)

A header-only C++20 library for compile-time x86/x86-64 instruction encoding.

This project is dual-licensed under the [Boost Software License 1.0](https://www.boost.org/LICENSE_1_0.txt) and the [MIT License](https://opensource.org/licenses/MIT). You may choose either license.

## Purpose

* Learning x86 assembly encoding in a practical and fun way
* Generating assembly instructions at compile time with full type safety
* Creating shellcode and JIT code templates without runtime overhead

## Quick Start

```cpp
#include "static_asm.hpp"

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// Build machine code at compile time
constexpr auto code = core::assemble(
    mov(rax, 0x12345678),    // mov rax, imm32
    add(rax, rcx),           // add rax, rcx
    xor_(r8, r8),            // xor r8, r8
    call(rax),               // call rax
    ret()                    // ret
);
// code is std::array<uint8_t, N> - fully constexpr!
```

For named branches and embedded data, see [Labels and Branch Relaxation](#labels-and-branch-relaxation).

## Examples by Category

### ALU Operations

```cpp
// Register to register
add(rax, rbx);           // 48 01 D8
sub(ecx, edx);           // 29 D1
and_(r8, r9);            // 4D 21 C8
or_(rsi, rdi);           // 48 09 FE
xor_(eax, eax);          // 31 C0 (common idiom to zero a register)
cmp(rax, rcx);           // 48 39 C8

// Register with immediate
add(rax, 0x10);          // 48 83 C0 10 (sign-extended imm8)
add(rax, 0x10000);       // 48 05 00 00 01 00 (imm32)
sub(ecx, 100);           // 83 E9 64
and_(rdx, 0xFF);         // 48 83 E2 FF

// With memory operands
add(eax, dword_ptr(rbx));                        // 03 03
add(rax, qword_ptr(rcx + disp<0x10>));           // 48 03 41 10
sub(dword_ptr(rsp + disp<0x20>), eax);           // 29 44 24 20
```

> **Use `disp<value>` for the shortest displacement.** It chooses no displacement
> for zero where legal, a signed `disp8` for -128 through 127, and `disp32`
> otherwise. RIP-relative addresses always need a `disp32`; RBP/R13 with zero
> still need a zero `disp8`. Values outside signed 32 bits fail compilation.
> A plain integer such as `rcx + 0x10` still uses `disp32`: C++ cannot select
> different exact-size instruction array types from the value of an `int`
> argument.
> `[base + disp]` with a general base uses the canonical ModR/M-only form (no
> SIB byte); RSP/R12 bases emit a SIB byte because the hardware requires it.

### Data Movement

```cpp
// Register to register
mov(rax, rbx);           // 48 89 D8
mov(eax, ecx);           // 89 C8
mov(r8, r9);             // 4D 89 C8

// Immediate to register
mov(rax, 0x12345678);    // 48 C7 C0 78 56 34 12
mov(eax, 0xDEADBEEF);    // B8 EF BE AD DE

// Memory operations
mov(rax, qword_ptr(rbx));                        // 48 8B 03
mov(eax, dword_ptr(rcx + disp<0x10>));           // 8B 41 10
mov(rax, qword_ptr(rbp - disp<0x20>));           // 48 8B 45 E0 (canonical, no SIB)
mov(qword_ptr(rsp + disp<0x8>), rax);            // 48 89 44 24 08 (RSP base needs SIB)

// Zero/sign extension
movzx(eax, bl);          // 0F B6 C3 (zero-extend byte to dword)
movzx(rax, bx);          // 48 0F B7 C3 (zero-extend word to qword)
movsx(eax, cl);          // 0F BE C1 (sign-extend byte to dword)
movsx(rax, dx);          // 48 0F BF C2 (sign-extend word to qword)
movsxd(rax, ecx);        // 48 63 C1 (sign-extend dword to qword)

// Load effective address
lea(rax, qword_ptr(rbx + rcx * s4));                      // 48 8D 04 8B
lea(rax, qword_ptr(rbx + rcx * s8 + disp<0x10>));         // 48 8D 44 CB 10

// Exchange (register or memory; XCHG is symmetric)
xchg(rax, rbx);                                  // 48 87 D8
xchg(qword_ptr(rcx), rax);                       // 48 87 01
xchg(rbx, qword_ptr(rcx + disp<0x10>));          // 48 87 59 10

// Store an immediate to a displaced / SIB / RIP memory destination
mov(dword_ptr(rbp - disp<0x20>), 0x100);         // C7 45 E0 00 01 00 00
mov(qword_ptr(rcx + disp<0x10>), 1);             // 48 C7 41 10 01 00 00 00
add(qword_ptr(rbx + disp<0x8>), 0x10);           // 48 81 43 08 10 00 00 00
```

A qword memory destination has only a sign-extended 32-bit immediate encoding.
An integer's 64-bit bit pattern must match that encoding: for example, `0x7FFFFFFF`
and `-0x80000000LL` fit, while positive `0x80000000ULL` does not. An invalid
value fails constant evaluation or throws `std::out_of_range` at runtime.
With exceptions disabled, an invalid runtime value terminates. Explicit
`imm32` operands supply the raw four immediate bytes.

### RIP-Relative Addressing

`rip + disp` (and `rip - disp`) encode the 64-bit RIP-relative form (ModR/M
`mod=00 r/m=101`, a mandatory `disp32`, never a SIB byte):

```cpp
mov(rax, qword_ptr(rip + 0x10));   // 48 8B 05 10 00 00 00
lea(rax, qword_ptr(rip + 0x100));  // 48 8D 05 00 01 00 00
add(qword_ptr(rip + 0x20), rbx);   // 48 01 1D 20 00 00 00
inc(qword_ptr(rip + 0x40));        // 48 FF 05 40 00 00 00
test(rax, qword_ptr(rip + 0x10)); // 48 85 05 10 00 00 00
test(qword_ptr(rip + 0), 0x10);   // 48 F7 05 00 00 00 00 10 00 00 00
```

### SIB Addressing (Scale-Index-Base)

```cpp
// [base + index*scale]
mov(eax, dword_ptr(rbx + rcx * s1));   // 8B 04 0B
mov(eax, dword_ptr(rbx + rcx * s2));   // 8B 04 4B
mov(eax, dword_ptr(rbx + rcx * s4));   // 8B 04 8B
mov(eax, dword_ptr(rbx + rcx * s8));   // 8B 04 CB

// [base + index*scale + displacement]
mov(rax, qword_ptr(rbx + rcx * s4 + disp<0x10>));         // 48 8B 44 8B 10
mov(rax, qword_ptr(r12 + r13 * s8 + 0x1000));             // 4B 8B 84 EC 00 10 00 00

// Store to SIB address
mov(dword_ptr(rax + rdx * s4), ecx);                      // 89 0C 90
mov(qword_ptr(rbx + rsi * s8 + disp<0x20>), rax);         // 48 89 44 F3 20

// LEA with SIB (useful for address calculations)
lea(rax, qword_ptr(rbx + rcx * s4));                      // 48 8D 04 8B
lea(rax, qword_ptr(rdi + rsi * s8 + 0x100));              // 48 8D 84 F7 00 01 00 00
```

### Shift and Rotate

```cpp
// Shift by 1
shl(eax, 1);             // D1 E0
shr(rax, 1);             // 48 D1 E8
sar(ecx, 1);             // D1 F9

// Shift by immediate
shl(eax, 4);             // C1 E0 04
shr(rax, 8);             // 48 C1 E8 08
sar(rdx, 16);            // 48 C1 FA 10

// Shift by CL register
shl(eax, cl);            // D3 E0
shr(rax, cl);            // 48 D3 E8

// Rotate
rol(eax, 1);             // D1 C0
ror(rax, 8);             // 48 C1 C8 08
rcl(ecx, cl);            // D3 D1
rcr(rdx, 1);             // 48 D1 DA
```

### Multiply and Divide

```cpp
// Single operand (result in rdx:rax)
mul(rbx);                // 48 F7 E3 (unsigned: rdx:rax = rax * rbx)
imul(rcx);               // 48 F7 E9 (signed: rdx:rax = rax * rcx)
div(rbx);                // 48 F7 F3 (unsigned: rax = rdx:rax / rbx, rdx = remainder)
idiv(rcx);               // 48 F7 F9 (signed division)

// Two-operand IMUL (dest = dest * src)
imul(rax, rbx);          // 48 0F AF C3
imul(ecx, edx);          // 0F AF CA

// Three-operand IMUL (dest = src * imm)
imul(rax, rbx, 10);      // 48 6B C3 0A
imul(ecx, edx, 1000);    // 69 CA E8 03 00 00

// Single-operand form with a memory operand
mul(qword_ptr(rcx));                             // 48 F7 21
imul(qword_ptr(rcx + disp<0x20>));               // 48 F7 69 20
div(dword_ptr(rax + disp<0x10>));                // F7 70 10
idiv(qword_ptr(rbx));                            // 48 F7 3B
```

### Increment / Decrement / Negate / Not

```cpp
// Register operands
inc(rax);                // 48 FF C0
dec(ecx);                // FF C9
neg(rbx);                // 48 F7 DB
not_(rdx);               // 48 F7 D2

// Memory operands (canonical ModR/M-only addressing)
inc(qword_ptr(rcx));                             // 48 FF 01
inc(qword_ptr(rcx + disp<0x20>));                // 48 FF 41 20
dec(dword_ptr(rax + disp<0x10>));                // FF 48 10
neg(qword_ptr(rbx));                             // 48 F7 1B
not_(qword_ptr(rbx - disp<0x4>));                // 48 F7 53 FC
```

### LOCK Prefix

`lock_` prepends the `LOCK` prefix (`0xF0`) to any encoded instruction, making a
read-modify-write on memory atomic:

```cpp
lock_(inc(qword_ptr(rcx + disp<0x20>)));         // F0 48 FF 41 20
lock_(add(dword_ptr(rax), ecx));                 // F0 01 08
```

It is a thin byte-level wrapper and does not validate that the wrapped
instruction is one the CPU permits `LOCK` on.

### Control Flow

```cpp
// Unconditional jumps
jmp(0x10);               // EB 10 (short, 8-bit offset)
jmp(0x1000);             // E9 00 10 00 00 (near, 32-bit offset)
jmp(rax);                // FF E0 (indirect)
jmp(here);               // EB FE (jmp $, infinite loop)

// Conditional jumps (8-bit offset)
jz(0x10);                // 74 10
jnz(0x20);               // 75 20
jb(0x08);                // 72 08 (below/carry)
jae(0x08);               // 73 08 (above or equal/no carry)
jl(0x10);                // 7C 10 (less than, signed)
jge(0x10);               // 7D 10 (greater or equal, signed)

// Conditional jumps (32-bit offset for longer branches)
jz_near(0x10000);        // 0F 84 00 00 01 00
jnz_near(0x20000);       // 0F 85 00 00 02 00

// Call and return
call(rax);               // FF D0 (indirect call)
call(0x100);             // E8 00 01 00 00 (relative call)
ret();                   // C3
ret(0x10);               // C2 10 00 (return and pop 16 bytes)
```

### Labels and Branch Relaxation

A named label groups the bytes that begin at its location. Pass the resulting
fragment to `core::assemble()` alongside ordinary encoded instructions. The
outer call resolves label references and chooses rel8 or rel32 for each branch.

```cpp
constexpr auto loop = label<"loop">;
constexpr auto code = core::assemble(
    loop.assemble(
        dec(ecx),
        jne(loop)
    )
);
// code == { FF, C9, 75, FC }
```

`loop.assemble(...)` creates a symbolic fragment. It does not finalize bytes;
references across fragments need the complete layout. The outer
`core::assemble()` returns an exact-size compile-time code object with the
same `size()`, indexing, and byte-array comparison behavior as an array.

Several labels can be passed to one outer call. Each `label.assemble(...)`
marks the start of its fragment; the parentheses keep the bytes belonging to
that label together in the source:

```cpp
constexpr auto loop = label<"loop">;
constexpr auto done = label<"done">;
constexpr auto data = label<"data">;

constexpr auto code = core::assemble(
    mov(ecx, 3),

    loop.assemble(
        dec(ecx),
        jne(loop),
        jmp(done)
    ),

    data.assemble(
        dq(42)
    ),

    done.assemble(
        lea(rax, qword_ptr(data)),
        ret()
    )
);
```

All labels share one program layout, even when fragments are nested. The
closing `)` ends the source grouping; it does not create a control-flow
boundary. `code.offset_of(label)` gives a final byte offset for runtime patch
sites:

```cpp
constexpr auto ctx = label<"ctx">;
constexpr auto code = core::assemble(
    nop(),
    ctx.assemble(
        movabs(rax, 0)
    )
);
constexpr std::size_t imm_slot = code.offset_of(ctx) + 2; // 3
```

Forward references work across fragments. Branch relaxation repeats until
every branch width is stable; `call(label)` always uses rel32.

`db`/`dw`/`dd`/`dq` produce raw little-endian byte arrays. Passing a
label to a sized memory helper creates a RIP-relative reference to that label:

```cpp
constexpr auto data = label<"data">;
constexpr auto code = core::assemble(
    lea(rax, qword_ptr(data)),
    ret(),
    data.assemble(
        dq(0xCAFEBABE)
    )
);
// 48 8D 05 01 00 00 00  C3  BE BA FE CA 00 00 00 00
```

RIP-relative labeled memory works for every currently exposed scalar instruction
family with an explicit memory operand. Literal `rip + displacement` addresses
use the same encoders. The supported forms are:

| Family | Labeled memory form |
| --- | --- |
| MOV, LEA, XCHG, TEST; ADD/ADC/SUB/SBB/CMP, AND/OR/XOR | Register/memory and memory/immediate forms where the instruction permits them |
| INC/DEC/NEG/NOT; MUL/IMUL/DIV/IDIV | Unary memory form |
| BSF/BSR; CMOVcc; MOVZX/MOVSX/MOVSXD | Register destination, memory source |
| BT/BTC/BTR/BTS | Memory destination, register or imm8 bit index |
| IMUL | Two-operand register/memory and three-operand register/memory/immediate forms |
| SHL/SHR/SAL/SAR/ROL/ROR/RCL/RCR | Memory destination, implicit count 1, CL, or imm8 |
| CALL/JMP | Indirect near branch through a qword memory operand |
| PUSH/POP | Word or qword memory operand |

For example, the previously missing bit-scan and conditional-move forms now
resolve a data label in the same `core::assemble()` call:

```cpp
constexpr auto data = label<"word">;
constexpr auto code = core::assemble(
    bsf(rax, qword_ptr(data)),
    cmovz(r8, qword_ptr(data)),
    data.assemble(dq(42))
);
```

Register-only instructions, relative branches, and the `nop<Len>()` padding
API do not take an explicit memory address in this interface.

A trailing immediate needs no manual displacement-tail length:

```cpp
constexpr auto data = label<"counter">;
constexpr auto code = core::assemble(
    mov(dword_ptr(data), 0x7B),
    test(qword_ptr(data), 0x10),
    ret(),
    data.assemble(
        dq(0)
    )
);
```

Each referenced label needs exactly one `label.assemble(...)` definition.
Missing or duplicate definitions fail at compile time.

`call(label)` and `jmp(label)` branch directly to code at the label.
`call(qword_ptr(label))` and `jmp(qword_ptr(label))` instead load a 64-bit
target address from memory at the label. These are different machine
instructions.

### Conditional Moves

```cpp
// Move if condition is true (no branch penalty!)
cmovz(rax, rbx);         // 48 0F 44 C3 (move if zero)
cmovnz(eax, ecx);        // 0F 45 C1 (move if not zero)
cmovl(rax, rdx);         // 48 0F 4C C2 (move if less, signed)
cmovge(ecx, esi);        // 0F 4D CE (move if greater or equal, signed)
cmovb(rax, rbx);         // 48 0F 42 C3 (move if below, unsigned)
cmovae(edx, edi);        // 0F 43 D7 (move if above or equal, unsigned)

// With memory source
cmovz(rax, qword_ptr(rbx));           // 48 0F 44 03
cmovnz(eax, dword_ptr(rcx + 0x10));   // 0F 45 41 10
```

### Bit Operations

```cpp
// Bit test
bt(eax, 5);              // 0F BA E0 05
bt(rax, rbx);            // 48 0F A3 D8

// Bit test and set/reset/complement
bts(eax, 10);            // 0F BA E8 0A (test and set)
btr(rax, rcx);           // 48 0F B3 C8 (test and reset)
btc(edx, 3);             // 0F BA FA 03 (test and complement)

// Bit scan
bsf(eax, ecx);           // 0F BC C1 (scan forward for first 1)
bsr(rax, rbx);           // 48 0F BD C3 (scan reverse for first 1)

// Population count and leading/trailing zeros
popcnt(eax, ecx);        // F3 0F B8 C1
lzcnt(rax, rbx);         // F3 48 0F BD C3
tzcnt(eax, edx);         // F3 0F BC C2

// Byte swap
bswap(eax);              // 0F C8 (reverse byte order)
bswap(rax);              // 48 0F C8
```

### String Operations

```cpp
// Basic string ops (operate on [rsi] and/or [rdi])
movsb();                 // A4 (move byte [rsi] -> [rdi])
movsw();                 // 66 A5
movsd();                 // A5
movsq();                 // 48 A5

cmpsb();                 // A6 (compare [rsi] with [rdi])
stosb();                 // AA (store al -> [rdi])
lodsb();                 // AC (load [rsi] -> al)
scasb();                 // AE (compare al with [rdi])

// With REP prefix (repeat rcx times)
rep_movsb();             // F3 A4 (memcpy)
rep_movsq();             // F3 48 A5 (fast memcpy, 8 bytes at a time)
rep_stosb();             // F3 AA (memset)
rep_stosq();             // F3 48 AB

// With REPE/REPNE (repeat while equal/not equal)
repe_cmpsb();            // F3 A6 (compare strings until mismatch)
repne_scasb();           // F2 AE (scan for byte in string)
```

### Stack Operations

```cpp
push(rax);               // 50
push(rbx);               // 53
push(r8);                // 41 50
push(0x10);              // 6A 10 (push imm8)
push(0x1000);            // 68 00 10 00 00 (push imm32)

pop(rax);                // 58
pop(rbx);                // 5B
pop(r15);                // 41 5F
```

### System Instructions

```cpp
// System calls
syscall_();              // 0F 05 (64-bit syscall)
sysenter();              // 0F 34
sysexit();               // 0F 35

// Interrupts
int3();                  // CC (breakpoint)
int_(0x80);              // CD 80 (Linux 32-bit syscall)
int_(0x21);              // CD 21 (DOS interrupt)

// CPU info
cpuid();                 // 0F A2
rdtsc();                 // 0F 31
rdtscp();                // 0F 01 F9

// Privilege
cli();                   // FA (clear interrupts)
sti();                   // FB (set interrupts)
hlt();                   // F4 (halt)

// Interrupt return
iret();                  // CF (16-bit)
iretd();                 // CF (32-bit)
iretq();                 // 48 CF (64-bit)
```

## Assembling Multiple Instructions

Use `core::assemble()` to concatenate instruction byte arrays:

```cpp
constexpr auto prologue = core::assemble(
    push(rbp),
    mov(rbp, rsp),
    sub(rsp, 0x20)
);

constexpr auto epilogue = core::assemble(
    add(rsp, 0x20),
    pop(rbp),
    ret()
);

// Combine them
constexpr auto full_function = core::assemble(prologue, epilogue);
```

## Installation

### Option 1: CMake FetchContent (Recommended)

Add to your `CMakeLists.txt`:

```cmake
include(FetchContent)
FetchContent_Declare(
    static_asm
    GIT_REPOSITORY https://github.com/mahmoudimus/static_asm.git
    GIT_TAG v0.1.0  # latest published release; update to v0.2.0 after release
)
FetchContent_MakeAvailable(static_asm)

target_link_libraries(your_target PRIVATE static_asm::static_asm)
```

### Option 2: CMake add_subdirectory

Clone or add as a git submodule:

```bash
git submodule add https://github.com/mahmoudimus/static_asm.git external/static_asm
```

Then in your `CMakeLists.txt`:

```cmake
add_subdirectory(external/static_asm)
target_link_libraries(your_target PRIVATE static_asm::static_asm)
```

When included via `add_subdirectory` or `FetchContent`, only the `static_asm::static_asm` interface library target is added to your project. Tests and examples are not built unless explicitly enabled with `-DSTATIC_ASM_BUILD_TESTS=ON`.

### Option 3: Single Header

Download `static_asm.hpp` from the [releases page](https://github.com/mahmoudimus/static_asm/releases) and include it directly:

```cpp
#include "static_asm.hpp"
```

### Option 4: System Install

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --install build --prefix /usr/local
```

Then use `find_package`:

```cmake
find_package(static_asm REQUIRED)
target_link_libraries(your_target PRIVATE static_asm::static_asm)
```

## Build

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Run tests
ctest --test-dir build --output-on-failure

# Build with examples (Clang only, uses inline assembly)
cmake -B build -DCMAKE_BUILD_TYPE=Release -DSTATIC_ASM_BUILD_EXAMPLES=ON
```

**Platform support:**
- Linux (GCC 11+, Clang 14+)
- macOS (Apple Clang, Clang)
- Windows (MSVC 2022+)

Note: The `core::emit()` inline assembly feature requires GCC or Clang targeting
x86 or x86-64 with -O2 optimization. Other targets can still build instruction
byte arrays, but cannot place x86 bytes in their instruction stream.

## Supported Instructions

| Category | Instructions |
|----------|-------------|
| ALU | ADD, ADC, SUB, SBB, AND, OR, XOR, CMP, TEST |
| Unary | INC, DEC, NEG, NOT |
| Multiply/Divide | MUL, IMUL (1/2/3 operand forms), DIV, IDIV |
| Data Movement | MOV, MOVABS, MOVZX, MOVSX, MOVSXD, LEA, XCHG (reg and memory), PUSH, POP |
| Shift/Rotate | SHL, SHR, SAL, SAR, ROL, ROR, RCL, RCR |
| Control Flow | JMP, CALL, RET, RETF |
| Conditional Jumps | JZ/JE, JNZ/JNE, JB/JC, JNB/JNC, JBE/JNA, JNBE/JA, JL, JNL, JLE, JNLE, JO, JNO, JS, JNS, JP, JNP (8-bit and 32-bit offsets) |
| Conditional Moves | CMOVA, CMOVAE, CMOVB, CMOVBE, CMOVE, CMOVG, CMOVGE, CMOVL, CMOVLE, CMOVNE, CMOVNO, CMOVNP, CMOVNS, CMOVO, CMOVP, CMOVS |
| Bit Operations | BT, BTC, BTR, BTS |
| Bit Scan/Count | BSF, BSR, POPCNT, LZCNT, TZCNT, BSWAP |
| String Operations | MOVSB/W/D/Q, CMPSB/W/D/Q, LODSB/W/D/Q, STOSB/W/D/Q, SCASB/W/D/Q (with REP/REPE/REPNE prefixes) |
| System | SYSCALL, SYSENTER, SYSEXIT, INT, INT3, IRET/D/Q, CLI, STI, HLT, CPUID, RDTSC, RDTSCP |
| Misc | NOP, UD2 |
| Prefixes | LOCK (`lock_`), REP/REPE/REPNE (string ops) |
| Assembler layer | `label<"name">`, `label.assemble(...)`, and `core::assemble(...)`: branch relaxation (rel8/rel32), patch offsets, `db`/`dw`/`dd`/`dq` data, and RIP-relative labeled memory operands |

**Operand support:**
- All 8/16/32/64-bit general purpose registers (AL-R15)
- Extended registers (R8-R15, R8D-R15D, R8W-R15W, R8B-R15B)
- Immediate values (8/16/32/64-bit)
- Memory operands: `[reg]`, `[reg +/- disp]` (canonical ModR/M-only), `[disp32]` (absolute), `[rip +/- disp32]`
- SIB addressing: `[base + index*scale + disp]` with scale factors 1, 2, 4, 8
- Memory destinations for the unary (INC/DEC/NEG/NOT) and MUL/IMUL/DIV/IDIV groups
- Memory destination + immediate for base/displaced/SIB/RIP addressing (MOV and the ALU group)

**Note**: No SIMD/AVX extensions yet.

## Roadmap / What's Left

Known gaps:

- **No SIMD/AVX/VEX/EVEX**, x87, or segment-override prefixes.

## Developing

### Environment Setup

This project uses [uv](https://docs.astral.sh/uv/) for Python tooling (code generation, single-header amalgamation).

```bash
# Install uv (if not already installed)
curl -LsSf https://astral.sh/uv/install.sh | sh

# Verify installation
uv --version

# All Python scripts can be run directly with uv (dependencies are auto-managed)
uv run scripts/gen_from_x86ref.py --help
uv run scripts/amalgamate.sh
```

**Required tools:**

| Tool | Purpose | Install |
|------|---------|---------|
| [uv](https://docs.astral.sh/uv/) | Python package/project manager | `curl -LsSf https://astral.sh/uv/install.sh \| sh` |
| [quom](https://github.com/Viatorus/quom) | Single-header amalgamation | `uv tool install quom` |
| CMake 3.19+ | Build system | [cmake.org](https://cmake.org/download/) |
| C++20 compiler | GCC 11+, Clang 14+, MSVC 2022+ | - |

**Optional tools for development:**

| Tool | Purpose | Install |
|------|---------|---------|
| clang-format | Code formatting | Via LLVM or system package |
| clang-tidy | Static analysis | Via LLVM or system package |

### Adding New Instructions

#### Option 1: Use the Code Generator

The project includes a generator that parses the [x86reference](https://github.com/mazegen/x86reference) XML database:

```bash
# Show instruction database summary
uv run scripts/gen_from_x86ref.py

# Show details for a specific instruction
uv run scripts/gen_from_x86ref.py -i lea
uv run scripts/gen_from_x86ref.py -i imul

# Generate instruction database files
uv run scripts/gen_from_x86ref.py --generate-db

# Generate exhaustive test file
uv run scripts/gen_from_x86ref.py --generate-tests
```

#### Option 2: Manual Addition

1. Add to the instruction db file (`instdb`, `prefix_db`, `prefix_0fdb` arrays)
2. Code the encoder in `encoder.hpp` or extend an existing encoder
3. Add tests

### Techniques for Creating a Single-Header Library

See [the single-header library guide](docs/single-header.md) for the
source-level rules and examples used when preparing an amalgamation.

## Credits

- [Godbolt](https://godbolt.org/) for prototyping
- Intel x86-64 [manual](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-instruction-set-reference-manual-325383.pdf)
- [Defuse.ca](https://defuse.ca/online-x86-assembler.htm#disassembly) for verifying outputs
- [Geek ABC reference](http://ref.x86asm.net/geek-abc.html)
- [mazegen/x86reference](https://github.com/mazegen/x86reference) for the instruction database XML

## Acknowledgments

This project is based on [cx_assembler](https://github.com/Midi12/cx_assembler) by [Midi12](https://github.com/Midi12). The original library provided the foundation for compile-time x86 assembly encoding in C++.
