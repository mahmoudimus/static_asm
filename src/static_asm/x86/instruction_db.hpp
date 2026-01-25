#pragma once

#include <array>
#include <cstdint>

namespace static_asm::x86 {

    enum class e_instruction_id {
        unknown,
        adc,
        add,
        and_,
        bsf,
        bsr,
        bswap,
        bt,
        btc,
        btr,
        bts,
        call,
        cmova,
        cmovae,
        cmovb,
        cmovbe,
        cmove,
        cmovg,
        cmovge,
        cmovl,
        cmovle,
        cmovno,
        cmovnp,
        cmovns,
        cmovnz,
        cmovo,
        cmovp,
        cmovs,
        cmovz,
        cmp,
        dec,
        div,
        idiv,
        imul,
        imul_two, // Two-operand IMUL: r = r * r/m (0F AF /r)
        imul_three, // Three-operand IMUL: r = r/m * imm (6B /r ib or 69 /r iw/id)
        inc,
        jb,
        jbe,
        jl,
        jle,
        jmp,
        jnb,
        jnbe,
        jnl,
        jnle,
        jno,
        jnp,
        jns,
        jnz,
        jo,
        jp,
        js,
        jz,
        // Near jumps (rel32) - 0F 8x opcodes
        jo_near,
        jno_near,
        jb_near,
        jnb_near,
        jz_near,
        jnz_near,
        jbe_near,
        jnbe_near,
        js_near,
        jns_near,
        jp_near,
        jnp_near,
        jl_near,
        jnl_near,
        jle_near,
        jnle_near,
        lea,
        mov,
        movabs,
        mul,
        neg,
        nop,
        not_,
        or_,
        pop,
        push,
        ret,
        retf,
        sbb,
        sub,
        test,
        ud2,
        xchg,
        xor_,
        // Shift/rotate instructions
        shl,
        shr,
        sal,
        sar,
        rol,
        ror,
        rcl,
        rcr,

        // String instructions
        movsb,
        movsw,
        movsd,
        movsq,
        cmpsb,
        cmpsw,
        cmpsd,
        cmpsq,
        scasb,
        scasw,
        scasd,
        scasq,
        lodsb,
        lodsw,
        lodsd,
        lodsq,
        stosb,
        stosw,
        stosd,
        stosq,

        // Move with zero/sign extension
        movzx,
        movsx,
        movsxd,

        // System instructions
        syscall_,
        sysenter,
        sysexit,
        int3,
        int_,
        into,
        iret,
        iretd,
        iretq,
        cli,
        sti,
        hlt,
        cpuid,
        rdtsc,
        rdtscp,

        //----
        count
    };

    // Encoding hint
    enum class e_encoding {
        alu,
        bitscan, // BSF, BSR - two operand with 0F prefix
        bswap, // BSWAP - single operand, opcode+rd
        bt,
        call,
        cmov, // CMOV - conditional move
        jcc,
        jcc_near, // Near conditional jumps with rel32 offset (0F 8x opcodes)
        jmp,
        lea,
        mov,
        muldiv, // MUL, DIV, IMUL (one-operand), IDIV - single operand with opcode extension
        imul_two_op, // IMUL r, r/m (two-operand form)
        imul_three_op, // IMUL r, r/m, imm (three-operand form)
        noops,
        pop,
        push,
        ret,
        shift, // SHL, SHR, SAL, SAR, ROL, ROR, RCL, RCR - shift/rotate with opcode extension
        string, // String instructions (MOVS, CMPS, SCAS, LODS, STOS)
        test,
        unary, // INC, DEC, NEG, NOT - single operand with opcode extension
        xchg,
        movzx_movsx, // MOVZX, MOVSX - move with zero/sign extension
        movsxd_enc, // MOVSXD - sign extend dword to qword
        int_imm // INT imm8 - software interrupt
    };

    // Register/ Opcode Field
    enum class e_regopc_field {
        none,
        opcode_ext, // opcode_ext indicates the value of the opcode extension to be assigned to the ModR/M reg bits (values from 0 through 7).
        regrm // regrm indicates that the ModR/M byte contains a register operand and an r/m operand.
    };

    class instruction_desc {
    public:
        constexpr instruction_desc(
            e_instruction_id id,
            std::uint8_t prefix,
            std::uint8_t prefix_0f,
            std::uint8_t primary_opcode,
            std::uint8_t secondary_opcode,
            e_encoding encoding,
            e_regopc_field regopc_field) : _id(id),
                                           _prefix(prefix),
                                           _prefix_0f(prefix_0f),
                                           _primary_opcode(primary_opcode),
                                           _secondary_opcode(secondary_opcode),
                                           _encoding(encoding),
                                           _regopc_field(regopc_field) {}

        constexpr e_instruction_id id() const {
            return _id;
        }
        constexpr std::uint8_t prefix() const {
            return _prefix;
        }
        constexpr std::uint8_t prefix_0f() const {
            return _prefix_0f;
        }
        constexpr std::uint8_t primary_opcode() const {
            return _primary_opcode;
        }
        constexpr std::uint8_t secondary_opcode() const {
            return _secondary_opcode;
        }
        constexpr e_encoding encoding() const {
            return _encoding;
        }
        constexpr e_regopc_field regopc_field() const {
            return _regopc_field;
        }

    private:
        e_instruction_id _id;
        std::uint8_t _prefix;
        std::uint8_t _prefix_0f;
        std::uint8_t _primary_opcode;
        std::uint8_t _secondary_opcode;
        e_encoding _encoding;
        e_regopc_field _regopc_field;
    };

    // ud2 instruction description
    constexpr instruction_desc _ud2(e_instruction_id::unknown, 0x0, 0x0f, 0x0b, 0x0, e_encoding::noops, e_regopc_field::none);

    using instructiondb = std::array<instruction_desc, static_cast<int>(e_instruction_id::count) - 1>;
} // namespace static_asm::x86