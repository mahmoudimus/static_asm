#pragma once

#include "opcode_extension.hpp"
#include "operands.hpp"

namespace static_asm::x86 {

    enum class e_mod : std::uint8_t {
        register_indirect_addressing = 0b00,
        one_byte_signed_displacement = 0b01,
        four_byte_signed_displacement = 0b10,
        register_addressing = 0b11
    };

    template<typename Reg1, typename Reg2>
        requires Register<Reg1> && Register<Reg2>
    inline constexpr std::uint8_t encode_modrm(e_mod mod, const Reg1& reg1, const Reg2& reg2) {
        return (static_cast<std::uint8_t>(mod) << 6) + ((static_cast<std::uint8_t>(reg2.id()) & 0b111) << 3) + (static_cast<std::uint8_t>(reg1.id()) & 0b111);
    }

    template<typename Reg1, typename Reg2>
        requires Register<Reg1> && Register<Reg2>
    inline constexpr std::uint8_t encode_modrm(const Reg1& reg1, const Reg2& reg2) {
        return encode_modrm(e_mod::register_addressing, reg1, reg2);
    }

    template<typename Reg>
        requires Register<Reg>
    inline constexpr std::uint8_t encode_modrm(e_mod mod, const Reg& reg) {
        return (static_cast<std::uint8_t>(mod) << 6) + ((static_cast<std::uint8_t>(000) & 0b111) << 3) + (static_cast<std::uint8_t>(reg.id()) & 0b111);
    }

    template<typename Reg>
        requires Register<Reg>
    inline constexpr std::uint8_t encode_modrm(const Reg& reg) {
        return encode_modrm(e_mod::register_addressing, reg);
    }

    template<typename Mem>
        requires Memory<Mem> && Register<typename Mem::value_type>
    inline constexpr std::uint8_t encode_modrm(const Mem& mem) {
        return encode_modrm(e_mod::register_indirect_addressing, mem.value());
    }

    template<typename Reg, typename Mem>
        requires Register<Reg> && Memory<Mem> && Register<typename Mem::value_type>
    inline constexpr std::uint8_t encode_modrm(e_mod mod, const Reg& reg, const Mem& mem) {
        return (static_cast<std::uint8_t>(mod) << 6) + ((static_cast<std::uint8_t>(reg.id()) & 0b111) << 3) + (static_cast<std::uint8_t>(mem.value().id()) & 0b111);
    }

    template<typename Reg, typename Mem>
        requires Register<Reg> && Memory<Mem> && Register<typename Mem::value_type>
    inline constexpr std::uint8_t encode_modrm(const Reg& reg, const Mem& mem) {
        return encode_modrm(e_mod::register_indirect_addressing, reg, mem);
    }

    template<typename Mem, typename Reg>
        requires Memory<Mem> && Register<typename Mem::value_type> && Register<Reg>
    inline constexpr std::uint8_t encode_modrm(e_mod mod, const Mem& mem, const Reg& reg) {
        return (static_cast<std::uint8_t>(mod) << 6) + ((static_cast<std::uint8_t>(reg.id()) & 0b111) << 3) + (static_cast<std::uint8_t>(mem.value().id()) & 0b111);
    }

    template<typename Mem, typename Reg>
        requires Memory<Mem> && Register<typename Mem::value_type> && Register<Reg>
    inline constexpr std::uint8_t encode_modrm(const Mem& mem, const Reg& reg) {
        return encode_modrm(e_mod::register_indirect_addressing, mem, reg);
    }

    template<typename Mem, typename Reg>
        requires Memory<Mem> && Immediate<typename Mem::value_type> && Register<Reg>
    inline constexpr std::uint8_t encode_modrm(e_mod mod, const Mem& mem, const Reg& reg) {
        return (static_cast<std::uint8_t>(mod) << 6) + ((static_cast<std::uint8_t>(reg.id()) & 0b111) << 3) + (static_cast<std::uint8_t>(0b100) & 0b111);
    }

    template<typename Mem, typename Reg>
        requires Memory<Mem> && Immediate<typename Mem::value_type> && Register<Reg>
    inline constexpr std::uint8_t encode_modrm(const Mem& mem, const Reg& reg) {
        return encode_modrm(e_mod::register_indirect_addressing, mem, reg);
    }

    template<typename Reg, typename Mem>
        requires Register<Reg> && Memory<Mem> && Immediate<typename Mem::value_type>
    inline constexpr std::uint8_t encode_modrm(const Reg& reg, const Mem& mem) {
        return encode_modrm(e_mod::register_indirect_addressing, mem, reg);
    }

    template<typename Reg>
        requires Register<Reg>
    inline constexpr std::uint8_t encode_modrm_ext(e_mod mod, std::uint8_t ext, const Reg& reg) {
        return (static_cast<std::uint8_t>(mod) << 6) + ((ext & 0b111) << 3) + (static_cast<std::uint8_t>(reg.id()) & 0b111);
    }

    template<typename Reg, typename Imm>
        requires Register<Reg> && Immediate<Imm>
    inline constexpr std::uint8_t encode_modrm(e_opcode_alu_extension ext, const Reg& reg, const Imm& imm) {
        return encode_modrm_ext(e_mod::register_addressing, static_cast<std::uint8_t>(ext), reg);
    }

    template<typename Mem, typename Imm>
        requires Memory<Mem> && Register<typename Mem::value_type> && Immediate<Imm>
    inline constexpr std::uint8_t encode_modrm(e_opcode_alu_extension ext, const Mem& mem, const Imm& imm) {
        return encode_modrm_ext(e_mod::register_indirect_addressing, static_cast<std::uint8_t>(ext), mem.value());
    }

    template<typename Reg, typename Imm>
        requires Register<Reg> && Immediate<Imm>
    inline constexpr std::uint8_t encode_modrm(e_mod mod, e_opcode_alu_extension ext, const Reg& reg, const Imm& imm) {
        return (static_cast<std::uint8_t>(mod) << 6) + ((static_cast<std::uint8_t>(ext) & 0b111) << 3) + (static_cast<std::uint8_t>(reg.id()) & 0b111);
    }

    template<typename Reg, typename Imm>
        requires Register<Reg> && Immediate<Imm>
    inline constexpr std::uint8_t encode_modrm(e_opcode_bt_extension ext, const Reg& reg, const Imm& imm) {
        return encode_modrm_ext(e_mod::register_addressing, static_cast<std::uint8_t>(ext), reg);
    }

    template<typename Reg>
        requires Register<Reg>
    inline constexpr std::uint8_t encode_modrm(e_opcode_ff_extension ext, const Reg& reg) {
        return encode_modrm_ext(e_mod::register_addressing, static_cast<std::uint8_t>(ext), reg);
    }

    template<typename Mem>
        requires Memory<Mem>
    inline constexpr std::uint8_t encode_modrm(e_opcode_ff_extension ext, const Mem& mem) {
        return encode_modrm_ext(e_mod::register_indirect_addressing, static_cast<std::uint8_t>(ext), mem.value());
    }

    // =========================================================================
    // ModR/M Encoding for SIB Addressing
    // =========================================================================

    // When using SIB addressing, the r/m field is set to 100 (indicating SIB follows)
    // The mod field indicates:
    //   00 = [base + index*scale] (no displacement, or disp32 if base=RBP/R13)
    //   01 = [base + index*scale + disp8]
    //   10 = [base + index*scale + disp32]

    // Helper to check if a register id corresponds to RBP/R13 (register code 5)
    inline constexpr bool is_rbp_or_r13(e_register_id id) {
        return id == e_register_id::bp || id == e_register_id::r13;
    }

    // Encode ModR/M for SIB memory with register operand
    template<typename Reg, typename SIBMem>
        requires Register<Reg> && SIBMemory<SIBMem>
    inline constexpr std::uint8_t encode_modrm_sib(const Reg& reg, const SIBMem& mem) {
        // Determine mod field based on displacement type and base register
        e_mod mod = e_mod::register_indirect_addressing; // mod=00

        constexpr e_displacement_type disp_type = SIBMem::disp_type;

        if constexpr (disp_type == e_displacement_type::disp8) {
            mod = e_mod::one_byte_signed_displacement; // mod=01
        } else if constexpr (disp_type == e_displacement_type::disp32) {
            mod = e_mod::four_byte_signed_displacement; // mod=10
        } else if constexpr (SIBMem::has_base) {
            // Check if base is RBP/R13 at runtime (but still constexpr-evaluable)
            // RBP/R13 with mod=00 means disp32 only (no base), so we need disp8=0
            if (is_rbp_or_r13(mem.base().id())) {
                mod = e_mod::one_byte_signed_displacement; // Force mod=01 with disp8=0
            }
        }

        // r/m = 100 indicates SIB byte follows
        return (static_cast<std::uint8_t>(mod) << 6) | ((static_cast<std::uint8_t>(reg.id()) & 0b111) << 3) | 0b100; // SIB follows
    }

    // Encode ModR/M for SIB memory as destination (memory, register order)
    template<typename SIBMem, typename Reg>
        requires SIBMemory<SIBMem> && Register<Reg>
    inline constexpr std::uint8_t encode_modrm_sib(const SIBMem& mem, const Reg& reg) {
        return encode_modrm_sib(reg, mem);
    }

    // Check if RBP/R13 base needs forced disp8 - must be called with actual operand
    template<typename SIBMem>
        requires SIBMemory<SIBMem>
    inline constexpr bool sib_needs_forced_disp8(const SIBMem& mem) {
        if constexpr (!SIBMem::has_base) {
            return false;
        }
        if constexpr (SIBMem::disp_type != e_displacement_type::disp0) {
            return false; // Already has displacement
        }
        // Check if base is RBP/R13
        return is_rbp_or_r13(mem.base().id());
    }
} // namespace static_asm::x86