// =============================================================================
// GENERATED FILE - DO NOT EDIT MANUALLY
// =============================================================================
// Source:    scripts/x86reference.xml
// MD5:       7f3fd7154e0809defc1a14de3d680bf8
// Generated: 2026-01-25T01:25:01Z
// =============================================================================
#pragma once

#include <array>
#include <utility>

#include "../instruction_db.hpp"

namespace static_asm::x86 {

    namespace internal {
        // Portable make_array implementation (replaces internal::make_array)
        template<typename T, typename... Args>
        inline constexpr auto make_array(Args&&... args) -> std::array<T, sizeof...(Args)> {
            return {{static_cast<T>(std::forward<Args>(args))...}};
        }
    }

#define INST_ENTRY(inst_id, prefix, prefix_0f, pri_opcode, sec_opcode, encoding, x) \
    instruction_desc(e_instruction_id::inst_id, prefix, prefix_0f, pri_opcode, sec_opcode, e_encoding::encoding, e_regopc_field::x)

    constexpr instructiondb instdb = internal::make_array<instructiondb::value_type>(
            INST_ENTRY(adc, 0, 0, 0x10, 0, alu, regrm),
            INST_ENTRY(add, 0, 0, 0x00, 0, alu, regrm),
            INST_ENTRY(and_, 0, 0, 0x20, 0, alu, regrm),
            INST_ENTRY(bsf, 0, 0x0f, 0xBC, 0, bitscan, regrm),   // 0F BC /r
            INST_ENTRY(bsr, 0, 0x0f, 0xBD, 0, bitscan, regrm),   // 0F BD /r
            INST_ENTRY(bswap, 0, 0x0f, 0xC8, 0, bswap, regrm),   // 0F C8+rd
            INST_ENTRY(bt, 0, 0x0f, 0xa3, 0, bt, regrm),
            INST_ENTRY(btc, 0, 0x0f, 0xba, 0, bt, regrm),
            INST_ENTRY(btr, 0, 0x0f, 0xb3, 0, bt, regrm),
            INST_ENTRY(bts, 0, 0x0f, 0xab, 0, bt, regrm),
            INST_ENTRY(call, 0, 0, 0xFF, 0, call, regrm),
            INST_ENTRY(cmova, 0, 0x0f, 0x47, 0, cmov, regrm),   // 0F 47: CMOVA/CMOVNBE
            INST_ENTRY(cmovae, 0, 0x0f, 0x43, 0, cmov, regrm),  // 0F 43: CMOVAE/CMOVNB/CMOVNC
            INST_ENTRY(cmovb, 0, 0x0f, 0x42, 0, cmov, regrm),   // 0F 42: CMOVB/CMOVC/CMOVNAE
            INST_ENTRY(cmovbe, 0, 0x0f, 0x46, 0, cmov, regrm),  // 0F 46: CMOVBE/CMOVNA
            INST_ENTRY(cmove, 0, 0x0f, 0x44, 0, cmov, regrm),   // 0F 44: CMOVE/CMOVZ
            INST_ENTRY(cmovg, 0, 0x0f, 0x4f, 0, cmov, regrm),   // 0F 4F: CMOVG/CMOVNLE
            INST_ENTRY(cmovge, 0, 0x0f, 0x4d, 0, cmov, regrm),  // 0F 4D: CMOVGE/CMOVNL
            INST_ENTRY(cmovl, 0, 0x0f, 0x4c, 0, cmov, regrm),   // 0F 4C: CMOVL/CMOVNGE
            INST_ENTRY(cmovle, 0, 0x0f, 0x4e, 0, cmov, regrm),  // 0F 4E: CMOVLE/CMOVNG
            INST_ENTRY(cmovno, 0, 0x0f, 0x41, 0, cmov, regrm),  // 0F 41: CMOVNO
            INST_ENTRY(cmovnp, 0, 0x0f, 0x4b, 0, cmov, regrm),  // 0F 4B: CMOVNP/CMOVPO
            INST_ENTRY(cmovns, 0, 0x0f, 0x49, 0, cmov, regrm),  // 0F 49: CMOVNS
            INST_ENTRY(cmovnz, 0, 0x0f, 0x45, 0, cmov, regrm),  // 0F 45: CMOVNE/CMOVNZ
            INST_ENTRY(cmovo, 0, 0x0f, 0x40, 0, cmov, regrm),   // 0F 40: CMOVO
            INST_ENTRY(cmovp, 0, 0x0f, 0x4a, 0, cmov, regrm),   // 0F 4A: CMOVP/CMOVPE
            INST_ENTRY(cmovs, 0, 0x0f, 0x48, 0, cmov, regrm),   // 0F 48: CMOVS
            INST_ENTRY(cmovz, 0, 0x0f, 0x44, 0, cmov, regrm),   // 0F 44: CMOVE/CMOVZ (alias)
            INST_ENTRY(cmp, 0, 0, 0x38, 0, alu, regrm),
            INST_ENTRY(dec, 0, 0, 0xFF, 1, unary, opcode_ext),   // FF /1 for 16/32/64-bit, FE /1 for 8-bit
            INST_ENTRY(div, 0, 0, 0xF7, 6, muldiv, opcode_ext),  // F7 /6 for 16/32/64-bit, F6 /6 for 8-bit
            INST_ENTRY(idiv, 0, 0, 0xF7, 7, muldiv, opcode_ext), // F7 /7 for 16/32/64-bit, F6 /7 for 8-bit
            INST_ENTRY(imul, 0, 0, 0xF7, 5, muldiv, opcode_ext), // F7 /5 for one-operand form
            INST_ENTRY(imul_two, 0, 0x0f, 0xAF, 0, imul_two_op, regrm), // 0F AF /r two-operand form
            INST_ENTRY(imul_three, 0, 0, 0x6B, 0, imul_three_op, regrm), // 6B /r ib (imm8) or 69 /r iw/id (imm16/32)
            INST_ENTRY(inc, 0, 0, 0xFF, 0, unary, opcode_ext),   // FF /0 for 16/32/64-bit, FE /0 for 8-bit
            INST_ENTRY(jb, 0, 0, 0x72, 0, jcc, regrm),
            INST_ENTRY(jbe, 0, 0, 0x76, 0, jcc, regrm),
            INST_ENTRY(jl, 0, 0, 0x7c, 0, jcc, regrm),
            INST_ENTRY(jle, 0, 0, 0x7e, 0, jcc, regrm),
            INST_ENTRY(jmp, 0, 0, 0xe9, 0, jmp, regrm),
            INST_ENTRY(jnb, 0, 0, 0x73, 0, jcc, regrm),
            INST_ENTRY(jnbe, 0, 0, 0x77, 0, jcc, regrm),
            INST_ENTRY(jnl, 0, 0, 0x7d, 0, jcc, regrm),
            INST_ENTRY(jnle, 0, 0, 0x7f, 0, jcc, regrm),
            INST_ENTRY(jno, 0, 0, 0x71, 0, jcc, regrm),
            INST_ENTRY(jnp, 0, 0, 0x7b, 0, jcc, regrm),
            INST_ENTRY(jns, 0, 0, 0x79, 0, jcc, regrm),
            INST_ENTRY(jnz, 0, 0, 0x75, 0, jcc, regrm),
            INST_ENTRY(jo, 0, 0, 0x70, 0, jcc, regrm),
            INST_ENTRY(jp, 0, 0, 0x7A, 0, jcc, regrm),
            INST_ENTRY(js, 0, 0, 0x78, 0, jcc, regrm),
            INST_ENTRY(jz, 0, 0, 0x74, 0, jcc, regrm),
            // Near jumps (rel32) - 0F 8x opcodes
            INST_ENTRY(jo_near, 0, 0x0f, 0x80, 0, jcc_near, regrm),
            INST_ENTRY(jno_near, 0, 0x0f, 0x81, 0, jcc_near, regrm),
            INST_ENTRY(jb_near, 0, 0x0f, 0x82, 0, jcc_near, regrm),
            INST_ENTRY(jnb_near, 0, 0x0f, 0x83, 0, jcc_near, regrm),
            INST_ENTRY(jz_near, 0, 0x0f, 0x84, 0, jcc_near, regrm),
            INST_ENTRY(jnz_near, 0, 0x0f, 0x85, 0, jcc_near, regrm),
            INST_ENTRY(jbe_near, 0, 0x0f, 0x86, 0, jcc_near, regrm),
            INST_ENTRY(jnbe_near, 0, 0x0f, 0x87, 0, jcc_near, regrm),
            INST_ENTRY(js_near, 0, 0x0f, 0x88, 0, jcc_near, regrm),
            INST_ENTRY(jns_near, 0, 0x0f, 0x89, 0, jcc_near, regrm),
            INST_ENTRY(jp_near, 0, 0x0f, 0x8a, 0, jcc_near, regrm),
            INST_ENTRY(jnp_near, 0, 0x0f, 0x8b, 0, jcc_near, regrm),
            INST_ENTRY(jl_near, 0, 0x0f, 0x8c, 0, jcc_near, regrm),
            INST_ENTRY(jnl_near, 0, 0x0f, 0x8d, 0, jcc_near, regrm),
            INST_ENTRY(jle_near, 0, 0x0f, 0x8e, 0, jcc_near, regrm),
            INST_ENTRY(jnle_near, 0, 0x0f, 0x8f, 0, jcc_near, regrm),
            INST_ENTRY(lea, 0, 0, 0x8D, 0, lea, regrm),          // LEA r, m
            INST_ENTRY(mov, 0, 0, 0x88, 0, mov, regrm),
            INST_ENTRY(movabs, 0, 0, 0xb8, 0, mov, regrm),
            INST_ENTRY(mul, 0, 0, 0xF7, 4, muldiv, opcode_ext),  // F7 /4 for 16/32/64-bit, F6 /4 for 8-bit
            INST_ENTRY(neg, 0, 0, 0xF7, 3, unary, opcode_ext),   // F7 /3 for 16/32/64-bit, F6 /3 for 8-bit
            INST_ENTRY(nop, 0, 0, 0x90, 0, noops, regrm),
            INST_ENTRY(not_, 0, 0, 0xF7, 2, unary, opcode_ext),  // F7 /2 for 16/32/64-bit, F6 /2 for 8-bit
            INST_ENTRY(or_, 0, 0, 0x08, 0, alu, regrm),
            INST_ENTRY(pop, 0, 0, 0x58, 0, pop, regrm),
            INST_ENTRY(push, 0, 0, 0x50, 0, push, regrm),
            INST_ENTRY(ret, 0, 0, 0xc3, 0, ret, none),
            INST_ENTRY(retf, 0, 0, 0xcb, 0, ret, none),
            INST_ENTRY(sbb, 0, 0, 0x18, 0, alu, regrm),
            INST_ENTRY(sub, 0, 0, 0x28, 0, alu, regrm),
            INST_ENTRY(test, 0, 0, 0x85, 0, test, regrm),        // 85 /r for reg-reg, F7 /0 for imm
            INST_ENTRY(ud2, 0, 0x0f, 0x0b, 0, noops, none),
            INST_ENTRY(xchg, 0, 0, 0x87, 0, xchg, regrm),        // 87 /r for reg-reg, 86 /r for 8-bit
            INST_ENTRY(xor_, 0, 0, 0x30, 0, alu, regrm),
            // Shift/rotate instructions - opcode extension in ModR/M reg field
            // D0/D1 (by 1), D2/D3 (by CL), C0/C1 (by imm8)
            // Extensions: 0=ROL, 1=ROR, 2=RCL, 3=RCR, 4=SHL/SAL, 5=SHR, 7=SAR
            INST_ENTRY(shl, 0, 0, 0xD1, 4, shift, opcode_ext),  // SHL/SAL r/m, 1/CL/imm8
            INST_ENTRY(shr, 0, 0, 0xD1, 5, shift, opcode_ext),  // SHR r/m, 1/CL/imm8
            INST_ENTRY(sal, 0, 0, 0xD1, 4, shift, opcode_ext),  // SAL (same as SHL)
            INST_ENTRY(sar, 0, 0, 0xD1, 7, shift, opcode_ext),  // SAR r/m, 1/CL/imm8
            INST_ENTRY(rol, 0, 0, 0xD1, 0, shift, opcode_ext),  // ROL r/m, 1/CL/imm8
            INST_ENTRY(ror, 0, 0, 0xD1, 1, shift, opcode_ext),  // ROR r/m, 1/CL/imm8
            INST_ENTRY(rcl, 0, 0, 0xD1, 2, shift, opcode_ext),  // RCL r/m, 1/CL/imm8
            INST_ENTRY(rcr, 0, 0, 0xD1, 3, shift, opcode_ext),  // RCR r/m, 1/CL/imm8

            // String instructions
            INST_ENTRY(movsb, 0, 0, 0xA4, 0, string, none),       // Move byte from [rsi] to [rdi]
            INST_ENTRY(movsw, 0x66, 0, 0xA5, 0, string, none),    // Move word from [rsi] to [rdi]
            INST_ENTRY(movsd, 0, 0, 0xA5, 0, string, none),       // Move dword from [rsi] to [rdi]
            INST_ENTRY(movsq, 0, 0, 0xA5, 0, string, none),       // Move qword from [rsi] to [rdi] (needs REX.W)
            INST_ENTRY(cmpsb, 0, 0, 0xA6, 0, string, none),       // Compare byte [rsi] with [rdi]
            INST_ENTRY(cmpsw, 0x66, 0, 0xA7, 0, string, none),    // Compare word [rsi] with [rdi]
            INST_ENTRY(cmpsd, 0, 0, 0xA7, 0, string, none),       // Compare dword [rsi] with [rdi]
            INST_ENTRY(cmpsq, 0, 0, 0xA7, 0, string, none),       // Compare qword [rsi] with [rdi] (needs REX.W)
            INST_ENTRY(scasb, 0, 0, 0xAE, 0, string, none),       // Scan byte in al with [rdi]
            INST_ENTRY(scasw, 0x66, 0, 0xAF, 0, string, none),    // Scan word in ax with [rdi]
            INST_ENTRY(scasd, 0, 0, 0xAF, 0, string, none),       // Scan dword in eax with [rdi]
            INST_ENTRY(scasq, 0, 0, 0xAF, 0, string, none),       // Scan qword in rax with [rdi] (needs REX.W)
            INST_ENTRY(lodsb, 0, 0, 0xAC, 0, string, none),       // Load byte from [rsi] into al
            INST_ENTRY(lodsw, 0x66, 0, 0xAD, 0, string, none),    // Load word from [rsi] into ax
            INST_ENTRY(lodsd, 0, 0, 0xAD, 0, string, none),       // Load dword from [rsi] into eax
            INST_ENTRY(lodsq, 0, 0, 0xAD, 0, string, none),       // Load qword from [rsi] into rax (needs REX.W)
            INST_ENTRY(stosb, 0, 0, 0xAA, 0, string, none),       // Store byte from al to [rdi]
            INST_ENTRY(stosw, 0x66, 0, 0xAB, 0, string, none),    // Store word from ax to [rdi]
            INST_ENTRY(stosd, 0, 0, 0xAB, 0, string, none),       // Store dword from eax to [rdi]
            INST_ENTRY(stosq, 0, 0, 0xAB, 0, string, none),       // Store qword from rax to [rdi] (needs REX.W)

            // Move with zero/sign extension
            INST_ENTRY(movzx, 0, 0x0f, 0xB6, 0, movzx_movsx, regrm),   // 0F B6/B7 - zero extend r/m8/16 to r16/32/64
            INST_ENTRY(movsx, 0, 0x0f, 0xBE, 0, movzx_movsx, regrm),   // 0F BE/BF - sign extend r/m8/16 to r16/32/64
            INST_ENTRY(movsxd, 0, 0, 0x63, 0, movsxd_enc, regrm),      // 63 /r - sign extend r/m32 to r64 (REX.W)

            // System instructions
            INST_ENTRY(syscall_, 0, 0x0f, 0x05, 0, noops, none),       // 0F 05 - fast system call (64-bit)
            INST_ENTRY(sysenter, 0, 0x0f, 0x34, 0, noops, none),       // 0F 34 - fast system call (32-bit)
            INST_ENTRY(sysexit, 0, 0x0f, 0x35, 0, noops, none),        // 0F 35 - fast return from system call
            INST_ENTRY(int3, 0, 0, 0xCC, 0, noops, none),              // CC - breakpoint
            INST_ENTRY(int_, 0, 0, 0xCD, 0, int_imm, none),            // CD ib - software interrupt
            INST_ENTRY(into, 0, 0, 0xCE, 0, noops, none),              // CE - interrupt on overflow (invalid in 64-bit)
            INST_ENTRY(iret, 0, 0, 0xCF, 0, noops, none),              // CF - interrupt return (16-bit)
            INST_ENTRY(iretd, 0, 0, 0xCF, 0, noops, none),             // CF - interrupt return (32-bit)
            INST_ENTRY(iretq, 0, 0, 0xCF, 0, noops, none),             // 48 CF - interrupt return (64-bit, needs REX.W)
            INST_ENTRY(cli, 0, 0, 0xFA, 0, noops, none),               // FA - clear interrupt flag
            INST_ENTRY(sti, 0, 0, 0xFB, 0, noops, none),               // FB - set interrupt flag
            INST_ENTRY(hlt, 0, 0, 0xF4, 0, noops, none),               // F4 - halt
            INST_ENTRY(cpuid, 0, 0x0f, 0xA2, 0, noops, none),          // 0F A2 - CPU identification
            INST_ENTRY(rdtsc, 0, 0x0f, 0x31, 0, noops, none),          // 0F 31 - read time-stamp counter
            INST_ENTRY(rdtscp, 0, 0x0f, 0x01, 0xF9, noops, none)       // 0F 01 F9 - read time-stamp counter and processor ID
    );

    template <e_instruction_id Id>
    inline constexpr instruction_desc find_instruction_desc() {
        constexpr int iid = static_cast<int>(Id) - 1;
        return (iid >= 0 && iid < instdb.size()) ? instdb[iid] : _ud2;
    }

#define HAS_PREFIX_ENTRY(inst_id, has_prefix) \
    has_prefix

    inline constexpr auto prefix_db = internal::make_array<bool>(
            HAS_PREFIX_ENTRY(adc, false),
            HAS_PREFIX_ENTRY(add, false),
            HAS_PREFIX_ENTRY(and_, false),
            HAS_PREFIX_ENTRY(bsf, false),
            HAS_PREFIX_ENTRY(bsr, false),
            HAS_PREFIX_ENTRY(bswap, false),
            HAS_PREFIX_ENTRY(bt, false),
            HAS_PREFIX_ENTRY(btc, false),
            HAS_PREFIX_ENTRY(btr, false),
            HAS_PREFIX_ENTRY(bts, false),
            HAS_PREFIX_ENTRY(call, false),
            HAS_PREFIX_ENTRY(cmova, false),
            HAS_PREFIX_ENTRY(cmovae, false),
            HAS_PREFIX_ENTRY(cmovb, false),
            HAS_PREFIX_ENTRY(cmovbe, false),
            HAS_PREFIX_ENTRY(cmove, false),
            HAS_PREFIX_ENTRY(cmovg, false),
            HAS_PREFIX_ENTRY(cmovge, false),
            HAS_PREFIX_ENTRY(cmovl, false),
            HAS_PREFIX_ENTRY(cmovle, false),
            HAS_PREFIX_ENTRY(cmovno, false),
            HAS_PREFIX_ENTRY(cmovnp, false),
            HAS_PREFIX_ENTRY(cmovns, false),
            HAS_PREFIX_ENTRY(cmovnz, false),
            HAS_PREFIX_ENTRY(cmovo, false),
            HAS_PREFIX_ENTRY(cmovp, false),
            HAS_PREFIX_ENTRY(cmovs, false),
            HAS_PREFIX_ENTRY(cmovz, false),
            HAS_PREFIX_ENTRY(cmp, false),
            HAS_PREFIX_ENTRY(dec, false),
            HAS_PREFIX_ENTRY(div, false),
            HAS_PREFIX_ENTRY(idiv, false),
            HAS_PREFIX_ENTRY(imul, false),
            HAS_PREFIX_ENTRY(imul_two, false),
            HAS_PREFIX_ENTRY(imul_three, false),
            HAS_PREFIX_ENTRY(inc, false),
            HAS_PREFIX_ENTRY(jb, false),
            HAS_PREFIX_ENTRY(jbe, false),
            HAS_PREFIX_ENTRY(jl, false),
            HAS_PREFIX_ENTRY(jle, false),
            HAS_PREFIX_ENTRY(jmp, false),
            HAS_PREFIX_ENTRY(jnb, false),
            HAS_PREFIX_ENTRY(jnbe, false),
            HAS_PREFIX_ENTRY(jnl, false),
            HAS_PREFIX_ENTRY(jnle, false),
            HAS_PREFIX_ENTRY(jno, false),
            HAS_PREFIX_ENTRY(jnp, false),
            HAS_PREFIX_ENTRY(jns, false),
            HAS_PREFIX_ENTRY(jnz, false),
            HAS_PREFIX_ENTRY(jo, false),
            HAS_PREFIX_ENTRY(jp, false),
            HAS_PREFIX_ENTRY(js, false),
            HAS_PREFIX_ENTRY(jz, false),
            // Near jumps (rel32)
            HAS_PREFIX_ENTRY(jo_near, false),
            HAS_PREFIX_ENTRY(jno_near, false),
            HAS_PREFIX_ENTRY(jb_near, false),
            HAS_PREFIX_ENTRY(jnb_near, false),
            HAS_PREFIX_ENTRY(jz_near, false),
            HAS_PREFIX_ENTRY(jnz_near, false),
            HAS_PREFIX_ENTRY(jbe_near, false),
            HAS_PREFIX_ENTRY(jnbe_near, false),
            HAS_PREFIX_ENTRY(js_near, false),
            HAS_PREFIX_ENTRY(jns_near, false),
            HAS_PREFIX_ENTRY(jp_near, false),
            HAS_PREFIX_ENTRY(jnp_near, false),
            HAS_PREFIX_ENTRY(jl_near, false),
            HAS_PREFIX_ENTRY(jnl_near, false),
            HAS_PREFIX_ENTRY(jle_near, false),
            HAS_PREFIX_ENTRY(jnle_near, false),
            HAS_PREFIX_ENTRY(lea, false),
            HAS_PREFIX_ENTRY(mov, false),
            HAS_PREFIX_ENTRY(movabs, false),
            HAS_PREFIX_ENTRY(mul, false),
            HAS_PREFIX_ENTRY(neg, false),
            HAS_PREFIX_ENTRY(nop, false),
            HAS_PREFIX_ENTRY(not_, false),
            HAS_PREFIX_ENTRY(or_, false),
            HAS_PREFIX_ENTRY(pop, false),
            HAS_PREFIX_ENTRY(push, false),
            HAS_PREFIX_ENTRY(ret, false),
            HAS_PREFIX_ENTRY(retf, false),
            HAS_PREFIX_ENTRY(sbb, false),
            HAS_PREFIX_ENTRY(sub, false),
            HAS_PREFIX_ENTRY(test, false),
            HAS_PREFIX_ENTRY(ud2, false),
            HAS_PREFIX_ENTRY(xchg, false),
            HAS_PREFIX_ENTRY(xor_, false),
            HAS_PREFIX_ENTRY(shl, false),
            HAS_PREFIX_ENTRY(shr, false),
            HAS_PREFIX_ENTRY(sal, false),
            HAS_PREFIX_ENTRY(sar, false),
            HAS_PREFIX_ENTRY(rol, false),
            HAS_PREFIX_ENTRY(ror, false),
            HAS_PREFIX_ENTRY(rcl, false),
            HAS_PREFIX_ENTRY(rcr, false),
            // String instructions
            HAS_PREFIX_ENTRY(movsb, false),
            HAS_PREFIX_ENTRY(movsw, true),   // 0x66 prefix for word
            HAS_PREFIX_ENTRY(movsd, false),
            HAS_PREFIX_ENTRY(movsq, false),
            HAS_PREFIX_ENTRY(cmpsb, false),
            HAS_PREFIX_ENTRY(cmpsw, true),   // 0x66 prefix for word
            HAS_PREFIX_ENTRY(cmpsd, false),
            HAS_PREFIX_ENTRY(cmpsq, false),
            HAS_PREFIX_ENTRY(scasb, false),
            HAS_PREFIX_ENTRY(scasw, true),   // 0x66 prefix for word
            HAS_PREFIX_ENTRY(scasd, false),
            HAS_PREFIX_ENTRY(scasq, false),
            HAS_PREFIX_ENTRY(lodsb, false),
            HAS_PREFIX_ENTRY(lodsw, true),   // 0x66 prefix for word
            HAS_PREFIX_ENTRY(lodsd, false),
            HAS_PREFIX_ENTRY(lodsq, false),
            HAS_PREFIX_ENTRY(stosb, false),
            HAS_PREFIX_ENTRY(stosw, true),   // 0x66 prefix for word
            HAS_PREFIX_ENTRY(stosd, false),
            HAS_PREFIX_ENTRY(stosq, false),
            // MOVZX/MOVSX/MOVSXD - 66 prefix handled dynamically for 16-bit destination
            HAS_PREFIX_ENTRY(movzx, false),
            HAS_PREFIX_ENTRY(movsx, false),
            HAS_PREFIX_ENTRY(movsxd, false),
            // System instructions
            HAS_PREFIX_ENTRY(syscall_, false),
            HAS_PREFIX_ENTRY(sysenter, false),
            HAS_PREFIX_ENTRY(sysexit, false),
            HAS_PREFIX_ENTRY(int3, false),
            HAS_PREFIX_ENTRY(int_, false),
            HAS_PREFIX_ENTRY(into, false),
            HAS_PREFIX_ENTRY(iret, false),
            HAS_PREFIX_ENTRY(iretd, false),
            HAS_PREFIX_ENTRY(iretq, false),
            HAS_PREFIX_ENTRY(cli, false),
            HAS_PREFIX_ENTRY(sti, false),
            HAS_PREFIX_ENTRY(hlt, false),
            HAS_PREFIX_ENTRY(cpuid, false),
            HAS_PREFIX_ENTRY(rdtsc, false),
            HAS_PREFIX_ENTRY(rdtscp, false)
    );

    template <e_instruction_id Id>
    inline constexpr bool has_prefix() {
        constexpr int iid = static_cast<int>(Id) - 1;
        return (iid >= 0 && iid < prefix_db.size()) ? prefix_db[iid] : false;
    }

#define HAS_PREFIX_0F_ENTRY(inst_id, has_prefix_0f) \
    has_prefix_0f

    inline constexpr auto prefix_0fdb = internal::make_array<bool>(
            HAS_PREFIX_0F_ENTRY(adc, false),
            HAS_PREFIX_0F_ENTRY(add, false),
            HAS_PREFIX_0F_ENTRY(and_, false),
            HAS_PREFIX_0F_ENTRY(bsf, true),
            HAS_PREFIX_0F_ENTRY(bsr, true),
            HAS_PREFIX_0F_ENTRY(bswap, true),
            HAS_PREFIX_0F_ENTRY(bt, true),
            HAS_PREFIX_0F_ENTRY(btc, true),
            HAS_PREFIX_0F_ENTRY(btr, true),
            HAS_PREFIX_0F_ENTRY(bts, true),
            HAS_PREFIX_0F_ENTRY(call, false),
            HAS_PREFIX_0F_ENTRY(cmova, true),
            HAS_PREFIX_0F_ENTRY(cmovae, true),
            HAS_PREFIX_0F_ENTRY(cmovb, true),
            HAS_PREFIX_0F_ENTRY(cmovbe, true),
            HAS_PREFIX_0F_ENTRY(cmove, true),
            HAS_PREFIX_0F_ENTRY(cmovg, true),
            HAS_PREFIX_0F_ENTRY(cmovge, true),
            HAS_PREFIX_0F_ENTRY(cmovl, true),
            HAS_PREFIX_0F_ENTRY(cmovle, true),
            HAS_PREFIX_0F_ENTRY(cmovno, true),
            HAS_PREFIX_0F_ENTRY(cmovnp, true),
            HAS_PREFIX_0F_ENTRY(cmovns, true),
            HAS_PREFIX_0F_ENTRY(cmovnz, true),
            HAS_PREFIX_0F_ENTRY(cmovo, true),
            HAS_PREFIX_0F_ENTRY(cmovp, true),
            HAS_PREFIX_0F_ENTRY(cmovs, true),
            HAS_PREFIX_0F_ENTRY(cmovz, true),
            HAS_PREFIX_0F_ENTRY(cmp, false),
            HAS_PREFIX_0F_ENTRY(dec, false),
            HAS_PREFIX_0F_ENTRY(div, false),
            HAS_PREFIX_0F_ENTRY(idiv, false),
            HAS_PREFIX_0F_ENTRY(imul, false),
            HAS_PREFIX_0F_ENTRY(imul_two, true),
            HAS_PREFIX_0F_ENTRY(imul_three, false),
            HAS_PREFIX_0F_ENTRY(inc, false),
            HAS_PREFIX_0F_ENTRY(jb, false),
            HAS_PREFIX_0F_ENTRY(jbe, false),
            HAS_PREFIX_0F_ENTRY(jl, false),
            HAS_PREFIX_0F_ENTRY(jle, false),
            HAS_PREFIX_0F_ENTRY(jmp, false),
            HAS_PREFIX_0F_ENTRY(jnb, false),
            HAS_PREFIX_0F_ENTRY(jnbe, false),
            HAS_PREFIX_0F_ENTRY(jnl, false),
            HAS_PREFIX_0F_ENTRY(jnle, false),
            HAS_PREFIX_0F_ENTRY(jno, false),
            HAS_PREFIX_0F_ENTRY(jnp, false),
            HAS_PREFIX_0F_ENTRY(jns, false),
            HAS_PREFIX_0F_ENTRY(jnz, false),
            HAS_PREFIX_0F_ENTRY(jo, false),
            HAS_PREFIX_0F_ENTRY(jp, false),
            HAS_PREFIX_0F_ENTRY(js, false),
            HAS_PREFIX_0F_ENTRY(jz, false),
            // Near jumps (rel32) - have 0F prefix
            HAS_PREFIX_0F_ENTRY(jo_near, true),
            HAS_PREFIX_0F_ENTRY(jno_near, true),
            HAS_PREFIX_0F_ENTRY(jb_near, true),
            HAS_PREFIX_0F_ENTRY(jnb_near, true),
            HAS_PREFIX_0F_ENTRY(jz_near, true),
            HAS_PREFIX_0F_ENTRY(jnz_near, true),
            HAS_PREFIX_0F_ENTRY(jbe_near, true),
            HAS_PREFIX_0F_ENTRY(jnbe_near, true),
            HAS_PREFIX_0F_ENTRY(js_near, true),
            HAS_PREFIX_0F_ENTRY(jns_near, true),
            HAS_PREFIX_0F_ENTRY(jp_near, true),
            HAS_PREFIX_0F_ENTRY(jnp_near, true),
            HAS_PREFIX_0F_ENTRY(jl_near, true),
            HAS_PREFIX_0F_ENTRY(jnl_near, true),
            HAS_PREFIX_0F_ENTRY(jle_near, true),
            HAS_PREFIX_0F_ENTRY(jnle_near, true),
            HAS_PREFIX_0F_ENTRY(lea, false),
            HAS_PREFIX_0F_ENTRY(mov, false),
            HAS_PREFIX_0F_ENTRY(movabs, false),
            HAS_PREFIX_0F_ENTRY(mul, false),
            HAS_PREFIX_0F_ENTRY(neg, false),
            HAS_PREFIX_0F_ENTRY(nop, false),
            HAS_PREFIX_0F_ENTRY(not_, false),
            HAS_PREFIX_0F_ENTRY(or_, false),
            HAS_PREFIX_0F_ENTRY(pop, false),
            HAS_PREFIX_0F_ENTRY(push, false),
            HAS_PREFIX_0F_ENTRY(ret, false),
            HAS_PREFIX_0F_ENTRY(retf, false),
            HAS_PREFIX_0F_ENTRY(sbb, false),
            HAS_PREFIX_0F_ENTRY(sub, false),
            HAS_PREFIX_0F_ENTRY(test, false),
            HAS_PREFIX_0F_ENTRY(ud2, true),
            HAS_PREFIX_0F_ENTRY(xchg, false),
            HAS_PREFIX_0F_ENTRY(xor_, false),
            HAS_PREFIX_0F_ENTRY(shl, false),
            HAS_PREFIX_0F_ENTRY(shr, false),
            HAS_PREFIX_0F_ENTRY(sal, false),
            HAS_PREFIX_0F_ENTRY(sar, false),
            HAS_PREFIX_0F_ENTRY(rol, false),
            HAS_PREFIX_0F_ENTRY(ror, false),
            HAS_PREFIX_0F_ENTRY(rcl, false),
            HAS_PREFIX_0F_ENTRY(rcr, false),
            // String instructions (none have 0F prefix)
            HAS_PREFIX_0F_ENTRY(movsb, false),
            HAS_PREFIX_0F_ENTRY(movsw, false),
            HAS_PREFIX_0F_ENTRY(movsd, false),
            HAS_PREFIX_0F_ENTRY(movsq, false),
            HAS_PREFIX_0F_ENTRY(cmpsb, false),
            HAS_PREFIX_0F_ENTRY(cmpsw, false),
            HAS_PREFIX_0F_ENTRY(cmpsd, false),
            HAS_PREFIX_0F_ENTRY(cmpsq, false),
            HAS_PREFIX_0F_ENTRY(scasb, false),
            HAS_PREFIX_0F_ENTRY(scasw, false),
            HAS_PREFIX_0F_ENTRY(scasd, false),
            HAS_PREFIX_0F_ENTRY(scasq, false),
            HAS_PREFIX_0F_ENTRY(lodsb, false),
            HAS_PREFIX_0F_ENTRY(lodsw, false),
            HAS_PREFIX_0F_ENTRY(lodsd, false),
            HAS_PREFIX_0F_ENTRY(lodsq, false),
            HAS_PREFIX_0F_ENTRY(stosb, false),
            HAS_PREFIX_0F_ENTRY(stosw, false),
            HAS_PREFIX_0F_ENTRY(stosd, false),
            HAS_PREFIX_0F_ENTRY(stosq, false),
            // MOVZX/MOVSX have 0F prefix, MOVSXD does not
            HAS_PREFIX_0F_ENTRY(movzx, true),
            HAS_PREFIX_0F_ENTRY(movsx, true),
            HAS_PREFIX_0F_ENTRY(movsxd, false),
            // System instructions
            HAS_PREFIX_0F_ENTRY(syscall_, true),   // 0F 05
            HAS_PREFIX_0F_ENTRY(sysenter, true),   // 0F 34
            HAS_PREFIX_0F_ENTRY(sysexit, true),    // 0F 35
            HAS_PREFIX_0F_ENTRY(int3, false),      // CC
            HAS_PREFIX_0F_ENTRY(int_, false),      // CD ib
            HAS_PREFIX_0F_ENTRY(into, false),      // CE
            HAS_PREFIX_0F_ENTRY(iret, false),      // CF
            HAS_PREFIX_0F_ENTRY(iretd, false),     // CF
            HAS_PREFIX_0F_ENTRY(iretq, false),     // 48 CF (REX.W prefix, not 0F)
            HAS_PREFIX_0F_ENTRY(cli, false),       // FA
            HAS_PREFIX_0F_ENTRY(sti, false),       // FB
            HAS_PREFIX_0F_ENTRY(hlt, false),       // F4
            HAS_PREFIX_0F_ENTRY(cpuid, true),      // 0F A2
            HAS_PREFIX_0F_ENTRY(rdtsc, true),      // 0F 31
            HAS_PREFIX_0F_ENTRY(rdtscp, true)      // 0F 01 F9
    );

    template <e_instruction_id Id>
    inline constexpr bool has_prefix_0f() {
        constexpr int iid = static_cast<int>(Id) - 1;
        return (iid >= 0 && iid < prefix_0fdb.size()) ? prefix_0fdb[iid] : false;
    }

}
