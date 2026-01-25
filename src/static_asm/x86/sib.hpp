#pragma once

// Note: This header depends on operands.hpp being included first.
// The SIBMemory concept and Register concept must be defined before use.

namespace static_asm::x86 {

    // Encode scale value to 2-bit field
    inline constexpr std::uint8_t encode_scale(int scale) {
        switch (scale) {
        case 1: return 0b00;
        case 2: return 0b01;
        case 4: return 0b10;
        case 8: return 0b11;
        default: return 0; // Should not happen with proper constraints
        }
    }

    // SIB byte format: [Scale:2][Index:3][Base:3]
    // Encode SIB byte for general SIB addressing
    // Now uses compile-time register IDs via Reg::id()
    template<typename Base, typename Index, int Scale>
        requires(Register<Base> || std::same_as<Base, no_base_t>) && (Register<Index> || std::same_as<Index, no_index_t>)
    inline constexpr std::uint8_t encode_sib([[maybe_unused]] const Base& base, [[maybe_unused]] const Index& index) {
        std::uint8_t scale_bits = encode_scale(Scale);
        std::uint8_t index_bits = 0b100; // Default: no index (RSP encoding)
        std::uint8_t base_bits = 0b101; // Default: no base (disp32 only)

        if constexpr (Register<Index>) {
            index_bits = static_cast<std::uint8_t>(Index::id()) & 0b111;
        }

        if constexpr (Register<Base>) {
            base_bits = static_cast<std::uint8_t>(Base::id()) & 0b111;
        }

        return (scale_bits << 6) | (index_bits << 3) | base_bits;
    }

    // Overload for SIB memory operand
    template<typename SIBMem>
        requires SIBMemory<SIBMem>
    inline constexpr std::uint8_t encode_sib(const SIBMem& mem) {
        return encode_sib<typename SIBMem::base_type, typename SIBMem::index_type, SIBMem::scale>(
            mem.base(), mem.index());
    }

    // Legacy function for disp32-only addressing (preserved for compatibility)
    template<typename Op1, typename Op2>
    inline constexpr std::uint8_t encode_sib_nodisp([[maybe_unused]] const Op1& op1, [[maybe_unused]] const Op2& op2) {
        return (static_cast<std::uint8_t>(0b00) << 6) // Scale = 1
               + ((static_cast<std::uint8_t>(0b100) & 0b111) << 3) // index = RSP (no index)
               + (static_cast<std::uint8_t>(0b101) & 0b111); // base = RBP (disp32 follows)
    }

    // Check if REX.X is needed (index register is r8-r15)
    template<typename Index>
        requires Register<Index> || std::same_as<Index, no_index_t>
    inline constexpr bool needs_rex_x() {
        if constexpr (Register<Index>) {
            return Index::extended;
        }
        return false;
    }

    // Check if REX.B is needed for SIB base (base register is r8-r15)
    template<typename Base>
        requires Register<Base> || std::same_as<Base, no_base_t>
    inline constexpr bool needs_rex_b_sib() {
        if constexpr (Register<Base>) {
            return Base::extended;
        }
        return false;
    }

} // namespace static_asm::x86
