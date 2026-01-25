#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>

namespace static_asm::x86 {

    // =========================================================================
    // Compile-time validation helpers
    // =========================================================================

    // Valid x86 operand sizes in bits
    template<std::size_t Size>
    concept ValidOperandSize = (Size == 8 || Size == 16 || Size == 32 || Size == 64);

    // Valid x86 SIB scale factors
    template<int Scale>
    concept ValidScale = (Scale == 1 || Scale == 2 || Scale == 4 || Scale == 8);

    struct void_operand {
        static constexpr bool extended = false;
        static constexpr std::size_t size = 0;

        using value_type = void;
    };

    using Void = void_operand;

    template<typename T>
    concept IsVoidOperand = std::is_void_v<typename T::value_type>;

    // Tag type representing "current instruction address" ($ in assembly)
    // Used for jmp(here) which encodes as EB FE (jmp rel8 -2)
    struct here_t {
        static constexpr bool extended = false;
        static constexpr std::size_t size = 0;
    };
    inline constexpr here_t here{};

    template<typename T>
    concept IsHere = std::same_as<std::remove_cvref_t<T>, here_t>;

    class base_operand {
    public:
        enum class e_operand_type {
            reg,
            mem,
            imm
        };

    public:
        constexpr base_operand() = delete;

        constexpr base_operand(e_operand_type type)
            : _type(type) {}

        constexpr bool is_mem() const {
            return _type == e_operand_type::mem;
        }
        constexpr bool is_reg() const {
            return _type == e_operand_type::reg;
        }
        constexpr bool is_imm() const {
            return _type == e_operand_type::imm;
        }

        constexpr e_operand_type type() const {
            return _type;
        }

        constexpr virtual ~base_operand() {}

    private:
        e_operand_type _type;
    };

    template<typename T>
    concept IsBaseOperand = std::same_as<base_operand, T>;

    template<typename T>
    concept DerivesBaseOperand = !IsBaseOperand<T> && std::is_base_of_v<base_operand, T>;

    enum class e_register_id {
        ax,
        cx,
        dx,
        bx,
        sp,
        bp,
        si,
        di,
        r8,
        r9,
        r10,
        r11,
        r12,
        r13,
        r14,
        r15,
        ip,

        unknown
    };

    // =========================================================================
    // Register Operand - ID is now a template parameter for full compile-time
    // =========================================================================
    template<e_register_id ID, std::size_t Size, bool Extended>
    class register_operand : base_operand {
    public:
        constexpr register_operand()
            : base_operand(e_operand_type::reg) {}

        constexpr ~register_operand() = default;

        // Static id accessor - returns the compile-time ID
        static constexpr e_register_id id() {
            return ID;
        }

        // Also provide as a static member for template metaprogramming
        static constexpr e_register_id id_value = ID;

        constexpr bool is8() const {
            return Size == 8;
        };
        constexpr bool is16() const {
            return Size == 16;
        };
        constexpr bool is32() const {
            return Size == 32;
        };
        constexpr bool is64() const {
            return Size == 64;
        };

        static constexpr bool extended = Extended;
        static constexpr std::size_t size = Size;

        static constexpr bool is_ip() {
            return ID == e_register_id::ip;
        }

        using value_type = register_operand<ID, Size, Extended>;
    };

    enum class e_register8bit_id {
        al,
        cl,
        dl,
        bl,
        ah,
        ch,
        dh,
        bh
    };

    // Special 8-bit register operand for legacy al/ah/bl/bh/etc registers
    // These have their own ID space (e_register8bit_id)
    template<e_register8bit_id ID>
    class register8bit_operand : public register_operand<e_register_id::unknown, 8, false> {
    public:
        constexpr register8bit_operand()
            : register_operand<e_register_id::unknown, 8, false>() {}

        constexpr ~register8bit_operand() = default;

        // Return the 8-bit specific ID
        static constexpr e_register8bit_id id() {
            return ID;
        }

        static constexpr e_register8bit_id id_value = ID;

        // override extended static member
        static constexpr bool extended = false;
        // override size static member
        static constexpr std::size_t size = 8;

        using value_type = register8bit_operand<ID>;
    };

    // =========================================================================
    // Type aliases for register operands (generic forms)
    // =========================================================================
    template<e_register_id ID, std::size_t Size, bool Extended>
    using reg = register_operand<ID, Size, Extended>;

    // Generic register type aliases (for backward compatibility in templates)
    // These are now parameterized by ID
    template<e_register_id ID>
    using reg8 = reg<ID, 8, false>;

    template<e_register_id ID>
    using reg16 = reg<ID, 16, false>;

    template<e_register_id ID>
    using reg32 = reg<ID, 32, false>;

    template<e_register_id ID>
    using reg64 = reg<ID, 64, false>;

    template<e_register_id ID>
    using ereg8 = reg<ID, 8, true>;

    template<e_register_id ID>
    using ereg16 = reg<ID, 16, true>;

    template<e_register_id ID>
    using ereg32 = reg<ID, 32, true>;

    template<e_register_id ID>
    using ereg64 = reg<ID, 64, true>;

    template<e_register8bit_id ID>
    using reg8lh = register8bit_operand<ID>;

    // =========================================================================
    // Concrete register type aliases for each physical register
    // =========================================================================

    // RAX family
    using rax_t = reg<e_register_id::ax, 64, false>;
    using eax_t = reg<e_register_id::ax, 32, false>;
    using ax_t = reg<e_register_id::ax, 16, false>;
    using al_t = register8bit_operand<e_register8bit_id::al>;
    using ah_t = register8bit_operand<e_register8bit_id::ah>;

    // RBX family
    using rbx_t = reg<e_register_id::bx, 64, false>;
    using ebx_t = reg<e_register_id::bx, 32, false>;
    using bx_t = reg<e_register_id::bx, 16, false>;
    using bl_t = register8bit_operand<e_register8bit_id::bl>;
    using bh_t = register8bit_operand<e_register8bit_id::bh>;

    // RCX family
    using rcx_t = reg<e_register_id::cx, 64, false>;
    using ecx_t = reg<e_register_id::cx, 32, false>;
    using cx_t = reg<e_register_id::cx, 16, false>;
    using cl_t = register8bit_operand<e_register8bit_id::cl>;
    using ch_t = register8bit_operand<e_register8bit_id::ch>;

    // RDX family
    using rdx_t = reg<e_register_id::dx, 64, false>;
    using edx_t = reg<e_register_id::dx, 32, false>;
    using dx_t = reg<e_register_id::dx, 16, false>;
    using dl_t = register8bit_operand<e_register8bit_id::dl>;
    using dh_t = register8bit_operand<e_register8bit_id::dh>;

    // RSI family
    using rsi_t = reg<e_register_id::si, 64, false>;
    using esi_t = reg<e_register_id::si, 32, false>;
    using si_t = reg<e_register_id::si, 16, false>;
    using sil_t = reg<e_register_id::si, 8, true>;

    // RDI family
    using rdi_t = reg<e_register_id::di, 64, false>;
    using edi_t = reg<e_register_id::di, 32, false>;
    using di_t = reg<e_register_id::di, 16, false>;
    using dil_t = reg<e_register_id::di, 8, true>;

    // RBP family
    using rbp_t = reg<e_register_id::bp, 64, false>;
    using ebp_t = reg<e_register_id::bp, 32, false>;
    using bp_t = reg<e_register_id::bp, 16, false>;
    using bpl_t = reg<e_register_id::bp, 8, true>;

    // RSP family
    using rsp_t = reg<e_register_id::sp, 64, false>;
    using esp_t = reg<e_register_id::sp, 32, false>;
    using sp_t = reg<e_register_id::sp, 16, false>;
    using spl_t = reg<e_register_id::sp, 8, true>;

    // R8 family
    using r8_t = reg<e_register_id::r8, 64, true>;
    using r8d_t = reg<e_register_id::r8, 32, true>;
    using r8w_t = reg<e_register_id::r8, 16, true>;
    using r8b_t = reg<e_register_id::r8, 8, true>;

    // R9 family
    using r9_t = reg<e_register_id::r9, 64, true>;
    using r9d_t = reg<e_register_id::r9, 32, true>;
    using r9w_t = reg<e_register_id::r9, 16, true>;
    using r9b_t = reg<e_register_id::r9, 8, true>;

    // R10 family
    using r10_t = reg<e_register_id::r10, 64, true>;
    using r10d_t = reg<e_register_id::r10, 32, true>;
    using r10w_t = reg<e_register_id::r10, 16, true>;
    using r10b_t = reg<e_register_id::r10, 8, true>;

    // R11 family
    using r11_t = reg<e_register_id::r11, 64, true>;
    using r11d_t = reg<e_register_id::r11, 32, true>;
    using r11w_t = reg<e_register_id::r11, 16, true>;
    using r11b_t = reg<e_register_id::r11, 8, true>;

    // R12 family
    using r12_t = reg<e_register_id::r12, 64, true>;
    using r12d_t = reg<e_register_id::r12, 32, true>;
    using r12w_t = reg<e_register_id::r12, 16, true>;
    using r12b_t = reg<e_register_id::r12, 8, true>;

    // R13 family
    using r13_t = reg<e_register_id::r13, 64, true>;
    using r13d_t = reg<e_register_id::r13, 32, true>;
    using r13w_t = reg<e_register_id::r13, 16, true>;
    using r13b_t = reg<e_register_id::r13, 8, true>;

    // R14 family
    using r14_t = reg<e_register_id::r14, 64, true>;
    using r14d_t = reg<e_register_id::r14, 32, true>;
    using r14w_t = reg<e_register_id::r14, 16, true>;
    using r14b_t = reg<e_register_id::r14, 8, true>;

    // R15 family
    using r15_t = reg<e_register_id::r15, 64, true>;
    using r15d_t = reg<e_register_id::r15, 32, true>;
    using r15w_t = reg<e_register_id::r15, 16, true>;
    using r15b_t = reg<e_register_id::r15, 8, true>;

    // RIP/EIP
    using rip_t = reg<e_register_id::ip, 64, false>;
    using eip_t = reg<e_register_id::ip, 32, false>;

    // =========================================================================
    // Register Concepts - Unified hierarchy for x86 register operands
    // =========================================================================
    //
    // Hierarchy:
    //   Register (base) - any register operand
    //     RegisterOfSize<N> - register of specific bit width (8, 16, 32, 64)
    //       ExtendedRegister<N> - r8-r15 variants (require REX prefix)
    //       LegacyRegister<N> - original x86/x86-64 registers
    //
    // Convenience aliases: Register8, Register16, Register32, Register64
    //                      LRegister8, LRegister16, etc.
    //                      ERegister8, ERegister16, etc.

    // Internal concept - matches exact register_operand type
    // Updated to work with the new template structure
    template<typename T>
    concept _RegisterOperand = requires {
        { T::id_value } -> std::convertible_to<e_register_id>;
        { T::extended } -> std::convertible_to<bool>;
        { T::size } -> std::convertible_to<std::size_t>;
    };

    // Check if type is a register8bit_operand (al/ah/bl/bh/etc)
    template<typename T>
    concept _Register8bitOperand = requires {
        { T::id_value } -> std::convertible_to<e_register8bit_id>;
        { T::extended } -> std::convertible_to<bool>;
        { T::size } -> std::convertible_to<std::size_t>;
    } && (T::size == 8) && (!T::extended);

    // Base concept: any register operand (including special 8-bit al/ah/etc)
    template<typename T>
    concept Register = _RegisterOperand<T> || _Register8bitOperand<T>;

    // Parameterized concept: register of specific size (8, 16, 32, or 64 bits)
    template<typename T, std::size_t Size>
    concept RegisterOfSize = Register<T> && (T::size == Size);

    // Extended registers (r8-r15 and their sub-registers) - require REX.B prefix
    template<typename T, std::size_t Size>
    concept ExtendedRegister = Register<T> && (T::size == Size) && T::extended;

    // Legacy registers (rax-rdi and their sub-registers) - no REX.B needed
    template<typename T, std::size_t Size>
    concept LegacyRegister = Register<T> && (T::size == Size) && !T::extended;

    // Size-specific legacy register concepts
    template<typename T>
    concept LRegister64 = LegacyRegister<T, 64>;

    template<typename T>
    concept LRegister32 = LegacyRegister<T, 32>;

    template<typename T>
    concept LRegister16 = LegacyRegister<T, 16>;

    template<typename T>
    concept LRegister8 = _Register8bitOperand<T>;

    // Size-specific extended register concepts
    template<typename T>
    concept ERegister64 = ExtendedRegister<T, 64>;

    template<typename T>
    concept ERegister32 = ExtendedRegister<T, 32>;

    template<typename T>
    concept ERegister16 = ExtendedRegister<T, 16>;

    template<typename T>
    concept ERegister8 = ExtendedRegister<T, 8>;

    // Size-specific register concepts (any extended or legacy)
    template<typename T>
    concept Register64 = RegisterOfSize<T, 64>;

    template<typename T>
    concept Register32 = RegisterOfSize<T, 32>;

    template<typename T>
    concept Register16 = RegisterOfSize<T, 16>;

    template<typename T>
    concept Register8 = RegisterOfSize<T, 8>;

    // Concept to check if a register is the CL register (used for shift/rotate by CL)
    template<typename T>
    concept IsCLRegister = LRegister8<T>;

    // =========================================================================
    // Compile-time register ID checks (concepts)
    // =========================================================================

    // Concept: Check if register is RBP or R13 (used for SIB encoding special cases)
    template<typename Reg>
    concept IsRBPOrR13 = Register<Reg> &&
                         requires {
                             { Reg::id_value } -> std::convertible_to<e_register_id>;
                         } &&
                         (Reg::id_value == e_register_id::bp || Reg::id_value == e_register_id::r13);

    // Concept: Check if register is RSP or R12 (need SIB byte)
    template<typename Reg>
    concept IsRSPOrR12 = Register<Reg> &&
                         requires {
                             { Reg::id_value } -> std::convertible_to<e_register_id>;
                         } &&
                         (Reg::id_value == e_register_id::sp || Reg::id_value == e_register_id::r12);

    namespace registers {

        inline constexpr rax_t rax{};
        inline constexpr eax_t eax{};
        inline constexpr ax_t ax{};
        inline constexpr al_t al{};
        inline constexpr ah_t ah{};

        inline constexpr rbx_t rbx{};
        inline constexpr ebx_t ebx{};
        inline constexpr bx_t bx{};
        inline constexpr bl_t bl{};
        inline constexpr bh_t bh{};

        inline constexpr rcx_t rcx{};
        inline constexpr ecx_t ecx{};
        inline constexpr cx_t cx{};
        inline constexpr cl_t cl{};
        inline constexpr ch_t ch{};

        inline constexpr rdx_t rdx{};
        inline constexpr edx_t edx{};
        inline constexpr dx_t dx{};
        inline constexpr dl_t dl{};
        inline constexpr dh_t dh{};

        inline constexpr rsi_t rsi{};
        inline constexpr esi_t esi{};
        inline constexpr si_t si{};
        inline constexpr sil_t sil{};

        inline constexpr rdi_t rdi{};
        inline constexpr edi_t edi{};
        inline constexpr di_t di{};
        inline constexpr dil_t dil{};

        inline constexpr rbp_t rbp{};
        inline constexpr ebp_t ebp{};
        inline constexpr bp_t bp{};
        inline constexpr bpl_t bpl{};

        inline constexpr rsp_t rsp{};
        inline constexpr esp_t esp{};
        inline constexpr sp_t sp{};
        inline constexpr spl_t spl{};

        inline constexpr r8_t r8{};
        inline constexpr r8d_t r8d{};
        inline constexpr r8w_t r8w{};
        inline constexpr r8b_t r8b{};

        inline constexpr r9_t r9{};
        inline constexpr r9d_t r9d{};
        inline constexpr r9w_t r9w{};
        inline constexpr r9b_t r9b{};

        inline constexpr r10_t r10{};
        inline constexpr r10d_t r10d{};
        inline constexpr r10w_t r10w{};
        inline constexpr r10b_t r10b{};

        inline constexpr r11_t r11{};
        inline constexpr r11d_t r11d{};
        inline constexpr r11w_t r11w{};
        inline constexpr r11b_t r11b{};

        inline constexpr r12_t r12{};
        inline constexpr r12d_t r12d{};
        inline constexpr r12w_t r12w{};
        inline constexpr r12b_t r12b{};

        inline constexpr r13_t r13{};
        inline constexpr r13d_t r13d{};
        inline constexpr r13w_t r13w{};
        inline constexpr r13b_t r13b{};

        inline constexpr r14_t r14{};
        inline constexpr r14d_t r14d{};
        inline constexpr r14w_t r14w{};
        inline constexpr r14b_t r14b{};

        inline constexpr r15_t r15{};
        inline constexpr r15d_t r15d{};
        inline constexpr r15w_t r15w{};
        inline constexpr r15b_t r15b{};

        inline constexpr rip_t rip{};
        inline constexpr eip_t eip{};
    } // namespace registers

    // Use standard library concepts for integer types
    // These provide proper type trait integration and are more maintainable
    template<typename T>
    concept SignedInteger = std::signed_integral<T>;

    template<typename T>
    concept UnsignedInteger = std::unsigned_integral<T>;

    template<typename T>
    concept Integer = std::integral<T> && !std::same_as<T, bool>;

    template<typename T>
        requires Integer<T>
    class immediate_operand : base_operand {
    public:
        constexpr immediate_operand() = delete;
        constexpr immediate_operand(T value)
            : base_operand(e_operand_type::imm),
              _value(value) {}

        constexpr ~immediate_operand() = default;

        constexpr T value() const {
            return _value;
        }

        static constexpr std::size_t size = sizeof(T) * 8;
        using value_type = T;

    private:
        T _value;
    };

    template<typename T>
    using immediate = immediate_operand<T>;

    using imm8 = immediate_operand<std::uint8_t>;
    using imm16 = immediate_operand<std::uint16_t>;
    using imm32 = immediate_operand<std::uint32_t>;
    using imm64 = immediate_operand<std::uint64_t>;

    template<typename T, typename U>
    concept _Immediate = std::same_as<immediate_operand<U>, T>;

    template<typename T>
    concept Immediate = _Immediate<T, typename T::value_type>;

    template<typename T>
    concept Immediate8 = _Immediate<T, std::uint8_t>;

    template<typename T>
    concept Immediate64 = _Immediate<T, std::uint64_t>;

    // =========================================================================
    // truncate_as - Type trait for truncating immediates to register size
    // =========================================================================
    template<typename T>
    struct truncate_as {};

    // Specializations for all register types
    template<e_register_id ID>
    struct truncate_as<reg<ID, 8, false>> {
        using type = std::uint8_t;
    };

    template<e_register_id ID>
    struct truncate_as<reg<ID, 16, false>> {
        using type = std::uint16_t;
    };

    template<e_register_id ID>
    struct truncate_as<reg<ID, 32, false>> {
        using type = std::uint32_t;
    };

    template<e_register_id ID>
    struct truncate_as<reg<ID, 64, false>> {
        using type = std::uint32_t;
    };

    template<e_register_id ID>
    struct truncate_as<reg<ID, 8, true>> {
        using type = std::uint8_t;
    };

    template<e_register_id ID>
    struct truncate_as<reg<ID, 16, true>> {
        using type = std::uint16_t;
    };

    template<e_register_id ID>
    struct truncate_as<reg<ID, 32, true>> {
        using type = std::uint32_t;
    };

    template<e_register_id ID>
    struct truncate_as<reg<ID, 64, true>> {
        using type = std::uint32_t;
    };

    template<e_register8bit_id ID>
    struct truncate_as<register8bit_operand<ID>> {
        using type = std::uint8_t;
    };

    enum class e_addressing_type {
        direct,
        indirect
    };

    enum class e_displacement_type {
        disp0,
        disp8,
        disp32
    };

    enum class e_mode {
        reg,
        sib,
        disp32_only
    };

    template<typename T, std::size_t Size = T::size>
        requires Immediate<T> || Register<T>
    class memory_operand : base_operand {
    public:
        constexpr memory_operand() = delete;

        constexpr memory_operand(e_addressing_type addressing_type, e_displacement_type displacement_type, e_mode mode, T value)
            : base_operand(e_operand_type::mem),
              _addressing_type(addressing_type),
              _displacement_type(displacement_type),
              _mode(mode),
              _value(value) {}

        constexpr ~memory_operand() = default;

        constexpr e_addressing_type addressing_type() const {
            return _addressing_type;
        }
        constexpr e_displacement_type displacement_type() const {
            return _displacement_type;
        }
        constexpr e_mode mode() const {
            return _mode;
        }
        constexpr bool has_sib() const {
            return _mode == e_mode::sib;
        }

        constexpr T value() const {
            return _value;
        }

        static constexpr bool is_reg = Register<T>;
        static constexpr bool is_imm = Immediate<T>;
        using value_type = T;

        static constexpr std::size_t size = Size;

    private:
        e_addressing_type _addressing_type;
        e_displacement_type _displacement_type;
        e_mode _mode;
        T _value;
    };

    template<typename T, std::size_t Size = T::size>
    using mem = memory_operand<T, Size>;

    template<typename T, typename U, std::size_t Size = T::size>
    concept _Memory = std::same_as<memory_operand<U, Size>, T>;

    template<typename T>
    concept Memory = _Memory<T, typename T::value_type>;

    template<typename T>
        requires Immediate<T>
    constexpr mem<T> ptr(T t) {
        return mem<T>(
            e_addressing_type::indirect,
            e_displacement_type::disp0,
            e_mode::disp32_only,
            t);
    }

    template<typename T, std::size_t Size = T::size>
        requires Register<T>
    constexpr mem<T, Size> ptr(T t) {
        return mem<T, Size>(
            e_addressing_type::indirect,
            e_displacement_type::disp0,
            e_mode::reg,
            t);
    }

    template<typename T>
        requires Register<T> || Immediate<T>
    constexpr mem<T, 8> byte_ptr(T t) {
        if constexpr (Register<T>) {
            return ptr<T, 8>(t);
        } else {
            return ptr(imm8(t));
        }
    }

    template<typename T>
        requires Integer<T>
    constexpr mem<imm32> byte_ptr(T t) {
        return ptr(imm32(t));
    }

    template<typename T>
        requires Register<T> || Immediate<T>
    constexpr mem<T, 16> word_ptr(T t) {
        if constexpr (Register<T>) {
            return ptr<T, 16>(t);
        } else {
            return ptr(imm16(t));
        }
    }

    template<typename T>
        requires Integer<T>
    constexpr mem<imm32> word_ptr(T t) {
        return ptr(imm32(t));
    }

    template<typename T>
        requires Register<T> || Immediate<T>
    constexpr mem<T, 32> dword_ptr(T t) {
        if constexpr (Register<T>) {
            return ptr<T, 32>(t);
        } else {
            return ptr(imm32(t));
        }
    }

    template<typename T>
        requires Integer<T>
    constexpr mem<imm32> dword_ptr(T t) {
        return ptr(imm32(t));
    }

    template<typename T>
        requires Register<T> || Immediate<T>
    constexpr mem<T, 64> qword_ptr(T t) {
        if constexpr (Register<T>) {
            return ptr<T, 64>(t);
        } else {
            return ptr(imm64(t));
        }
    }

    template<typename T>
        requires Integer<T>
    constexpr mem<imm32> qword_ptr(T t) {
        return ptr(imm32(t));
    }

    template<typename T>
    struct truncate_as<mem<T, 8>> {
        using type = std::uint8_t;
    };

    template<typename T>
    struct truncate_as<mem<T, 16>> {
        using type = std::uint16_t;
    };

    template<typename T>
    struct truncate_as<mem<T, 32>> {
        using type = std::uint32_t;
    };

    template<typename T>
    struct truncate_as<mem<T, 64>> {
        using type = std::uint32_t;
    };

    // =========================================================================
    // SIB (Scale-Index-Base) Addressing Support
    // =========================================================================

    // Represents a register multiplied by a scale factor: rbx * 4
    template<typename Reg, int Scale>
        requires Register<Reg> && ValidScale<Scale>
    struct scaled_reg {
        Reg reg;
        static constexpr int scale = Scale;

        constexpr scaled_reg(Reg r)
            : reg(r) {}
    };

    // Concept for scaled registers - checks for scaled_reg template structure
    template<typename T>
    concept ScaledRegister = requires(T t) {
        { t.reg } -> Register; // Must have a register member that satisfies Register
        { T::scale } -> std::convertible_to<int>; // Must have a scale constant
    } && ValidScale<T::scale>; // Valid x86 scales only

    // Sentinel types for "no base" and "no index"
    struct no_base_t {
        static constexpr bool extended = false;
        static constexpr e_register_id id() {
            return e_register_id::unknown;
        }
        static constexpr e_register_id id_value = e_register_id::unknown;
    };
    struct no_index_t {
        static constexpr bool extended = false;
        static constexpr e_register_id id() {
            return e_register_id::unknown;
        }
        static constexpr e_register_id id_value = e_register_id::unknown;
    };
    inline constexpr no_base_t no_base{};
    inline constexpr no_index_t no_index{};

    // Represents a full SIB address: base + index*scale + displacement
    template<typename Base, typename Index, int Scale, e_displacement_type DispType = e_displacement_type::disp0>
        requires(Register<Base> || std::same_as<Base, no_base_t>) && (Register<Index> || std::same_as<Index, no_index_t>) && ValidScale<Scale>
    struct address_expr {
        Base base;
        Index index;
        std::int32_t displacement = 0;

        static constexpr int scale = Scale;
        static constexpr e_displacement_type disp_type = DispType;
        static constexpr bool has_base = !std::same_as<Base, no_base_t>;
        static constexpr bool has_index = !std::same_as<Index, no_index_t>;

        constexpr address_expr(Base b, Index i, std::int32_t disp = 0)
            : base(b), index(i), displacement(disp) {}

        // Check if SIB byte is needed
        static constexpr bool needs_sib() {
            if constexpr (has_index) {
                return true; // Always need SIB with an index register
            } else if constexpr (has_base) {
                // RSP/R12 (id=4) as base always needs SIB
                return false; // No index, base is not RSP - will be checked at runtime
            }
            return false;
        }
    };

    // Concept for address expressions - used in SIB addressing
    template<typename T>
    concept AddressExpression = requires {
        { T::scale } -> std::convertible_to<int>;
        { T::disp_type } -> std::convertible_to<e_displacement_type>;
        { T::needs_sib() } -> std::convertible_to<bool>;
        { T::has_base } -> std::convertible_to<bool>;
        { T::has_index } -> std::convertible_to<bool>;
    } && ValidScale<T::scale>;

    // Displacement type concepts (for operator+ overloads)
    // Displacement8: fits in signed 8-bit (-128 to 127)
    template<typename T>
    concept Displacement8 = Integer<T> && (sizeof(T) == 1);

    // Displacement32: requires 32-bit displacement
    template<typename T>
    concept Displacement32 = Integer<T> && (sizeof(T) > 1 || std::same_as<T, int>);

    // =========================================================================
    // Operator Overloads for Building Address Expressions
    // =========================================================================

    // rbx * 4 -> scaled_reg<Reg, 4>
    template<typename Reg, int Scale>
        requires Register<Reg> && ValidScale<Scale>
    constexpr auto make_scaled_reg(Reg r) {
        return scaled_reg<Reg, Scale>{ r };
    }

    // Scale constants for use with operator*
    // Usage: rcx * s4 creates scaled_reg<rcx_type, 4>
    template<int N>
        requires ValidScale<N>
    struct scale_t {
        static constexpr int value = N;
    };

    // Scale constant instances for convenient syntax
    inline constexpr scale_t<1> s1{};
    inline constexpr scale_t<2> s2{};
    inline constexpr scale_t<4> s4{};
    inline constexpr scale_t<8> s8{};

    // Define operator* overloads for scale_t
    template<typename Reg, int N>
        requires Register<Reg> && ValidScale<N>
    constexpr scaled_reg<Reg, N> operator*(Reg r, scale_t<N>) {
        return scaled_reg<Reg, N>{ r };
    }

    template<typename Reg, int N>
        requires Register<Reg> && ValidScale<N>
    constexpr scaled_reg<Reg, N> operator*(scale_t<N>, Reg r) {
        return scaled_reg<Reg, N>{ r };
    }

    // rax + rbx*4 -> address_expr<rax, rbx, 4, disp0>
    template<typename Base, typename Index, int Scale>
        requires Register<Base> && Register<Index>
    constexpr auto operator+(Base b, scaled_reg<Index, Scale> sr) {
        return address_expr<Base, Index, Scale, e_displacement_type::disp0>{ b, sr.reg, 0 };
    }

    // rax + rbx -> address_expr<rax, rbx, 1, disp0>
    template<typename Base, typename Index>
        requires Register<Base> && Register<Index>
    constexpr auto operator+(Base b, Index i) {
        return address_expr<Base, Index, 1, e_displacement_type::disp0>{ b, i, 0 };
    }

    // (rax + rbx*4) + disp -> address_expr with appropriate displacement type
    // Uses disp8 for small displacements (-128 to 127), disp32 otherwise
    template<typename Base, typename Index, int Scale, e_displacement_type DT, std::integral Disp>
        requires(std::same_as<Base, no_base_t> || Register<Base>) && (std::same_as<Index, no_index_t> || Register<Index>)
    constexpr auto operator+(address_expr<Base, Index, Scale, DT> addr, Disp disp) {
        auto disp32 = static_cast<std::int32_t>(disp);
        // At compile time, determine displacement type based on value
        if constexpr (sizeof(Disp) == 1) {
            // int8_t/uint8_t always fits in disp8
            return address_expr<Base, Index, Scale, e_displacement_type::disp8>{
                addr.base, addr.index, disp32
            };
        } else {
            // For larger types, always use disp32 to maintain consistent return type
            return address_expr<Base, Index, Scale, e_displacement_type::disp32>{
                addr.base, addr.index, disp32
            };
        }
    }

    // =========================================================================
    // SIB Memory Operand Type
    // =========================================================================

    template<typename Base, typename Index, int Scale, std::size_t Size, e_displacement_type DispType>
        requires(Register<Base> || std::same_as<Base, no_base_t>) && (Register<Index> || std::same_as<Index, no_index_t>) && ValidScale<Scale> && ValidOperandSize<Size>
    class sib_memory_operand : base_operand {
    public:
        using base_type = Base;
        using index_type = Index;
        static constexpr int scale = Scale;
        static constexpr std::size_t size = Size;
        static constexpr e_displacement_type disp_type = DispType;
        static constexpr bool has_sib = true;
        static constexpr bool has_base = !std::same_as<Base, no_base_t>;
        static constexpr bool has_index = !std::same_as<Index, no_index_t>;

        constexpr sib_memory_operand(address_expr<Base, Index, Scale, DispType> addr)
            : base_operand(e_operand_type::mem),
              _base(addr.base),
              _index(addr.index),
              _displacement(addr.displacement) {}

        constexpr Base base() const {
            return _base;
        }
        constexpr Index index() const {
            return _index;
        }
        constexpr std::int32_t displacement() const {
            return _displacement;
        }

        // For compatibility with existing Memory concept
        using value_type = Base; // The base register type for REX encoding

    private:
        Base _base;
        Index _index;
        std::int32_t _displacement;
    };

    // Concept for SIB memory operands - validates scale and structure
    template<typename T>
    concept SIBMemory = requires {
        { T::has_sib } -> std::convertible_to<bool>;
        { T::scale } -> std::convertible_to<int>;
        { T::size } -> std::convertible_to<std::size_t>;
        typename T::base_type;
        typename T::index_type;
    } && T::has_sib && ValidScale<T::scale> && ValidOperandSize<T::size>;

    // =========================================================================
    // Memory Operand Creation for Address Expressions
    // =========================================================================

    // ptr(rax + rbx*4) or ptr(rax + rbx*4 + disp)
    template<typename Base, typename Index, int Scale, e_displacement_type DT>
    constexpr auto ptr(address_expr<Base, Index, Scale, DT> addr) {
        // Determine size from base register if available, otherwise default to 64
        constexpr std::size_t Size = Register<Base> ? Base::size : 64;
        return sib_memory_operand<Base, Index, Scale, Size, DT>{ addr };
    }

    // byte_ptr for address expressions
    template<typename Base, typename Index, int Scale, e_displacement_type DT>
    constexpr auto byte_ptr(address_expr<Base, Index, Scale, DT> addr) {
        return sib_memory_operand<Base, Index, Scale, 8, DT>{ addr };
    }

    // word_ptr for address expressions
    template<typename Base, typename Index, int Scale, e_displacement_type DT>
    constexpr auto word_ptr(address_expr<Base, Index, Scale, DT> addr) {
        return sib_memory_operand<Base, Index, Scale, 16, DT>{ addr };
    }

    // dword_ptr for address expressions
    template<typename Base, typename Index, int Scale, e_displacement_type DT>
    constexpr auto dword_ptr(address_expr<Base, Index, Scale, DT> addr) {
        return sib_memory_operand<Base, Index, Scale, 32, DT>{ addr };
    }

    // qword_ptr for address expressions
    template<typename Base, typename Index, int Scale, e_displacement_type DT>
    constexpr auto qword_ptr(address_expr<Base, Index, Scale, DT> addr) {
        return sib_memory_operand<Base, Index, Scale, 64, DT>{ addr };
    }
} // namespace static_asm::x86
