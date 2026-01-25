// static_asm - Compile-time x86/x86-64 assembler for C++20
// SPDX-License-Identifier: BSL-1.0 OR MIT

#ifndef STATIC_ASM_HPP
#define STATIC_ASM_HPP

// Core components
#include "../src/static_asm/core/assembler.hpp"
#include "../src/static_asm/core/emitter.hpp"

// x86 architecture
#include "../src/static_asm/x86/operands.hpp"
#include "../src/static_asm/x86/instruction_db.hpp"
#include "../src/static_asm/x86/gen/instruction_db.g.hpp"
#include "../src/static_asm/x86/opcode_extension.hpp"
#include "../src/static_asm/x86/rex.hpp"
#include "../src/static_asm/x86/modrm.hpp"
#include "../src/static_asm/x86/sib.hpp"
#include "../src/static_asm/x86/encoder.hpp"
#include "../src/static_asm/x86/gen/instruction.g.hpp"

#endif // STATIC_ASM_HPP
