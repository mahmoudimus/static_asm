#pragma once

#include <array>
#include <tuple>
#include <utility>

#include "gen/instruction_db.g.hpp"
#include "instruction_db.hpp"
#include "modrm.hpp"
#include "opcode_extension.hpp"
#include "operands.hpp"
#include "rex.hpp"
#include "sib.hpp"

namespace static_asm::x86 {

    namespace internal {

        // Note: make_array is defined in instruction_db.g.hpp

        constexpr std::uint8_t prefix_16bit = 0x66;
        constexpr std::uint8_t prefix_overridesize = 0x67;

        template<typename... Ts>
        struct _sizeof {
            constexpr static std::size_t value = (sizeof(Ts) + ...);
        };

        template<typename... Ts>
        inline constexpr auto _sizeof_v = _sizeof<Ts...>::value;

        template<e_instruction_id Id, typename Op1, typename Op2>
        struct _sizeof_prefixes {
            constexpr static std::size_t value =
                has_prefix<Id>() +
                has_prefix_0f<Id>() +
                ((Register16<Op1> || (!IsVoidOperand<Op2> && Register16<Op2>)) || (Memory<Op1> && Op1::size == 16) ? 1 : 0) +
                ((Memory<Op1> && Register32<typename Op1::value_type>) || (!IsVoidOperand<Op2> && Memory<Op2> && Register32<typename Op2::value_type>) ? 1 : 0);
        };

        template<e_instruction_id Id, typename Op1, typename Op2>
        inline constexpr auto _sizeof_prefixes_v = _sizeof_prefixes<Id, Op1, Op2>::value;

        template<typename... Bytes>
        constexpr std::array<std::uint8_t, sizeof...(Bytes)> make_bytes(Bytes&&... args) noexcept {
            return { std::uint8_t(std::forward<Bytes>(args))... };
        }

        template<e_instruction_id Id, typename Op1, typename Op2, typename... Args>
            requires DerivesBaseOperand<Op1> && (DerivesBaseOperand<Op2> || IsVoidOperand<Op2>)
        inline constexpr auto encode([[maybe_unused]] instruction_desc desc, Args&&... args) {
            auto tuple = std::make_tuple(std::forward<Args>(args)...);

            auto has_rex = []() constexpr {
                if constexpr (Immediate<Op2> || IsVoidOperand<Op2>) {
                    if constexpr (Id == e_instruction_id::call || Id == e_instruction_id::jmp) {
                        // For call/jmp, only need REX for extended registers (REX.B)
                        if constexpr (Register<Op1>) {
                            return needs_rex_extended<Op1>();
                        } else {
                            return false;
                        }
                    } else {
                        return needs_rex<Op1>();
                    }
                } else {
                    return needs_rex<Op1, Op2>();
                }
            };

            constexpr auto size = has_rex() + _sizeof_prefixes_v<Id, Op1, Op2> + _sizeof_v<Args...>;
            std::array<std::uint8_t, size> arr{};

            int j = 0;

            if constexpr ((Memory<Op1> && Register32<typename Op1::value_type>) || (!IsVoidOperand<Op2> && Memory<Op2> && Register32<typename Op2::value_type>)) {
                arr[j++] = prefix_overridesize;
            }

            if constexpr ((Register16<Op1> || (!IsVoidOperand<Op2> && Register16<Op2>)) || (Memory<Op1> && Op1::size == 16)) {
                arr[j++] = prefix_16bit;
            }

            if constexpr (has_rex()) {
                if constexpr (Immediate<Op2> || IsVoidOperand<Op2>) {
                    if constexpr ((Id == e_instruction_id::call || Id == e_instruction_id::jmp) && Register<Op1>) {
                        // For call/jmp with register, only set REX.B (no REX.W)
                        arr[j++] = encode_rex_b_only<Op1>();
                    } else {
                        arr[j++] = encode_rex<Op1>();
                    }
                } else {
                    arr[j++] = encode_rex<Op1, Op2>();
                }
            }

            if constexpr (has_prefix<Id>()) {
                arr[j++] = desc.prefix();
            }

            if constexpr (has_prefix_0f<Id>()) {
                arr[j++] = desc.prefix_0f();
            }

            auto emit = []<typename T>(std::array<std::uint8_t, size>& arr, int& j, const T& value) {
                for (std::size_t i = 0; i < sizeof(T); i++) {
                    arr[j++] = static_cast<std::uint8_t>(value >> (i * 8));
                }
            };

            std::apply([&arr, &j, &emit](auto&&... args) {
                ((emit(arr, j, args)), ...);
            },
                tuple);

            return arr;
        }
    } // namespace internal

    template<typename Op1, typename Op2>
        requires(Register<Op1> || Memory<Op1>) && (Register<Op2> || Memory<Op2>)
    inline constexpr auto encode_opcode_alu(const std::uint8_t& opcode) {
        return static_cast<std::uint8_t>(
            (opcode & 0b11111100) +
            (Memory<Op2> ? 0b10 : 0b00) + // setup bit `d`
            (Op1::size > 8 ? 0b01 : 0b00) // setup bit `s`
        );
    }

    template<typename Op1, typename Op2>
        requires(Register<Op1> || Memory<Op1>) && (Immediate<Op2>)
    inline constexpr auto encode_opcode_alu(const std::uint8_t& opcode) {
        return static_cast<std::uint8_t>(
            (opcode & 0b11111100) +
            (false ? 0b10 : 0b00) + // setup bit `x` : todo : change to handle onebyte sign extend (1), constant same size as operand (0)
            (Op2::size > 8 ? 0b01 : 0b00) // setup bit `s`
        );
    }

    // Overload for SIBMemory operands
    template<typename Op1, typename Op2>
        requires(Register<Op1>) && (SIBMemory<Op2>)
    inline constexpr auto encode_opcode_alu(const std::uint8_t& opcode) {
        // SIBMemory as source: d=1 (reg is destination), s depends on operand size
        return static_cast<std::uint8_t>(
            (opcode & 0b11111100) +
            0b10 + // d=1: reg is destination
            (Op1::size > 8 ? 0b01 : 0b00) // setup bit `s`
        );
    }

    template<typename Op1, typename Op2>
        requires(SIBMemory<Op1>) && (Register<Op2>)
    inline constexpr auto encode_opcode_alu(const std::uint8_t& opcode) {
        // SIBMemory as destination: d=0 (r/m is destination), s depends on operand size
        return static_cast<std::uint8_t>(
            (opcode & 0b11111100) +
            0b00 + // d=0: r/m is destination
            (Op2::size > 8 ? 0b01 : 0b00) // setup bit `s`
        );
    }

    template<typename Reg>
        requires Register<Reg>
    inline constexpr auto encode_opcode_pushpop(const std::uint8_t& opcode, [[maybe_unused]] const Reg& reg) {
        // Only use lower 3 bits of register ID; REX.B handles bit 3 for extended registers
        return static_cast<std::uint8_t>(opcode + (static_cast<std::uint8_t>(Reg::id()) & 0x07));
    }

    template<e_instruction_id Id, typename Op1, typename Op2>
        requires(Register<Op1> || Memory<Op1>) && (Register<Op2> || Memory<Op2> || Immediate<Op2>)
    inline constexpr auto encode_alu([[maybe_unused]] instruction_desc desc, [[maybe_unused]] Op1 op1, [[maybe_unused]] Op2 op2) {
        auto id = desc.id();
        auto opcode = desc.primary_opcode();

        if constexpr (Immediate<Op2>) {
            // Immediate src operands use the 80h opcode
            opcode = encode_opcode_alu<Op1, Op2>(0x80);
            typename Op2::value_type value = static_cast<typename Op2::value_type>(op2.value());

            return internal::encode<Id, Op1, Op2>(desc, opcode, encode_modrm(opcodeext_alu(id), op1, op2), value);
        } else {
            // src operand is Register or Memory
            opcode = encode_opcode_alu<Op1, Op2>(opcode);

            if constexpr (Memory<Op1> && Immediate<typename Op1::value_type>) {
                return internal::encode<Id, Op1, Op2>(
                    desc,
                    opcode,
                    encode_modrm(op1, op2),
                    encode_sib_nodisp(op1, op2),
                    static_cast<std::uint32_t>(op1.value().value()));
            } else if constexpr (Memory<Op2> && Immediate<typename Op2::value_type>) {
                return internal::encode<Id, Op1, Op2>(
                    desc,
                    opcode,
                    encode_modrm(op1, op2),
                    encode_sib_nodisp(op1, op2),
                    static_cast<std::uint32_t>(op2.value().value()));
            } else {
                return internal::encode<Id, Op1, Op2>(
                    desc,
                    opcode,
                    encode_modrm(op1, op2));
            }
        }
    }

    template<e_instruction_id Id, typename Op1, typename Op2>
        requires Integer<Op2>
    inline constexpr auto encode_alu([[maybe_unused]] instruction_desc desc, const Op1& op1, const Op2& op2) {
        return encode_alu<Id>(desc, op1, immediate<typename truncate_as<Op1>::type>(op2));
    }

    // BSF/BSR - bit scan forward/reverse
    // Encoding: 0F BC /r (BSF) or 0F BD /r (BSR)
    // Format: BSF/BSR r16/32/64, r/m16/32/64
    // Note: Op1 (destination) goes in REG field, Op2 (source) goes in R/M field
    // encode_modrm(reg1, reg2) puts reg2 in REG and reg1 in R/M, so we swap the order
    template<e_instruction_id Id, typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto encode_bitscan([[maybe_unused]] instruction_desc desc, const Op1& op1, const Op2& op2) {
        auto opcode = desc.primary_opcode(); // 0xBC for BSF, 0xBD for BSR

        // Swap operand order: op2 (source) in r/m, op1 (dest) in reg
        return internal::encode<Id, Op2, Op1>(desc, opcode, encode_modrm(op2, op1));
    }

    // BSWAP - byte swap
    // Encoding: 0F C8+rd (32-bit), REX.W + 0F C8+rd (64-bit)
    // Format: BSWAP r32/64
    template<e_instruction_id Id, typename Op1>
        requires Register<Op1> && (Op1::size == 32 || Op1::size == 64)
    inline constexpr auto encode_bswap([[maybe_unused]] instruction_desc desc, [[maybe_unused]] const Op1& op1) {
        auto base_opcode = desc.primary_opcode(); // 0xC8
        // Lower 3 bits of register ID are encoded in opcode
        auto opcode = static_cast<std::uint8_t>(base_opcode + (static_cast<std::uint8_t>(Op1::id()) & 0x07));

        if constexpr (needs_rex<Op1>()) {
            return internal::make_array<std::uint8_t>(encode_rex<Op1>(), 0x0F, opcode);
        } else if constexpr (Op1::extended) {
            // Extended register (r8d-r15d) needs REX.B
            return internal::make_array<std::uint8_t>(encode_rex_b_only<Op1>(), 0x0F, opcode);
        } else {
            return internal::make_array<std::uint8_t>(0x0F, opcode);
        }
    }

    template<typename Op1, typename Op2>
        requires(Register<Op1> || Memory<Op1>) && (Register<Op2> || Memory<Op2> || Immediate<Op2>)
    inline constexpr auto encode_opcode_bt(const std::uint8_t& opcode) {
        return static_cast<std::uint8_t>(
            (opcode & 0b11111110) +
            (!Immediate<Op2> ? 0b01 : 0b00) // setup bit `s`
        );
    }

    template<e_instruction_id Id, typename Op1, typename Op2>
        requires(Register<Op1> || Memory<Op1>) && (Register<Op2> || Memory<Op2> || Immediate<Op2>)
    inline constexpr auto encode_bt([[maybe_unused]] instruction_desc desc, [[maybe_unused]] Op1 op1, [[maybe_unused]] Op2 op2) {
        auto id = desc.id();
        auto opcode = desc.primary_opcode();

        if constexpr (Immediate<Op2>) {
            // Immediate src use 0xBA opcode
            opcode = 0xBA;

            return internal::encode<Id, Op1, Op2>(
                desc, opcode, encode_modrm(opcodeext_bt(id), op1, op2), op2.value());
        } else {
            opcode = encode_opcode_bt<Op1, Op2>(opcode);

            return internal::encode<Id, Op1, Op2>(
                desc, opcode, encode_modrm(op1, op2));
        }
    }

    template<e_instruction_id Id, typename Op1, typename Op2>
        requires Integer<Op2>
    inline constexpr auto encode_bt([[maybe_unused]] instruction_desc desc, const Op1& op1, const Op2& op2) {
        return encode_bt<Id>(desc, op1, immediate<std::uint8_t>(op2));
    }

    template<e_instruction_id Id, typename Op1>
    inline constexpr auto encode_call([[maybe_unused]] instruction_desc desc, const Op1& op1) {
        auto id = desc.id();
        auto opcode = desc.primary_opcode();

        if constexpr (Immediate<Op1>) {
            opcode = 0xE8;

            return internal::encode<Id, Op1, Void>(
                desc, opcode, op1.value());
        } else {
            return internal::encode<Id, Op1, Void>(
                desc, opcode, encode_modrm(opcodeext_ff(id), op1));
        }
    }

    template<e_instruction_id Id, typename Op1>
        requires Integer<Op1>
    inline constexpr auto encode_call([[maybe_unused]] instruction_desc desc, const Op1& op1) {
        return encode_call<Id>(desc, immediate<std::uint32_t>(op1));
    }

    template<e_instruction_id Id, typename Op1>
        requires Immediate8<Op1>
    inline constexpr auto encode_jcc([[maybe_unused]] instruction_desc desc, Op1 op1) {
        auto opcode = desc.primary_opcode();

        return internal::encode<Id, Op1, Void>(desc, opcode, op1.value());
    }

    // Near conditional jump encoder (rel32 offset, 0F 8x opcodes)
    template<e_instruction_id Id, typename Op1>
        requires std::same_as<Op1, imm32>
    inline constexpr auto encode_jcc_near([[maybe_unused]] instruction_desc desc, Op1 op1) {
        auto opcode = desc.primary_opcode();

        return internal::encode<Id, Op1, Void>(desc, opcode, op1.value());
    }

    template<e_instruction_id Id, typename Op1>
        requires(Immediate<Op1> || Register<Op1> || (Memory<Op1> && Register<typename Op1::value_type>))
    inline constexpr auto encode_jmp([[maybe_unused]] instruction_desc desc, Op1 op1) {
        auto id = desc.id();
        auto opcode = desc.primary_opcode();

        if constexpr (Immediate8<Op1>) {
            opcode = static_cast<std::uint8_t>(0xeb);

            return internal::encode<Id, Op1, Void>(
                desc, opcode, op1.value());
        } else if constexpr (Register<Op1> || (Memory<Op1> && Register<typename Op1::value_type>)) {
            opcode = static_cast<std::uint8_t>(0xff);

            return internal::encode<Id, Op1, Void>(
                desc, opcode, encode_modrm(opcodeext_ff(id), op1));
        } else if constexpr (Immediate<Op1>) {
            return internal::encode<Id, Op1, Void>(
                desc, opcode, op1.value());
        }
    }

    template<int Len>
    inline constexpr auto encode_nop() {
        if constexpr (Len == 1) {
            return internal::make_array<std::uint8_t>(0x90);
        } else if constexpr (Len == 2) {
            return internal::make_array<std::uint8_t>(0x66, 0x90);
        } else if constexpr (Len == 3) {
            return internal::make_array<std::uint8_t>(0x0F, 0x1F, 0x00);
        } else if constexpr (Len == 4) {
            return internal::make_array<std::uint8_t>(0x0F, 0x1F, 0x40, 0x00);
        } else if constexpr (Len == 5) {
            return internal::make_array<std::uint8_t>(0x0F, 0x1F, 0x44, 0x00, 0x00);
        } else if constexpr (Len == 6) {
            return internal::make_array<std::uint8_t>(0x66, 0x0F, 0x1F, 0x44, 0x00, 0x00);
        } else if constexpr (Len == 7) {
            return internal::make_array<std::uint8_t>(0x0F, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00);
        } else if constexpr (Len == 8) {
            return internal::make_array<std::uint8_t>(0x0F, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00);
        } else if constexpr (Len == 9) {
            return internal::make_array<std::uint8_t>(0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00);
        }
    }

    template<typename Op1, typename Op2>
        requires(Register<Op1> || Memory<Op1>) && (Register<Op2> || Memory<Op2> || Immediate<Op2>)
    inline constexpr auto encode_opcode_mov(const std::uint8_t& opcode) {
        return static_cast<std::uint8_t>(
            (opcode & 0b11111110) +
            (Op2::size > 8 ? 0b01 : 0b00) // setup bit `s`
        );
    }

    // Optimized encoding for MOV reg, imm (uses shorter B0+rb/B8+rd form)
    // This overload is more constrained and takes precedence for register-immediate moves
    // For 8/16/32-bit registers, this saves 1 byte vs the C6/C7 ModR/M form
    // For 64-bit, we still use C7 (sign-extended imm32) as it's shorter than movabs
    template<e_instruction_id Id, typename Op1, typename Op2>
        requires Register<Op1> && Immediate<Op2> && (Op1::size <= 32)
    inline constexpr auto encode_mov([[maybe_unused]] instruction_desc desc, [[maybe_unused]] Op1 op1, Op2 op2) {
        // B0+rb for 8-bit, B8+rd for 16/32-bit
        constexpr std::uint8_t base_opcode = Op1::size == 8 ? 0xB0 : 0xB8;
        constexpr auto reg_id = static_cast<std::uint8_t>(Op1::id()) & 0x07;
        constexpr auto opcode = static_cast<std::uint8_t>(base_opcode + reg_id);

        if constexpr (Op1::size == 8) {
            // 8-bit: B0+rb ib
            if constexpr (Op1::extended) {
                // Extended 8-bit registers (r8b-r15b) need REX.B
                return internal::make_array<std::uint8_t>(
                    encode_rex<Op1>(), opcode, static_cast<std::uint8_t>(op2.value()));
            } else {
                // Legacy 8-bit registers (al, cl, dl, bl, ah, ch, dh, bh)
                return internal::make_array<std::uint8_t>(
                    opcode, static_cast<std::uint8_t>(op2.value()));
            }
        } else if constexpr (Op1::size == 16) {
            // 16-bit: 66 B8+rw iw
            auto imm = static_cast<std::uint16_t>(op2.value());
            if constexpr (Op1::extended) {
                return internal::make_array<std::uint8_t>(
                    0x66, encode_rex<Op1>(), opcode,
                    static_cast<std::uint8_t>(imm & 0xFF),
                    static_cast<std::uint8_t>((imm >> 8) & 0xFF));
            } else {
                return internal::make_array<std::uint8_t>(
                    0x66, opcode,
                    static_cast<std::uint8_t>(imm & 0xFF),
                    static_cast<std::uint8_t>((imm >> 8) & 0xFF));
            }
        } else {
            // 32-bit: B8+rd id
            auto imm = static_cast<std::uint32_t>(op2.value());
            if constexpr (Op1::extended) {
                return internal::make_array<std::uint8_t>(
                    encode_rex<Op1>(), opcode,
                    static_cast<std::uint8_t>(imm & 0xFF),
                    static_cast<std::uint8_t>((imm >> 8) & 0xFF),
                    static_cast<std::uint8_t>((imm >> 16) & 0xFF),
                    static_cast<std::uint8_t>((imm >> 24) & 0xFF));
            } else {
                return internal::make_array<std::uint8_t>(
                    opcode,
                    static_cast<std::uint8_t>(imm & 0xFF),
                    static_cast<std::uint8_t>((imm >> 8) & 0xFF),
                    static_cast<std::uint8_t>((imm >> 16) & 0xFF),
                    static_cast<std::uint8_t>((imm >> 24) & 0xFF));
            }
        }
    }

    // General MOV encoding for register-register, register-memory, memory-register,
    // memory-immediate, and 64-bit register-immediate (uses C6/C7 ModR/M form)
    template<e_instruction_id Id, typename Op1, typename Op2>
        requires((Register<Op1> || Memory<Op1>) && (Register<Op2> || Memory<Op2> || Immediate<Op2>) && !(Register<Op1> && Immediate<Op2> && (Op1::size <= 32)))
    inline constexpr auto encode_mov([[maybe_unused]] instruction_desc desc, [[maybe_unused]] Op1 op1, [[maybe_unused]] Op2 op2) {
        auto opcode = desc.primary_opcode();

        if constexpr (Immediate<Op2>) {
            if constexpr (Id != e_instruction_id::movabs) {
                opcode = encode_opcode_mov<Op1, Op2>(0xC6);
                return internal::encode<Id, Op1, Op2>(desc, opcode, encode_modrm(op1), op2.value());
            } else {
                // movabs encoding: opcode is B8+rd, register ID in lower 3 bits
                auto movabs_opcode = static_cast<std::uint8_t>(opcode + (static_cast<std::uint8_t>(Op1::id()) & 0x07));
                return internal::encode<Id, Op1, Op2>(desc, movabs_opcode, op2.value());
            }
        } else {
            opcode = encode_opcode_mov<Op1, Op2>(opcode);
            return internal::encode<Id, Op1, Op2>(desc, opcode, encode_modrm(op1, op2));
        }
    }

    template<e_instruction_id Id, typename Op1, typename Op2>
        requires Integer<Op2>
    inline constexpr auto encode_mov([[maybe_unused]] instruction_desc desc, const Op1& op1, const Op2& op2) {
        if constexpr (Id == e_instruction_id::movabs) {
            return encode_mov<Id>(desc, op1, immediate<std::uint64_t>(op2));
        } else {
            return encode_mov<Id>(desc, op1, immediate<typename truncate_as<Op1>::type>(op2));
        }
    }

    // =========================================================================
    // Shared memory-operand tail: ModR/M [+ SIB] [+ displacement]
    // =========================================================================
    //
    // Emits the canonical addressing tail for a SIB-capable memory operand,
    // choosing the ModR/M-only form when the hardware permits it and only
    // emitting a SIB byte when an index register, an RSP/R12 base, or a
    // base-less (disp32-only) operand requires one. `reg_bits` is the ModR/M
    // reg field: a register's low 3 bits for a reg<->mem instruction, or an
    // opcode-group extension for the unary / mul-div groups.
    template<typename SIBMem>
        requires SIBMemory<SIBMem>
    inline constexpr std::size_t mem_tail_size() {
        return 1u /* modrm */ + (mem_uses_sib<SIBMem>() ? 1u : 0u) + mem_disp_size<SIBMem>();
    }

    template<typename SIBMem>
        requires SIBMemory<SIBMem>
    inline constexpr void write_mem_tail(std::uint8_t reg_bits, const SIBMem& mem, std::uint8_t* out, std::size_t& i) {
        out[i++] = encode_modrm_mem<SIBMem>(reg_bits);
        if constexpr (mem_uses_sib<SIBMem>()) {
            out[i++] = encode_sib(mem);
        }
        constexpr std::size_t ds = mem_disp_size<SIBMem>();
        if constexpr (ds == 1) {
            out[i++] = static_cast<std::uint8_t>(mem.displacement());
        } else if constexpr (ds == 4) {
            auto disp = mem.displacement();
            out[i++] = static_cast<std::uint8_t>(disp);
            out[i++] = static_cast<std::uint8_t>(disp >> 8);
            out[i++] = static_cast<std::uint8_t>(disp >> 16);
            out[i++] = static_cast<std::uint8_t>(disp >> 24);
        }
    }

    // =========================================================================
    // SIB Encoding for MOV: mov reg, [base + index*scale + disp]
    // =========================================================================

    template<e_instruction_id Id, typename Op1, typename Op2>
        requires Register<Op1> && SIBMemory<Op2>
    inline constexpr auto encode_mov_sib([[maybe_unused]] instruction_desc desc, [[maybe_unused]] Op1 op1, [[maybe_unused]] Op2 op2) {
        // Opcode for mov reg, r/m (8B for 32/64-bit, 8A for 8-bit)
        std::uint8_t opcode = Op1::size == 8 ? 0x8A : 0x8B;

        constexpr bool need_rex = needs_rex_sib<Op1, typename Op2::base_type, typename Op2::index_type>();
        constexpr bool need_16bit_prefix = Op1::size == 16;

        constexpr std::size_t total_size = need_16bit_prefix + need_rex + 1 + mem_tail_size<Op2>();
        std::array<std::uint8_t, total_size> result{};
        std::size_t i = 0;

        if constexpr (need_16bit_prefix) {
            result[i++] = 0x66;
        }
        if constexpr (need_rex) {
            result[i++] = encode_rex_sib<Op1, typename Op2::base_type, typename Op2::index_type>();
        }
        result[i++] = opcode;
        write_mem_tail<Op2>(static_cast<std::uint8_t>(Op1::id()), op2, result.data(), i);
        return result;
    }

    // SIB Encoding for MOV: mov [base + index*scale + disp], reg
    template<e_instruction_id Id, typename Op1, typename Op2>
        requires SIBMemory<Op1> && Register<Op2>
    inline constexpr auto encode_mov_sib([[maybe_unused]] instruction_desc desc, [[maybe_unused]] Op1 op1, [[maybe_unused]] Op2 op2) {
        // Opcode for mov r/m, reg (89 for 32/64-bit, 88 for 8-bit)
        std::uint8_t opcode = Op2::size == 8 ? 0x88 : 0x89;

        constexpr bool need_rex = needs_rex_sib<Op2, typename Op1::base_type, typename Op1::index_type>();
        constexpr bool need_16bit_prefix = Op2::size == 16;

        constexpr std::size_t total_size = need_16bit_prefix + need_rex + 1 + mem_tail_size<Op1>();
        std::array<std::uint8_t, total_size> result{};
        std::size_t i = 0;

        if constexpr (need_16bit_prefix) {
            result[i++] = 0x66;
        }
        if constexpr (need_rex) {
            result[i++] = encode_rex_sib<Op2, typename Op1::base_type, typename Op1::index_type>();
        }
        result[i++] = opcode;
        write_mem_tail<Op1>(static_cast<std::uint8_t>(Op2::id()), op1, result.data(), i);
        return result;
    }

    // =========================================================================
    // SIB Encoding for ALU: add/sub/and/or/xor/cmp reg, [base + index*scale + disp]
    // =========================================================================

    template<e_instruction_id Id, typename Op1, typename Op2>
        requires Register<Op1> && SIBMemory<Op2>
    inline constexpr auto encode_alu_sib([[maybe_unused]] instruction_desc desc, [[maybe_unused]] Op1 op1, [[maybe_unused]] Op2 op2) {
        // Get base opcode and adjust for direction bit (reg, r/m -> d=1, so opcode | 0x02)
        auto opcode = desc.primary_opcode();
        opcode = encode_opcode_alu<Op1, Op2>(opcode);
        // For reg, mem: direction bit should indicate reg is destination
        opcode = static_cast<std::uint8_t>((opcode & 0xFC) | 0x02 | (Op1::size > 8 ? 1 : 0));

        constexpr bool need_rex = needs_rex_sib<Op1, typename Op2::base_type, typename Op2::index_type>();
        constexpr bool need_16bit_prefix = Op1::size == 16;

        constexpr std::size_t total_size = need_16bit_prefix + need_rex + 1 + mem_tail_size<Op2>();
        std::array<std::uint8_t, total_size> result{};
        std::size_t i = 0;

        if constexpr (need_16bit_prefix) {
            result[i++] = 0x66;
        }
        if constexpr (need_rex) {
            result[i++] = encode_rex_sib<Op1, typename Op2::base_type, typename Op2::index_type>();
        }
        result[i++] = opcode;
        write_mem_tail<Op2>(static_cast<std::uint8_t>(Op1::id()), op2, result.data(), i);
        return result;
    }

    // ALU with SIB memory as destination
    template<e_instruction_id Id, typename Op1, typename Op2>
        requires SIBMemory<Op1> && Register<Op2>
    inline constexpr auto encode_alu_sib([[maybe_unused]] instruction_desc desc, [[maybe_unused]] Op1 op1, [[maybe_unused]] Op2 op2) {
        auto opcode = desc.primary_opcode();
        // For mem, reg: d=0, direction bit clear
        opcode = static_cast<std::uint8_t>((opcode & 0xFC) | (Op2::size > 8 ? 1 : 0));

        constexpr bool need_rex = needs_rex_sib<Op2, typename Op1::base_type, typename Op1::index_type>();
        constexpr bool need_16bit_prefix = Op2::size == 16;

        constexpr std::size_t total_size = need_16bit_prefix + need_rex + 1 + mem_tail_size<Op1>();
        std::array<std::uint8_t, total_size> result{};
        std::size_t i = 0;

        if constexpr (need_16bit_prefix) {
            result[i++] = 0x66;
        }
        if constexpr (need_rex) {
            result[i++] = encode_rex_sib<Op2, typename Op1::base_type, typename Op1::index_type>();
        }
        result[i++] = opcode;
        write_mem_tail<Op1>(static_cast<std::uint8_t>(Op2::id()), op1, result.data(), i);
        return result;
    }

    // =========================================================================
    // SIB Encoding for LEA: lea reg, [base + index*scale + disp]
    // =========================================================================

    template<e_instruction_id Id, typename Op1, typename Op2>
        requires Register<Op1> && SIBMemory<Op2>
    inline constexpr auto encode_lea_sib([[maybe_unused]] instruction_desc desc, [[maybe_unused]] Op1 op1, [[maybe_unused]] Op2 op2) {
        std::uint8_t opcode = 0x8D; // LEA opcode

        constexpr bool need_rex = needs_rex_sib<Op1, typename Op2::base_type, typename Op2::index_type>();
        constexpr bool need_16bit_prefix = Op1::size == 16;

        constexpr std::size_t total_size = need_16bit_prefix + need_rex + 1 + mem_tail_size<Op2>();
        std::array<std::uint8_t, total_size> result{};
        std::size_t i = 0;

        if constexpr (need_16bit_prefix) {
            result[i++] = 0x66;
        }
        if constexpr (need_rex) {
            result[i++] = encode_rex_sib<Op1, typename Op2::base_type, typename Op2::index_type>();
        }
        result[i++] = opcode;
        write_mem_tail<Op2>(static_cast<std::uint8_t>(Op1::id()), op2, result.data(), i);
        return result;
    }

    template<e_instruction_id Id, typename Op1>
        requires Register<Op1>
    inline constexpr auto encode_pop([[maybe_unused]] instruction_desc desc, const Op1& op1) {
        auto primary_opcode = desc.primary_opcode();

        if constexpr (Op1::extended) {
            // Extended registers (r8-r15) need REX.B prefix
            return internal::make_array<std::uint8_t>(
                encode_rex_b_only<Op1>(),
                encode_opcode_pushpop(primary_opcode, op1));
        } else {
            return internal::make_array<std::uint8_t>(encode_opcode_pushpop(primary_opcode, op1));
        }
    }

    template<e_instruction_id Id, typename Op1>
        requires(Register<Op1> || (Immediate<Op1> && (Op1::size == 8 || Op1::size == 32)))
    inline constexpr auto encode_push([[maybe_unused]] instruction_desc desc, const Op1& op1) {
        auto primary_opcode = desc.primary_opcode();

        if constexpr (Immediate<Op1>) {
            primary_opcode = 0x68;

            if constexpr (Immediate8<Op1>) {
                primary_opcode = 0x6A;
            }

            return internal::encode<Id, Op1, Void>(desc, primary_opcode, op1.value());
        } else if constexpr (Register<Op1>) {
            if constexpr (Op1::extended) {
                // Extended registers (r8-r15) need REX.B prefix
                return internal::make_array<std::uint8_t>(
                    encode_rex_b_only<Op1>(),
                    encode_opcode_pushpop(primary_opcode, op1));
            } else {
                return internal::make_array<std::uint8_t>(encode_opcode_pushpop(primary_opcode, op1));
            }
        }
    }

    template<e_instruction_id Id>
    inline constexpr auto encode_push([[maybe_unused]] instruction_desc desc, const std::uint32_t& val) {
        return encode_push<Id, imm32>(desc, imm32(val));
    }

    template<e_instruction_id Id, typename Op1>
        requires Immediate<Op1>
    inline constexpr auto encode_ret([[maybe_unused]] instruction_desc desc, const Op1& op1) {
        auto value = static_cast<std::uint16_t>(op1.value());

        auto opcode = static_cast<std::uint8_t>(desc.primary_opcode() - 1);
        return internal::encode<Id, Op1, Void>(desc, opcode, value);
    }

    template<e_instruction_id Id, typename Op1>
        requires Integer<Op1>
    inline constexpr auto encode_ret([[maybe_unused]] instruction_desc desc, const Op1& op1) {
        return encode_ret<Id>(desc, immediate<std::uint16_t>(op1));
    }

    // Unary operations: INC, DEC, NEG, NOT
    // These use opcode extension in ModR/M reg field
    template<e_instruction_id Id, typename Op1>
        requires Register<Op1>
    inline constexpr auto encode_unary([[maybe_unused]] instruction_desc desc, [[maybe_unused]] const Op1& op1) {
        // Opcode: FE for 8-bit, FF for 16/32/64-bit (INC/DEC)
        //         F6 for 8-bit, F7 for 16/32/64-bit (NEG/NOT)
        auto opcode = desc.primary_opcode();
        auto ext = desc.secondary_opcode(); // opcode extension (0=INC, 1=DEC, 2=NOT, 3=NEG)

        // Adjust opcode for 8-bit operands
        if constexpr (Op1::size == 8) {
            opcode = static_cast<std::uint8_t>(opcode - 1); // FF->FE or F7->F6
        }

        // ModR/M byte: mod=11 (register), reg=extension, r/m=register
        auto modrm = static_cast<std::uint8_t>(0xC0 | (ext << 3) | (static_cast<std::uint8_t>(Op1::id()) & 0x07));

        if constexpr (needs_rex<Op1>()) {
            return internal::make_array<std::uint8_t>(encode_rex<Op1>(), opcode, modrm);
        } else if constexpr (Op1::extended) {
            // Extended register without REX.W (e.g., r8d)
            return internal::make_array<std::uint8_t>(encode_rex_b_only<Op1>(), opcode, modrm);
        } else {
            return internal::make_array<std::uint8_t>(opcode, modrm);
        }
    }

    // Unary on a SIB memory operand, e.g. inc qword [rcx + 0x20]. The opcode
    // extension (0=INC, 1=DEC, 2=NOT, 3=NEG) goes in the ModR/M reg field and
    // the r/m field selects the SIB byte. Mirrors encode_mov_sib, but with an
    // extension in place of a register operand.
    template<e_instruction_id Id, typename Op1>
        requires SIBMemory<Op1>
    inline constexpr auto encode_unary([[maybe_unused]] instruction_desc desc, const Op1& op1) {
        auto opcode = desc.primary_opcode();
        auto ext = desc.secondary_opcode();
        if constexpr (Op1::size == 8) {
            opcode = static_cast<std::uint8_t>(opcode - 1); // FF->FE or F7->F6
        }

        // REX.W from operand size; REX.X/B from the SIB index/base registers.
        constexpr bool rex_w = (Op1::size >= 64);
        constexpr bool rex_x = needs_rex_x<typename Op1::index_type>();
        constexpr bool rex_b = needs_rex_b_sib<typename Op1::base_type>();
        constexpr bool need_rex = rex_w || rex_x || rex_b;
        constexpr bool need_16bit_prefix = (Op1::size == 16);

        constexpr std::size_t total_size = need_16bit_prefix + need_rex + 1 /*opcode*/ + mem_tail_size<Op1>();
        std::array<std::uint8_t, total_size> result{};
        std::size_t i = 0;

        if constexpr (need_16bit_prefix) {
            result[i++] = 0x66;
        }
        if constexpr (need_rex) {
            result[i++] = static_cast<std::uint8_t>((0b0100 << 4) | (rex_w << 3) | (0 << 2) | (rex_x << 1) | rex_b);
        }
        result[i++] = opcode;
        write_mem_tail<Op1>(ext, op1, result.data(), i);
        return result;
    }

    // Unary on a plain register-indirect memory operand, e.g. inc qword [rcx].
    // (RSP/R12 as base would require a SIB byte and RBP/R13 a forced disp8; use
    // the SIB form `reg + disp` for those bases.)
    template<e_instruction_id Id, typename Op1>
        requires Memory<Op1> && Register<typename Op1::value_type>
    inline constexpr auto encode_unary([[maybe_unused]] instruction_desc desc, [[maybe_unused]] const Op1& op1) {
        auto opcode = desc.primary_opcode();
        auto ext = desc.secondary_opcode();
        if constexpr (Op1::size == 8) {
            opcode = static_cast<std::uint8_t>(opcode - 1);
        }

        using Base = typename Op1::value_type;
        std::uint8_t modrm = static_cast<std::uint8_t>((static_cast<std::uint8_t>(e_mod::register_indirect_addressing) << 6) | ((ext & 0b111) << 3) | (static_cast<std::uint8_t>(Base::id()) & 0b111));

        constexpr bool rex_w = (Op1::size >= 64);
        constexpr bool rex_b = Base::extended;
        constexpr bool need_rex = rex_w || rex_b;
        constexpr bool need_16bit_prefix = (Op1::size == 16);

        constexpr std::size_t total_size = need_16bit_prefix + need_rex + 1 + 1;
        std::array<std::uint8_t, total_size> result{};
        std::size_t i = 0;
        if constexpr (need_16bit_prefix) {
            result[i++] = 0x66;
        }
        if constexpr (need_rex) {
            result[i++] = static_cast<std::uint8_t>((0b0100 << 4) | (rex_w << 3) | (0 << 2) | (0 << 1) | rex_b);
        }
        result[i++] = opcode;
        result[i++] = modrm;
        return result;
    }

    // TEST instruction: reg-reg or reg-imm
    template<e_instruction_id Id, typename Op1, typename Op2>
        requires Register<Op1> && Register<Op2>
    inline constexpr auto encode_test([[maybe_unused]] instruction_desc desc, const Op1& op1, const Op2& op2) {
        // TEST r/m, r: opcode 84 (8-bit) or 85 (16/32/64-bit)
        auto opcode = desc.primary_opcode(); // 0x85

        if constexpr (Op1::size == 8) {
            opcode = static_cast<std::uint8_t>(opcode - 1); // 85->84
        }

        auto modrm = encode_modrm(op1, op2);

        if constexpr (needs_rex<Op1, Op2>()) {
            return internal::make_array<std::uint8_t>(encode_rex<Op1, Op2>(), opcode, modrm);
        } else {
            return internal::make_array<std::uint8_t>(opcode, modrm);
        }
    }

    template<e_instruction_id Id, typename Op1, typename Op2>
        requires Register<Op1> && Immediate<Op2>
    inline constexpr auto encode_test([[maybe_unused]] instruction_desc desc, [[maybe_unused]] const Op1& op1, const Op2& op2) {
        // TEST r/m, imm: opcode F6 /0 (8-bit) or F7 /0 (16/32/64-bit)
        std::uint8_t opcode = Op1::size == 8 ? 0xF6 : 0xF7;

        // ModR/M: mod=11, reg=0 (extension), r/m=register
        auto modrm = static_cast<std::uint8_t>(0xC0 | (static_cast<std::uint8_t>(Op1::id()) & 0x07));

        auto value = op2.value();

        // Build byte array manually to avoid double REX prefix from internal::encode
        if constexpr (needs_rex<Op1>()) {
            if constexpr (Op2::size == 8) {
                return internal::make_array<std::uint8_t>(
                    encode_rex<Op1>(), opcode, modrm,
                    static_cast<std::uint8_t>(value));
            } else if constexpr (Op2::size == 16) {
                return internal::make_array<std::uint8_t>(
                    encode_rex<Op1>(), opcode, modrm,
                    static_cast<std::uint8_t>(value),
                    static_cast<std::uint8_t>(value >> 8));
            } else {
                return internal::make_array<std::uint8_t>(
                    encode_rex<Op1>(), opcode, modrm,
                    static_cast<std::uint8_t>(value),
                    static_cast<std::uint8_t>(value >> 8),
                    static_cast<std::uint8_t>(value >> 16),
                    static_cast<std::uint8_t>(value >> 24));
            }
        } else if constexpr (Op1::extended) {
            if constexpr (Op2::size == 8) {
                return internal::make_array<std::uint8_t>(
                    encode_rex_b_only<Op1>(), opcode, modrm,
                    static_cast<std::uint8_t>(value));
            } else if constexpr (Op2::size == 16) {
                return internal::make_array<std::uint8_t>(
                    encode_rex_b_only<Op1>(), opcode, modrm,
                    static_cast<std::uint8_t>(value),
                    static_cast<std::uint8_t>(value >> 8));
            } else {
                return internal::make_array<std::uint8_t>(
                    encode_rex_b_only<Op1>(), opcode, modrm,
                    static_cast<std::uint8_t>(value),
                    static_cast<std::uint8_t>(value >> 8),
                    static_cast<std::uint8_t>(value >> 16),
                    static_cast<std::uint8_t>(value >> 24));
            }
        } else {
            if constexpr (Op2::size == 8) {
                return internal::make_array<std::uint8_t>(
                    opcode, modrm,
                    static_cast<std::uint8_t>(value));
            } else if constexpr (Op2::size == 16) {
                return internal::make_array<std::uint8_t>(
                    opcode, modrm,
                    static_cast<std::uint8_t>(value),
                    static_cast<std::uint8_t>(value >> 8));
            } else {
                return internal::make_array<std::uint8_t>(
                    opcode, modrm,
                    static_cast<std::uint8_t>(value),
                    static_cast<std::uint8_t>(value >> 8),
                    static_cast<std::uint8_t>(value >> 16),
                    static_cast<std::uint8_t>(value >> 24));
            }
        }
    }

    template<e_instruction_id Id, typename Op1, typename Op2>
        requires Register<Op1> && Integer<Op2>
    inline constexpr auto encode_test([[maybe_unused]] instruction_desc desc, const Op1& op1, const Op2& op2) {
        return encode_test<Id>(desc, op1, immediate<typename truncate_as<Op1>::type>(op2));
    }

    // LEA instruction: load effective address
    template<e_instruction_id Id, typename Op1, typename Op2>
        requires Register<Op1> && Memory<Op2>
    inline constexpr auto encode_lea([[maybe_unused]] instruction_desc desc, const Op1& op1, const Op2& op2) {
        auto opcode = desc.primary_opcode(); // 0x8D
        return internal::encode<Id, Op1, Op2>(desc, opcode, encode_modrm(op1, op2));
    }

    // MUL/DIV/IMUL/IDIV - single operand with opcode extension (same pattern as unary)
    template<e_instruction_id Id, typename Op1>
        requires Register<Op1>
    inline constexpr auto encode_muldiv([[maybe_unused]] instruction_desc desc, [[maybe_unused]] const Op1& op1) {
        // Opcode: F6 for 8-bit, F7 for 16/32/64-bit
        auto opcode = desc.primary_opcode(); // 0xF7
        auto ext = desc.secondary_opcode(); // 4=MUL, 5=IMUL, 6=DIV, 7=IDIV

        if constexpr (Op1::size == 8) {
            opcode = static_cast<std::uint8_t>(opcode - 1); // F7->F6
        }

        // ModR/M byte: mod=11 (register), reg=extension, r/m=register
        auto modrm = static_cast<std::uint8_t>(0xC0 | (ext << 3) | (static_cast<std::uint8_t>(Op1::id()) & 0x07));

        if constexpr (needs_rex<Op1>()) {
            return internal::make_array<std::uint8_t>(encode_rex<Op1>(), opcode, modrm);
        } else if constexpr (Op1::extended) {
            return internal::make_array<std::uint8_t>(encode_rex_b_only<Op1>(), opcode, modrm);
        } else {
            return internal::make_array<std::uint8_t>(opcode, modrm);
        }
    }

    // MUL/IMUL/DIV/IDIV on a SIB memory operand, e.g. mul qword [rcx + 0x20].
    // The F6/F7 group extension (4=MUL, 5=IMUL, 6=DIV, 7=IDIV) goes in the
    // ModR/M reg field; mirrors the SIB encode_unary overload.
    template<e_instruction_id Id, typename Op1>
        requires SIBMemory<Op1>
    inline constexpr auto encode_muldiv([[maybe_unused]] instruction_desc desc, const Op1& op1) {
        auto opcode = desc.primary_opcode();
        auto ext = desc.secondary_opcode();
        if constexpr (Op1::size == 8) {
            opcode = static_cast<std::uint8_t>(opcode - 1); // F7->F6
        }

        constexpr bool rex_w = (Op1::size >= 64);
        constexpr bool rex_x = needs_rex_x<typename Op1::index_type>();
        constexpr bool rex_b = needs_rex_b_sib<typename Op1::base_type>();
        constexpr bool need_rex = rex_w || rex_x || rex_b;
        constexpr bool need_16bit_prefix = (Op1::size == 16);

        constexpr std::size_t total_size = need_16bit_prefix + need_rex + 1 /*opcode*/ + mem_tail_size<Op1>();
        std::array<std::uint8_t, total_size> result{};
        std::size_t i = 0;

        if constexpr (need_16bit_prefix) {
            result[i++] = 0x66;
        }
        if constexpr (need_rex) {
            result[i++] = static_cast<std::uint8_t>((0b0100 << 4) | (rex_w << 3) | (0 << 2) | (rex_x << 1) | rex_b);
        }
        result[i++] = opcode;
        write_mem_tail<Op1>(ext, op1, result.data(), i);
        return result;
    }

    // MUL/IMUL/DIV/IDIV on a plain register-indirect memory operand, e.g.
    // mul qword [rcx]. (RSP/R12 or RBP/R13 bases need the `reg + disp` form.)
    template<e_instruction_id Id, typename Op1>
        requires Memory<Op1> && Register<typename Op1::value_type>
    inline constexpr auto encode_muldiv([[maybe_unused]] instruction_desc desc, [[maybe_unused]] const Op1& op1) {
        auto opcode = desc.primary_opcode();
        auto ext = desc.secondary_opcode();
        if constexpr (Op1::size == 8) {
            opcode = static_cast<std::uint8_t>(opcode - 1);
        }

        using Base = typename Op1::value_type;
        std::uint8_t modrm = static_cast<std::uint8_t>((static_cast<std::uint8_t>(e_mod::register_indirect_addressing) << 6) | ((ext & 0b111) << 3) | (static_cast<std::uint8_t>(Base::id()) & 0b111));

        constexpr bool rex_w = (Op1::size >= 64);
        constexpr bool rex_b = Base::extended;
        constexpr bool need_rex = rex_w || rex_b;
        constexpr bool need_16bit_prefix = (Op1::size == 16);

        constexpr std::size_t total_size = need_16bit_prefix + need_rex + 1 + 1;
        std::array<std::uint8_t, total_size> result{};
        std::size_t i = 0;
        if constexpr (need_16bit_prefix) {
            result[i++] = 0x66;
        }
        if constexpr (need_rex) {
            result[i++] = static_cast<std::uint8_t>((0b0100 << 4) | (rex_w << 3) | (0 << 2) | (0 << 1) | rex_b);
        }
        result[i++] = opcode;
        result[i++] = modrm;
        return result;
    }

    // Shift/rotate instructions: SHL, SHR, SAL, SAR, ROL, ROR, RCL, RCR
    // Three forms:
    // 1. r/m, 1      - shift by 1 (D0/D1 opcode)
    // 2. r/m, CL     - shift by CL register (D2/D3 opcode)
    // 3. r/m, imm8   - shift by immediate (C0/C1 opcode)

    // Helper to check if a register8bit_operand is CL - now compile-time
    template<typename Reg>
        requires LRegister8<Reg>
    inline consteval bool is_cl_register() {
        return Reg::id() == e_register8bit_id::cl;
    }

    // Shift by 1 (implicit)
    template<e_instruction_id Id, typename Op1>
        requires Register<Op1>
    inline constexpr auto encode_shift_by_one([[maybe_unused]] instruction_desc desc, [[maybe_unused]] const Op1& op1) {
        // Opcode: D0 for 8-bit, D1 for 16/32/64-bit
        std::uint8_t opcode = Op1::size == 8 ? 0xD0 : 0xD1;
        auto ext = desc.secondary_opcode(); // opcode extension (4=SHL, 5=SHR, 7=SAR, 0=ROL, 1=ROR, 2=RCL, 3=RCR)

        // ModR/M byte: mod=11 (register), reg=extension, r/m=register
        auto modrm = static_cast<std::uint8_t>(0xC0 | (ext << 3) | (static_cast<std::uint8_t>(Op1::id()) & 0x07));

        if constexpr (needs_rex<Op1>()) {
            return internal::make_array<std::uint8_t>(encode_rex<Op1>(), opcode, modrm);
        } else if constexpr (Op1::extended) {
            return internal::make_array<std::uint8_t>(encode_rex_b_only<Op1>(), opcode, modrm);
        } else {
            return internal::make_array<std::uint8_t>(opcode, modrm);
        }
    }

    // Shift by CL register
    template<e_instruction_id Id, typename Op1, typename Op2>
        requires Register<Op1> && IsCLRegister<Op2>
    inline constexpr auto encode_shift_by_cl([[maybe_unused]] instruction_desc desc, [[maybe_unused]] const Op1& op1, [[maybe_unused]] const Op2& op2) {
        // Opcode: D2 for 8-bit, D3 for 16/32/64-bit
        std::uint8_t opcode = Op1::size == 8 ? 0xD2 : 0xD3;
        auto ext = desc.secondary_opcode();

        // ModR/M byte: mod=11 (register), reg=extension, r/m=register
        auto modrm = static_cast<std::uint8_t>(0xC0 | (ext << 3) | (static_cast<std::uint8_t>(Op1::id()) & 0x07));

        if constexpr (needs_rex<Op1>()) {
            return internal::make_array<std::uint8_t>(encode_rex<Op1>(), opcode, modrm);
        } else if constexpr (Op1::extended) {
            return internal::make_array<std::uint8_t>(encode_rex_b_only<Op1>(), opcode, modrm);
        } else {
            return internal::make_array<std::uint8_t>(opcode, modrm);
        }
    }

    // Shift by immediate
    template<e_instruction_id Id, typename Op1, typename Op2>
        requires Register<Op1> && Immediate8<Op2>
    inline constexpr auto encode_shift_by_imm([[maybe_unused]] instruction_desc desc, [[maybe_unused]] const Op1& op1, const Op2& op2) {
        // Opcode: C0 for 8-bit, C1 for 16/32/64-bit
        std::uint8_t opcode = Op1::size == 8 ? 0xC0 : 0xC1;
        auto ext = desc.secondary_opcode();

        // ModR/M byte: mod=11 (register), reg=extension, r/m=register
        auto modrm = static_cast<std::uint8_t>(0xC0 | (ext << 3) | (static_cast<std::uint8_t>(Op1::id()) & 0x07));

        auto imm_value = static_cast<std::uint8_t>(op2.value());

        if constexpr (needs_rex<Op1>()) {
            return internal::make_array<std::uint8_t>(encode_rex<Op1>(), opcode, modrm, imm_value);
        } else if constexpr (Op1::extended) {
            return internal::make_array<std::uint8_t>(encode_rex_b_only<Op1>(), opcode, modrm, imm_value);
        } else {
            return internal::make_array<std::uint8_t>(opcode, modrm, imm_value);
        }
    }

    // Shift by integer (converted to immediate)
    template<e_instruction_id Id, typename Op1, typename Op2>
        requires Register<Op1> && Integer<Op2>
    inline constexpr auto encode_shift([[maybe_unused]] instruction_desc desc, const Op1& op1, const Op2& op2) {
        return encode_shift_by_imm<Id>(desc, op1, imm8(static_cast<std::uint8_t>(op2)));
    }

    // CMOV instruction: conditional move
    // Form: cmovCC r16/32/64, r/m16/32/64
    // Encoding: 0F 4x /r (opcode is in primary_opcode, 0F prefix handled by internal::encode)
    template<e_instruction_id Id, typename Op1, typename Op2>
        requires Register<Op1> && Register<Op2>
    inline constexpr auto encode_cmov([[maybe_unused]] instruction_desc desc, const Op1& op1, const Op2& op2) {
        auto opcode = desc.primary_opcode();
        // CMOV: destination is in reg field, source is in r/m field
        // ModR/M: mod=11 (register), reg=destination, r/m=source
        return internal::encode<Id, Op2, Op1>(desc, opcode, encode_modrm(op2, op1));
    }

    template<e_instruction_id Id, typename Op1, typename Op2>
        requires Register<Op1> && Memory<Op2>
    inline constexpr auto encode_cmov([[maybe_unused]] instruction_desc desc, const Op1& op1, const Op2& op2) {
        auto opcode = desc.primary_opcode();
        // CMOV r, m: destination register in reg field, memory in r/m field
        return internal::encode<Id, Op1, Op2>(desc, opcode, encode_modrm(op1, op2));
    }

    // IMUL two-operand form: r = r * r/m (0F AF /r)
    // Note: op1 (destination) goes in REG field, op2 (source) goes in R/M field
    template<e_instruction_id Id, typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto encode_imul_two([[maybe_unused]] instruction_desc desc, const Op1& op1, const Op2& op2) {
        auto opcode = desc.primary_opcode(); // 0xAF

        if constexpr (Register<Op2>) {
            // reg-reg form: op1 (dst) in REG, op2 (src) in R/M
            // encode_modrm(r/m, reg) - op2 is r/m, op1 is reg
            auto modrm = encode_modrm(op2, op1);

            // REX prefix: REX.R from op1 (dst in reg field), REX.B from op2 (src in r/m field)
            // encode_rex<Op2, Op1> gives: R from Op1, B from Op2
            if constexpr (needs_rex<Op2, Op1>()) {
                return internal::make_array<std::uint8_t>(encode_rex<Op2, Op1>(), 0x0F, opcode, modrm);
            } else {
                return internal::make_array<std::uint8_t>(0x0F, opcode, modrm);
            }
        } else {
            // reg-mem form: op1 (dst) in REG, op2 (mem src) in R/M
            auto modrm = encode_modrm(op1, op2);

            if constexpr (needs_rex<Op1, Op2>()) {
                return internal::make_array<std::uint8_t>(encode_rex<Op1, Op2>(), 0x0F, opcode, modrm);
            } else {
                return internal::make_array<std::uint8_t>(0x0F, opcode, modrm);
            }
        }
    }

    // IMUL three-operand form: r = r/m * imm
    // 6B /r ib - IMUL r, r/m, imm8 (sign-extended)
    // 69 /r iw/id - IMUL r, r/m, imm16/32
    template<e_instruction_id Id, typename Op1, typename Op2, typename Op3>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>) && Immediate<Op3>
    inline constexpr auto encode_imul_three([[maybe_unused]] instruction_desc desc, const Op1& op1, const Op2& op2, const Op3& op3) {
        // Determine opcode based on immediate size
        // 0x6B for imm8 (sign-extended), 0x69 for imm16/32
        constexpr bool use_imm8 = Op3::size == 8;
        auto opcode = use_imm8 ? static_cast<std::uint8_t>(0x6B) : static_cast<std::uint8_t>(0x69);

        auto value = op3.value();

        if constexpr (Register<Op2>) {
            // ModR/M: mod=11 (register), reg=op1, r/m=op2
            auto modrm = encode_modrm(op2, op1);

            if constexpr (needs_rex<Op1, Op2>()) {
                if constexpr (use_imm8) {
                    return internal::make_array<std::uint8_t>(
                        encode_rex<Op1, Op2>(), opcode, modrm,
                        static_cast<std::uint8_t>(value));
                } else if constexpr (Op3::size == 16) {
                    return internal::make_array<std::uint8_t>(
                        encode_rex<Op1, Op2>(), opcode, modrm,
                        static_cast<std::uint8_t>(value),
                        static_cast<std::uint8_t>(value >> 8));
                } else {
                    return internal::make_array<std::uint8_t>(
                        encode_rex<Op1, Op2>(), opcode, modrm,
                        static_cast<std::uint8_t>(value),
                        static_cast<std::uint8_t>(value >> 8),
                        static_cast<std::uint8_t>(value >> 16),
                        static_cast<std::uint8_t>(value >> 24));
                }
            } else if constexpr (Op1::extended || Op2::extended) {
                if constexpr (use_imm8) {
                    return internal::make_array<std::uint8_t>(
                        encode_rex<Op1, Op2>(), opcode, modrm,
                        static_cast<std::uint8_t>(value));
                } else if constexpr (Op3::size == 16) {
                    return internal::make_array<std::uint8_t>(
                        encode_rex<Op1, Op2>(), opcode, modrm,
                        static_cast<std::uint8_t>(value),
                        static_cast<std::uint8_t>(value >> 8));
                } else {
                    return internal::make_array<std::uint8_t>(
                        encode_rex<Op1, Op2>(), opcode, modrm,
                        static_cast<std::uint8_t>(value),
                        static_cast<std::uint8_t>(value >> 8),
                        static_cast<std::uint8_t>(value >> 16),
                        static_cast<std::uint8_t>(value >> 24));
                }
            } else {
                if constexpr (use_imm8) {
                    return internal::make_array<std::uint8_t>(
                        opcode, modrm,
                        static_cast<std::uint8_t>(value));
                } else if constexpr (Op3::size == 16) {
                    return internal::make_array<std::uint8_t>(
                        opcode, modrm,
                        static_cast<std::uint8_t>(value),
                        static_cast<std::uint8_t>(value >> 8));
                } else {
                    return internal::make_array<std::uint8_t>(
                        opcode, modrm,
                        static_cast<std::uint8_t>(value),
                        static_cast<std::uint8_t>(value >> 8),
                        static_cast<std::uint8_t>(value >> 16),
                        static_cast<std::uint8_t>(value >> 24));
                }
            }
        } else {
            // Memory operand - use similar logic
            auto modrm = encode_modrm(op1, op2);

            if constexpr (needs_rex<Op1, Op2>()) {
                if constexpr (use_imm8) {
                    return internal::make_array<std::uint8_t>(
                        encode_rex<Op1, Op2>(), opcode, modrm,
                        static_cast<std::uint8_t>(value));
                } else if constexpr (Op3::size == 16) {
                    return internal::make_array<std::uint8_t>(
                        encode_rex<Op1, Op2>(), opcode, modrm,
                        static_cast<std::uint8_t>(value),
                        static_cast<std::uint8_t>(value >> 8));
                } else {
                    return internal::make_array<std::uint8_t>(
                        encode_rex<Op1, Op2>(), opcode, modrm,
                        static_cast<std::uint8_t>(value),
                        static_cast<std::uint8_t>(value >> 8),
                        static_cast<std::uint8_t>(value >> 16),
                        static_cast<std::uint8_t>(value >> 24));
                }
            } else {
                if constexpr (use_imm8) {
                    return internal::make_array<std::uint8_t>(
                        opcode, modrm,
                        static_cast<std::uint8_t>(value));
                } else if constexpr (Op3::size == 16) {
                    return internal::make_array<std::uint8_t>(
                        opcode, modrm,
                        static_cast<std::uint8_t>(value),
                        static_cast<std::uint8_t>(value >> 8));
                } else {
                    return internal::make_array<std::uint8_t>(
                        opcode, modrm,
                        static_cast<std::uint8_t>(value),
                        static_cast<std::uint8_t>(value >> 8),
                        static_cast<std::uint8_t>(value >> 16),
                        static_cast<std::uint8_t>(value >> 24));
                }
            }
        }
    }

    // IMUL three-operand with integer literal (convenience overload)
    template<e_instruction_id Id, typename Op1, typename Op2, typename Op3>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>) && Integer<Op3>
    inline constexpr auto encode_imul_three([[maybe_unused]] instruction_desc desc, const Op1& op1, const Op2& op2, const Op3& op3) {
        // Choose immediate size based on value
        // If value fits in signed 8-bit, use imm8 for smaller encoding
        if constexpr (sizeof(Op3) == 1) {
            return encode_imul_three<Id>(desc, op1, op2, imm8(static_cast<std::uint8_t>(op3)));
        } else {
            // Use 32-bit immediate for larger values
            return encode_imul_three<Id>(desc, op1, op2, imm32(static_cast<std::uint32_t>(op3)));
        }
    }

    // XCHG instruction: exchange register contents
    template<e_instruction_id Id, typename Op1, typename Op2>
        requires Register<Op1> && Register<Op2>
    inline constexpr auto encode_xchg([[maybe_unused]] instruction_desc desc, const Op1& op1, const Op2& op2) {
        // XCHG has a short form for xchg rax, r: 90+rd
        // But for simplicity, we'll use the general form: 86 /r (8-bit), 87 /r (16/32/64-bit)
        auto opcode = desc.primary_opcode(); // 0x87

        if constexpr (Op1::size == 8) {
            opcode = static_cast<std::uint8_t>(opcode - 1); // 87->86
        }

        return internal::encode<Id, Op1, Op2>(desc, opcode, encode_modrm(op1, op2));
    }

    // =========================================================================
    // MOVZX/MOVSX - Move with Zero/Sign Extension
    // =========================================================================
    // MOVZX r16/32/64, r/m8:  0F B6 /r (with 66 prefix for 16-bit dest)
    // MOVZX r32/64, r/m16:    0F B7 /r
    // MOVSX r16/32/64, r/m8:  0F BE /r (with 66 prefix for 16-bit dest)
    // MOVSX r32/64, r/m16:    0F BF /r
    //
    // The opcode selects source size:
    //   B6/BE = source is 8-bit
    //   B7/BF = source is 16-bit
    //
    // REX.W determines if destination is 64-bit
    // 66 prefix makes destination 16-bit

    // Helper to get the 8-bit register ID for encoding - now compile-time
    template<typename Reg>
        requires Register8<Reg>
    inline constexpr std::uint8_t get_reg8_id([[maybe_unused]] const Reg& reg) {
        if constexpr (LRegister8<Reg>) {
            // Map al=0, cl=1, dl=2, bl=3, ah=4, ch=5, dh=6, bh=7
            return static_cast<std::uint8_t>(Reg::id());
        } else {
            // Extended 8-bit registers (sil, dil, bpl, spl, r8b-r15b)
            return static_cast<std::uint8_t>(Reg::id()) & 0x07;
        }
    }

    // MOVZX/MOVSX: register to register (8-bit source)
    template<e_instruction_id Id, typename Dest, typename Src>
        requires Register<Dest> && Register8<Src> && (Dest::size >= 16)
    inline constexpr auto encode_movzx_movsx([[maybe_unused]] instruction_desc desc, [[maybe_unused]] const Dest& dest, [[maybe_unused]] const Src& src) {
        // Opcode: B6 (MOVZX from 8-bit) or BE (MOVSX from 8-bit)
        auto opcode = desc.primary_opcode();

        // ModR/M: mod=11 (register), reg=destination, r/m=source
        constexpr auto src_id = get_reg8_id(Src{});
        auto modrm = static_cast<std::uint8_t>(0xC0 | ((static_cast<std::uint8_t>(Dest::id()) & 0x07) << 3) | (src_id & 0x07));

        // Determine REX prefix
        constexpr bool need_rex_w = (Dest::size == 64);
        constexpr bool dest_extended = Dest::extended;
        // Check if source is extended 8-bit register
        constexpr bool src_extended = []() {
            if constexpr (LRegister8<Src>) {
                return false; // Legacy 8-bit registers (al, cl, dl, bl, ah, ch, dh, bh)
            } else {
                return Src::extended; // Extended 8-bit (sil, dil, bpl, spl, r8b-r15b)
            }
        }();

        constexpr bool need_rex = need_rex_w || dest_extended || src_extended;
        constexpr bool need_16bit_prefix = (Dest::size == 16);

        constexpr std::size_t total_size = need_16bit_prefix + need_rex + 2 + 1; // [66] + [REX] + 0F opcode + ModR/M
        std::array<std::uint8_t, total_size> result{};
        std::size_t i = 0;

        if constexpr (need_16bit_prefix) {
            result[i++] = 0x66;
        }

        if constexpr (need_rex) {
            std::uint8_t rex = 0x40;
            if constexpr (need_rex_w)
                rex |= 0x08;
            if constexpr (dest_extended)
                rex |= 0x04; // REX.R for destination in reg field
            if constexpr (src_extended)
                rex |= 0x01; // REX.B for source in r/m field
            result[i++] = rex;
        }

        result[i++] = 0x0F;
        result[i++] = opcode;
        result[i++] = modrm;

        return result;
    }

    // MOVZX/MOVSX: register to register (16-bit source)
    template<e_instruction_id Id, typename Dest, typename Src>
        requires Register<Dest> && Register16<Src> && (Dest::size >= 32)
    inline constexpr auto encode_movzx_movsx([[maybe_unused]] instruction_desc desc, [[maybe_unused]] const Dest& dest, [[maybe_unused]] const Src& src) {
        // Opcode: B7 (MOVZX from 16-bit) or BF (MOVSX from 16-bit)
        auto opcode = static_cast<std::uint8_t>(desc.primary_opcode() + 1);

        // ModR/M: mod=11 (register), reg=destination, r/m=source
        auto modrm = static_cast<std::uint8_t>(0xC0 | ((static_cast<std::uint8_t>(Dest::id()) & 0x07) << 3) |
                                               (static_cast<std::uint8_t>(Src::id()) & 0x07));

        constexpr bool need_rex_w = (Dest::size == 64);
        constexpr bool dest_extended = Dest::extended;
        constexpr bool src_extended = Src::extended;
        constexpr bool need_rex = need_rex_w || dest_extended || src_extended;

        constexpr std::size_t total_size = need_rex + 2 + 1; // [REX] + 0F opcode + ModR/M
        std::array<std::uint8_t, total_size> result{};
        std::size_t i = 0;

        if constexpr (need_rex) {
            std::uint8_t rex = 0x40;
            if constexpr (need_rex_w)
                rex |= 0x08;
            if constexpr (dest_extended)
                rex |= 0x04;
            if constexpr (src_extended)
                rex |= 0x01;
            result[i++] = rex;
        }

        result[i++] = 0x0F;
        result[i++] = opcode;
        result[i++] = modrm;

        return result;
    }

    // MOVZX/MOVSX: memory to register (8-bit source)
    template<e_instruction_id Id, typename Dest, typename Src>
        requires Register<Dest> && Memory<Src> && (Dest::size >= 16) && (Src::size == 8)
    inline constexpr auto encode_movzx_movsx([[maybe_unused]] instruction_desc desc, const Dest& dest, const Src& src) {
        auto opcode = desc.primary_opcode(); // B6 or BE

        // ModR/M for memory operand
        auto modrm = encode_modrm(dest, src);

        constexpr bool need_rex_w = (Dest::size == 64);
        constexpr bool dest_extended = Dest::extended;
        constexpr bool src_base_extended = Register<typename Src::value_type> && Src::value_type::extended;
        constexpr bool need_rex = need_rex_w || dest_extended || src_base_extended;
        constexpr bool need_16bit_prefix = (Dest::size == 16);

        constexpr std::size_t total_size = need_16bit_prefix + need_rex + 2 + 1;
        std::array<std::uint8_t, total_size> result{};
        std::size_t i = 0;

        if constexpr (need_16bit_prefix) {
            result[i++] = 0x66;
        }

        if constexpr (need_rex) {
            std::uint8_t rex = 0x40;
            if constexpr (need_rex_w)
                rex |= 0x08;
            if constexpr (dest_extended)
                rex |= 0x04;
            if constexpr (src_base_extended)
                rex |= 0x01;
            result[i++] = rex;
        }

        result[i++] = 0x0F;
        result[i++] = opcode;
        result[i++] = modrm;

        return result;
    }

    // MOVZX/MOVSX: memory to register (16-bit source)
    template<e_instruction_id Id, typename Dest, typename Src>
        requires Register<Dest> && Memory<Src> && (Dest::size >= 32) && (Src::size == 16)
    inline constexpr auto encode_movzx_movsx([[maybe_unused]] instruction_desc desc, const Dest& dest, const Src& src) {
        auto opcode = static_cast<std::uint8_t>(desc.primary_opcode() + 1); // B7 or BF

        auto modrm = encode_modrm(dest, src);

        constexpr bool need_rex_w = (Dest::size == 64);
        constexpr bool dest_extended = Dest::extended;
        constexpr bool src_base_extended = Register<typename Src::value_type> && Src::value_type::extended;
        constexpr bool need_rex = need_rex_w || dest_extended || src_base_extended;

        constexpr std::size_t total_size = need_rex + 2 + 1;
        std::array<std::uint8_t, total_size> result{};
        std::size_t i = 0;

        if constexpr (need_rex) {
            std::uint8_t rex = 0x40;
            if constexpr (need_rex_w)
                rex |= 0x08;
            if constexpr (dest_extended)
                rex |= 0x04;
            if constexpr (src_base_extended)
                rex |= 0x01;
            result[i++] = rex;
        }

        result[i++] = 0x0F;
        result[i++] = opcode;
        result[i++] = modrm;

        return result;
    }

    // =========================================================================
    // MOVSXD - Move with Sign Extension (Doubleword to Quadword)
    // =========================================================================
    // MOVSXD r64, r/m32: REX.W + 63 /r
    //
    // This instruction only makes sense for 64-bit destination (extending 32-bit to 64-bit)

    // MOVSXD: register to register (32-bit source to 64-bit destination)
    template<e_instruction_id Id, typename Dest, typename Src>
        requires Register64<Dest> && Register32<Src>
    inline constexpr auto encode_movsxd([[maybe_unused]] instruction_desc desc, [[maybe_unused]] const Dest& dest, [[maybe_unused]] const Src& src) {
        auto opcode = desc.primary_opcode(); // 0x63

        // ModR/M: mod=11 (register), reg=destination, r/m=source
        auto modrm = static_cast<std::uint8_t>(0xC0 | ((static_cast<std::uint8_t>(Dest::id()) & 0x07) << 3) |
                                               (static_cast<std::uint8_t>(Src::id()) & 0x07));

        // REX.W is always needed for MOVSXD (64-bit destination)
        constexpr bool dest_extended = Dest::extended;
        constexpr bool src_extended = Src::extended;

        std::uint8_t rex = 0x48; // REX.W
        if constexpr (dest_extended)
            rex |= 0x04; // REX.R
        if constexpr (src_extended)
            rex |= 0x01; // REX.B

        return internal::make_array<std::uint8_t>(rex, opcode, modrm);
    }

    // MOVSXD: memory to register (32-bit source to 64-bit destination)
    template<e_instruction_id Id, typename Dest, typename Src>
        requires Register64<Dest> && Memory<Src> && (Src::size == 32)
    inline constexpr auto encode_movsxd([[maybe_unused]] instruction_desc desc, const Dest& dest, const Src& src) {
        auto opcode = desc.primary_opcode(); // 0x63

        auto modrm = encode_modrm(dest, src);

        constexpr bool dest_extended = Dest::extended;
        constexpr bool src_base_extended = Register<typename Src::value_type> && Src::value_type::extended;

        std::uint8_t rex = 0x48; // REX.W
        if constexpr (dest_extended)
            rex |= 0x04;
        if constexpr (src_base_extended)
            rex |= 0x01;

        return internal::make_array<std::uint8_t>(rex, opcode, modrm);
    }

    template<e_instruction_id Id, typename Op1, typename Op2>
    inline constexpr auto encode(const Op1& op1, const Op2& op2) {
        constexpr auto desc = find_instruction_desc<Id>();

        // Check for SIB memory operands first
        if constexpr (SIBMemory<Op1> || SIBMemory<Op2>) {
            if constexpr (desc.encoding() == e_encoding::alu) {
                return encode_alu_sib<Id>(desc, op1, op2);
            } else if constexpr (desc.encoding() == e_encoding::mov) {
                return encode_mov_sib<Id>(desc, op1, op2);
            } else if constexpr (desc.encoding() == e_encoding::lea) {
                return encode_lea_sib<Id>(desc, op1, op2);
            } else {
                static_assert(sizeof(Op1) == 0, "SIB addressing not supported for this instruction type");
            }
        } else if constexpr (desc.encoding() == e_encoding::alu) {
            return encode_alu<Id>(desc, op1, op2);
        } else if constexpr (desc.encoding() == e_encoding::bitscan) {
            return encode_bitscan<Id>(desc, op1, op2);
        } else if constexpr (desc.encoding() == e_encoding::bt) {
            return encode_bt<Id>(desc, op1, op2);
        } else if constexpr (desc.encoding() == e_encoding::cmov) {
            return encode_cmov<Id>(desc, op1, op2);
        } else if constexpr (desc.encoding() == e_encoding::lea) {
            return encode_lea<Id>(desc, op1, op2);
        } else if constexpr (desc.encoding() == e_encoding::mov) {
            return encode_mov<Id>(desc, op1, op2);
        } else if constexpr (desc.encoding() == e_encoding::shift) {
            if constexpr (IsCLRegister<Op2>) {
                return encode_shift_by_cl<Id>(desc, op1, op2);
            } else if constexpr (Immediate8<Op2>) {
                return encode_shift_by_imm<Id>(desc, op1, op2);
            } else if constexpr (Integer<Op2>) {
                return encode_shift<Id>(desc, op1, op2);
            } else {
                static_assert("Shift instruction requires CL or imm8 operand");
            }
        } else if constexpr (desc.encoding() == e_encoding::test) {
            return encode_test<Id>(desc, op1, op2);
        } else if constexpr (desc.encoding() == e_encoding::xchg) {
            return encode_xchg<Id>(desc, op1, op2);
        } else if constexpr (desc.encoding() == e_encoding::imul_two_op) {
            return encode_imul_two<Id>(desc, op1, op2);
        } else if constexpr (desc.encoding() == e_encoding::movzx_movsx) {
            return encode_movzx_movsx<Id>(desc, op1, op2);
        } else if constexpr (desc.encoding() == e_encoding::movsxd_enc) {
            return encode_movsxd<Id>(desc, op1, op2);
        } else {
            static_assert(sizeof(Op1) == 0, "Failed to encode instruction");
        }
    }

    template<e_instruction_id Id, typename Op1, typename Op2, typename Op3>
    inline constexpr auto encode(const Op1& op1, const Op2& op2, const Op3& op3) {
        constexpr auto desc = find_instruction_desc<Id>();

        if constexpr (desc.encoding() == e_encoding::imul_three_op) {
            return encode_imul_three<Id>(desc, op1, op2, op3);
        } else {
            static_assert("Failed to encode three-operand instruction");
        }
    }

    // INT imm8 - software interrupt
    // Must be defined before single-operand encode() due to two-phase lookup
    template<e_instruction_id Id>
    inline constexpr auto encode_int(std::uint8_t vector) {
        constexpr auto desc = find_instruction_desc<Id>();
        // CD ib
        return internal::make_array<std::uint8_t>(desc.primary_opcode(), vector);
    }

    // IRETQ - 64-bit interrupt return (needs REX.W prefix)
    inline constexpr auto encode_iretq() {
        // 48 CF - REX.W + IRET opcode
        return internal::make_array<std::uint8_t>(0x48, 0xCF);
    }

    template<e_instruction_id Id, typename Op1>
    inline constexpr auto encode(const Op1& op1) {
        constexpr auto desc = find_instruction_desc<Id>();

        if constexpr (desc.encoding() == e_encoding::ret) {
            return encode_ret<Id>(desc, op1);
        } else if constexpr (desc.encoding() == e_encoding::bswap) {
            return encode_bswap<Id>(desc, op1);
        } else if constexpr (desc.encoding() == e_encoding::call) {
            return encode_call<Id>(desc, op1);
        } else if constexpr (desc.encoding() == e_encoding::pop) {
            return encode_pop<Id>(desc, op1);
        } else if constexpr (desc.encoding() == e_encoding::push) {
            return encode_push<Id>(desc, op1);
        } else if constexpr (desc.encoding() == e_encoding::jcc) {
            return encode_jcc<Id>(desc, op1);
        } else if constexpr (desc.encoding() == e_encoding::jcc_near) {
            return encode_jcc_near<Id>(desc, op1);
        } else if constexpr (desc.encoding() == e_encoding::jmp) {
            return encode_jmp<Id>(desc, op1);
        } else if constexpr (desc.encoding() == e_encoding::unary) {
            return encode_unary<Id>(desc, op1);
        } else if constexpr (desc.encoding() == e_encoding::muldiv) {
            return encode_muldiv<Id>(desc, op1);
        } else if constexpr (desc.encoding() == e_encoding::shift) {
            // Single-operand shift: shift by 1
            return encode_shift_by_one<Id>(desc, op1);
        } else if constexpr (desc.encoding() == e_encoding::int_imm) {
            // INT imm8
            return encode_int<Id>(static_cast<std::uint8_t>(op1));
        } else {
            static_assert("Failed to encode instruction");
        }
    }

    template<e_instruction_id Id>
    inline constexpr auto encode() {
        constexpr auto desc = find_instruction_desc<Id>();
        constexpr auto size = (desc.prefix() != 0) + (desc.prefix_0f() != 0) + 1 /* _primary_opcode */ + (desc.secondary_opcode() != 0);

        std::array<std::uint8_t, size> arr{};

        int i = 0;

        if (desc.prefix() != 0) {
            arr[i++] = desc.prefix();
        }

        if (desc.prefix_0f() != 0) {
            arr[i++] = desc.prefix_0f();
        }

        if (desc.primary_opcode() != 0) {
            arr[i++] = desc.primary_opcode();
        }

        if (desc.secondary_opcode() != 0) {
            arr[i++] = desc.secondary_opcode();
        }

        return arr;
    }

    // String instruction encoder for qword variants (need REX.W prefix)
    template<e_instruction_id Id>
    inline constexpr auto encode_string_q() {
        constexpr auto desc = find_instruction_desc<Id>();
        // REX.W (0x48) + opcode
        return internal::make_array<std::uint8_t>(0x48, desc.primary_opcode());
    }

    // REP prefix constants
    namespace rep_prefix {
        inline constexpr std::uint8_t rep = 0xF3; // REP/REPE/REPZ
        inline constexpr std::uint8_t repe = 0xF3; // Same as REP
        inline constexpr std::uint8_t repz = 0xF3; // Same as REP
        inline constexpr std::uint8_t repne = 0xF2; // REPNE/REPNZ
        inline constexpr std::uint8_t repnz = 0xF2; // Same as REPNE
    } // namespace rep_prefix

    // Helper to prepend REP prefix to string instruction
    template<std::size_t N>
    inline constexpr auto with_rep_prefix(std::uint8_t prefix, const std::array<std::uint8_t, N>& instr) {
        std::array<std::uint8_t, N + 1> result{};
        result[0] = prefix;
        for (std::size_t i = 0; i < N; ++i) {
            result[i + 1] = instr[i];
        }
        return result;
    }

    // LOCK prefix (0xF0) - makes a read-modify-write memory instruction atomic
    inline constexpr std::uint8_t lock_prefix = 0xF0;

    // Helper to prepend the LOCK prefix to an already-encoded instruction.
    template<std::size_t N>
    inline constexpr auto with_lock_prefix(const std::array<std::uint8_t, N>& instr) {
        std::array<std::uint8_t, N + 1> result{};
        result[0] = lock_prefix;
        for (std::size_t i = 0; i < N; ++i) {
            result[i + 1] = instr[i];
        }
        return result;
    }

    template<e_instruction_id Id, typename... Args>
    inline constexpr auto encode(Args&&... args) {
        return encode<Id, Args...>(std::forward<Args>(args)...);
    }
} // namespace static_asm::x86