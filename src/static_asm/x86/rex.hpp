#pragma once

namespace static_asm::x86 {

    template <typename Reg1, typename Reg2> requires Register<Reg1> && Register<Reg2>
    inline constexpr std::uint8_t encode_rex() {
        return (0b0100 << 4) // fixed value
               + ((Reg1::size >= 64 || Reg2::size >= 64) << 3) // Rex.W
               + ((Reg2::extended ? 1 : 0) << 2) // Rex.R
               + (0 << 1) // Rex.X
               + (Reg1::extended ? 1 : 0); // Rex.B
    }

    template <typename Mem, typename Reg> requires Memory<Mem> && Register<typename Mem::value_type> && Register<Reg>
    inline constexpr std::uint8_t encode_rex() {
        return encode_rex<typename Mem::value_type, Reg>();
    }

    template <typename Reg, typename Mem> requires Register<Reg> && Memory<Mem> && Register<typename Mem::value_type>
    inline constexpr std::uint8_t encode_rex() {
        // For (Reg, Mem) order: Reg goes in reg field (REX.R), Mem base goes in r/m field (REX.B)
        // REX.W is based on the register operand size, not the memory base size
        return (0b0100 << 4) // fixed value
               + ((Reg::size >= 64) << 3) // Rex.W - based on register operand size
               + ((Reg::extended ? 1 : 0) << 2) // Rex.R - extended destination register
               + (0 << 1) // Rex.X
               + (Mem::value_type::extended ? 1 : 0); // Rex.B - extended memory base
    }

    template <typename Reg1> requires Register<Reg1>
    inline constexpr std::uint8_t encode_rex() {
        return (0b0100 << 4) // fixed value
               + ((Reg1::size >= 64) << 3) // Rex.W
               + (0 << 2) // Rex.R
               + (0 << 1) // Rex.X
               + (Reg1::extended ? 1 : 0); // Rex.B
    }

    template <typename Mem> requires Memory<Mem>
    inline constexpr std::uint8_t encode_rex() {
        return (0b0100 << 4) // fixed value
               + ((Mem::size >= 64) << 3) // Rex.W
               + (0 << 2) // Rex.R
               + (0 << 1) // Rex.X
               + (0); // Rex.B
    }

    template <typename Imm> requires Immediate<Imm>
    inline constexpr std::uint8_t encode_rex() {
        return 0; // todo
    }

    template <typename Mem, typename Reg> requires Memory<Mem> && Immediate<typename Mem::value_type> && Register<Reg>
    inline constexpr std::uint8_t encode_rex() {
        return (0b0100 << 4) // fixed value
               + ((Reg::size >= 64) << 3) // Rex.W
               + (0 << 2) // Rex.R
               + (0 << 1) // Rex.X
               + (Reg::extended ? 1 : 0); // Rex.B
    }

    template <typename Reg, typename Mem> requires Register<Reg> && Memory<Mem> && Immediate<typename Mem::value_type>
    inline constexpr std::uint8_t encode_rex() {
        return (0b0100 << 4) // fixed value
               + ((Reg::size >= 64) << 3) // Rex.W
               + ((Reg::extended ? 1 : 0) << 2) // Rex.R
               + (0 << 1) // Rex.X
               + (0); // Rex.B
    }

    // REX prefix with only B bit (no W) - for jmp/call with extended registers
    template <typename Reg1> requires Register<Reg1>
    inline constexpr std::uint8_t encode_rex_b_only() {
        return (0b0100 << 4) // fixed value
               + (0 << 3) // Rex.W = 0
               + (0 << 2) // Rex.R
               + (0 << 1) // Rex.X
               + (Reg1::extended ? 1 : 0); // Rex.B
    }

    template <typename Reg1, typename Reg2> requires Register<Reg1> && Register<Reg2>
    inline constexpr bool needs_rex() {
        return Reg1::extended || Reg2::extended || Reg1::size >= 64 || Reg2::size >= 64;
    }

    template <typename Reg1> requires Register<Reg1>
    inline constexpr bool needs_rex() {
        return Reg1::extended || Reg1::size >= 64;
    }

    // Check if register is extended (r8-r15) - needs REX.B
    template <typename Reg1> requires Register<Reg1>
    inline constexpr bool needs_rex_extended() {
        return Reg1::extended;
    }

    template <typename Mem> requires Memory<Mem> && Register<typename Mem::value_type>
    inline constexpr bool needs_rex() {
        return Mem::size >= 64;
    }

    template <typename Imm> requires Immediate<Imm>
    inline constexpr bool needs_rex() {
        return sizeof(Imm) >= 64;
    }

    template <typename Mem, typename Reg> requires Memory<Mem> && Register<Reg>
    inline constexpr bool needs_rex() {
        return Reg::extended || Reg::size >= 64;
    }

    template <typename Reg, typename Mem> requires Register<Reg> && Memory<Mem>
    inline constexpr bool needs_rex() {
        return needs_rex<Mem, Reg>();
    }

    // =========================================================================
    // REX Encoding for SIB Addressing
    // =========================================================================

    // REX prefix for SIB addressing: REX.W + REX.R + REX.X + REX.B
    // - REX.W: 64-bit operand size
    // - REX.R: Extension of reg field (destination register)
    // - REX.X: Extension of index field in SIB
    // - REX.B: Extension of base field in SIB (or r/m field)
    template <typename Reg, typename Base, typename Index>
        requires Register<Reg>
              && (Register<Base> || std::same_as<Base, no_base_t>)
              && (Register<Index> || std::same_as<Index, no_index_t>)
    inline constexpr std::uint8_t encode_rex_sib() {
        constexpr bool rex_w = (Reg::size >= 64);
        constexpr bool rex_r = Reg::extended;
        constexpr bool rex_x = []() {
            if constexpr (Register<Index>) {
                return Index::extended;
            }
            return false;
        }();
        constexpr bool rex_b = []() {
            if constexpr (Register<Base>) {
                return Base::extended;
            }
            return false;
        }();

        return (0b0100 << 4)
             | (rex_w << 3)
             | (rex_r << 2)
             | (rex_x << 1)
             | (rex_b);
    }

    // Check if REX is needed for SIB addressing
    template <typename Reg, typename Base, typename Index>
        requires Register<Reg>
              && (Register<Base> || std::same_as<Base, no_base_t>)
              && (Register<Index> || std::same_as<Index, no_index_t>)
    inline constexpr bool needs_rex_sib() {
        constexpr bool need_w = (Reg::size >= 64);
        constexpr bool need_r = Reg::extended;
        constexpr bool need_x = []() {
            if constexpr (Register<Index>) {
                return Index::extended;
            }
            return false;
        }();
        constexpr bool need_b = []() {
            if constexpr (Register<Base>) {
                return Base::extended;
            }
            return false;
        }();

        return need_w || need_r || need_x || need_b;
    }

    // REX prefix for SIB with SIBMemory operand type
    template <typename Reg, typename SIBMem>
        requires Register<Reg> && SIBMemory<SIBMem>
    inline constexpr std::uint8_t encode_rex_sib_mem() {
        return encode_rex_sib<Reg, typename SIBMem::base_type, typename SIBMem::index_type>();
    }

    template <typename Reg, typename SIBMem>
        requires Register<Reg> && SIBMemory<SIBMem>
    inline constexpr bool needs_rex_sib_mem() {
        return needs_rex_sib<Reg, typename SIBMem::base_type, typename SIBMem::index_type>();
    }

}