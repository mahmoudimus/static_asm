// static_asm - Compile-time x86/x86-64 assembler for C++20
// SPDX-License-Identifier: BSL-1.0 OR MIT

#ifndef STATIC_ASM_HPP
#define STATIC_ASM_HPP

// Core components
#include "static_asm/core.hpp"

// x86 architecture
#include "static_asm/x86/asm_block.hpp"
#include "static_asm/x86/encoder.hpp"
#include "static_asm/x86/gen/instruction.g.hpp"
#include "static_asm/x86/gen/instruction_db.g.hpp"
#include "static_asm/x86/instruction_db.hpp"
#include "static_asm/x86/modrm.hpp"
#include "static_asm/x86/opcode_extension.hpp"
#include "static_asm/x86/operands.hpp"
#include "static_asm/x86/rex.hpp"
#include "static_asm/x86/sib.hpp"

#endif // STATIC_ASM_HPP
