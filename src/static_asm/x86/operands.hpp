#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>

namespace static_asm::x86 {

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

        constexpr base_operand(e_operand_type type) : _type(type) {};

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

    template<std::size_t Size, bool Extended>
    class register_operand : base_operand {
    public:
        constexpr register_operand() = delete;
        constexpr register_operand(e_register_id id) : base_operand(e_operand_type::reg),
                                                       _id(id) {}

        constexpr ~register_operand() = default;

        constexpr e_register_id id() const {
            return _id;
        }

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

        // constexpr bool is_extended_register() const { return Extended; }
        // constexpr std::size_t register_size() const { return Size; }

        static constexpr bool extended = Extended;
        static constexpr std::size_t size = Size;

        constexpr bool is_ip() const {
            return _id == e_register_id::ip;
        }

        using value_type = register_operand<Size, Extended>;

    private:
        e_register_id _id;
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

    class register8bit_operand : register_operand<8, false> {
    public:
        constexpr register8bit_operand() = delete;
        constexpr register8bit_operand(e_register8bit_id id) : register_operand(e_register_id::unknown),
                                                               _id(id) {}

        constexpr ~register8bit_operand() = default;

        constexpr e_register8bit_id id() const {
            return _id;
        }

        // override extended static member
        static constexpr bool extended = false;
        // override size static member
        static constexpr std::size_t size = 8;

        using value_type = register_operand<8, false>;

    private:
        e_register8bit_id _id;
    };

    template<std::size_t Size, bool Extended>
    using reg = register_operand<Size, Extended>;

    using reg8 = reg<8, false>;
    using reg16 = reg<16, false>;
    using reg32 = reg<32, false>;
    using reg64 = reg<64, false>;

    using ereg8 = reg<8, true>;
    using ereg16 = reg<16, true>;
    using ereg32 = reg<32, true>;
    using ereg64 = reg<64, true>;

    using reg8lh = register8bit_operand;

    template<typename T, std::size_t Size, bool Extended>
    concept _Register = std::same_as<reg<Size, Extended>, T>;

    template<typename T>
    concept Register = _Register<T, T::size, T::extended> || std::same_as<register8bit_operand, T>;

    template<typename T, std::size_t Size>
    concept ExtendedRegister = _Register<T, Size, true>;

    template<typename T, std::size_t Size>
    concept LegacyRegister = _Register<T, Size, false>;

    template<typename T>
    concept LRegister64 = LegacyRegister<T, 64>;

    template<typename T>
    concept LRegister32 = LegacyRegister<T, 32>;

    template<typename T>
    concept LRegister16 = LegacyRegister<T, 16>;

    template<typename T>
    concept LRegister8 = std::same_as<register8bit_operand, T>;

    template<typename T>
    concept ERegister64 = ExtendedRegister<T, 64>;

    template<typename T>
    concept ERegister32 = ExtendedRegister<T, 32>;

    template<typename T>
    concept ERegister16 = ExtendedRegister<T, 16>;

    template<typename T>
    concept ERegister8 = ExtendedRegister<T, 8>;

    template<typename T>
    concept Register64 = ERegister64<T> || LRegister64<T>;

    template<typename T>
    concept Register32 = ERegister32<T> || LRegister32<T>;

    template<typename T>
    concept Register16 = ERegister16<T> || LRegister16<T>;

    template<typename T>
    concept Register8 = ERegister8<T> || LRegister8<T>;

    // Concept to check if a register is the CL register (used for shift/rotate by CL)
    template<typename T>
    concept IsCLRegister = LRegister8<T>;

    namespace registers {

        constexpr reg64 rax(e_register_id::ax);
        constexpr reg32 eax(e_register_id::ax);
        constexpr reg16 ax(e_register_id::ax);
        constexpr reg8lh al(e_register8bit_id::al);
        constexpr reg8lh ah(e_register8bit_id::ah);

        constexpr reg64 rbx(e_register_id::bx);
        constexpr reg32 ebx(e_register_id::bx);
        constexpr reg16 bx(e_register_id::bx);
        constexpr reg8lh bl(e_register8bit_id::bl);
        constexpr reg8lh bh(e_register8bit_id::bh);

        constexpr reg64 rcx(e_register_id::cx);
        constexpr reg32 ecx(e_register_id::cx);
        constexpr reg16 cx(e_register_id::cx);
        constexpr reg8lh cl(e_register8bit_id::cl);
        constexpr reg8lh ch(e_register8bit_id::ch);

        constexpr reg64 rdx(e_register_id::dx);
        constexpr reg32 edx(e_register_id::dx);
        constexpr reg16 dx(e_register_id::dx);
        constexpr reg8lh dl(e_register8bit_id::dl);
        constexpr reg8lh dh(e_register8bit_id::dh);

        constexpr reg64 rsi(e_register_id::si);
        constexpr reg32 esi(e_register_id::si);
        constexpr reg16 si(e_register_id::si);
        constexpr ereg8 sil(e_register_id::si);

        constexpr reg64 rdi(e_register_id::di);
        constexpr reg32 edi(e_register_id::di);
        constexpr reg16 di(e_register_id::di);
        constexpr ereg8 dil(e_register_id::di);

        constexpr reg64 rbp(e_register_id::bp);
        constexpr reg32 ebp(e_register_id::bp);
        constexpr reg16 bp(e_register_id::bp);
        constexpr ereg8 bpl(e_register_id::bp);

        constexpr reg64 rsp(e_register_id::sp);
        constexpr reg32 esp(e_register_id::sp);
        constexpr reg16 sp(e_register_id::sp);
        constexpr ereg8 spl(e_register_id::sp);

        constexpr ereg64 r8(e_register_id::r8);
        constexpr ereg32 r8d(e_register_id::r8);
        constexpr ereg16 r8w(e_register_id::r8);
        constexpr ereg8 r8b(e_register_id::r8);

        constexpr ereg64 r9(e_register_id::r9);
        constexpr ereg32 r9d(e_register_id::r9);
        constexpr ereg16 r9w(e_register_id::r9);
        constexpr ereg8 r9b(e_register_id::r9);

        constexpr ereg64 r10(e_register_id::r10);
        constexpr ereg32 r10d(e_register_id::r10);
        constexpr ereg16 r10w(e_register_id::r10);
        constexpr ereg8 r10b(e_register_id::r10);

        constexpr ereg64 r11(e_register_id::r11);
        constexpr ereg32 r11d(e_register_id::r11);
        constexpr ereg16 r11w(e_register_id::r11);
        constexpr ereg8 r11b(e_register_id::r11);

        constexpr ereg64 r12(e_register_id::r12);
        constexpr ereg32 r12d(e_register_id::r12);
        constexpr ereg16 r12w(e_register_id::r12);
        constexpr ereg8 r12b(e_register_id::r12);

        constexpr ereg64 r13(e_register_id::r13);
        constexpr ereg32 r13d(e_register_id::r13);
        constexpr ereg16 r13w(e_register_id::r13);
        constexpr ereg8 r13b(e_register_id::r13);

        constexpr ereg64 r14(e_register_id::r14);
        constexpr ereg32 r14d(e_register_id::r14);
        constexpr ereg16 r14w(e_register_id::r14);
        constexpr ereg8 r14b(e_register_id::r14);

        constexpr ereg64 r15(e_register_id::r15);
        constexpr ereg32 r15d(e_register_id::r15);
        constexpr ereg16 r15w(e_register_id::r15);
        constexpr ereg8 r15b(e_register_id::r15);

        constexpr reg64 rip(e_register_id::ip);
        constexpr reg32 eip(e_register_id::ip);
    } // namespace registers

    template<typename T>
    concept SignedInteger = std::same_as<T, std::int8_t> || std::same_as<T, std::int16_t> || std::same_as<T, std::int32_t> || std::same_as<T, std::int64_t> || std::same_as<T, signed char> || std::same_as<T, short> || std::same_as<T, int> || std::same_as<T, long> || std::same_as<T, long long>;

    template<typename T>
    concept UnsignedInteger = std::same_as<T, std::uint8_t> || std::same_as<T, std::uint16_t> || std::same_as<T, std::uint32_t> || std::same_as<T, std::uint64_t> || std::same_as<T, unsigned char> || std::same_as<T, unsigned short> || std::same_as<T, unsigned int> || std::same_as<T, unsigned long> || std::same_as<T, unsigned long long>;

    template<typename T>
    concept Integer = UnsignedInteger<T> || SignedInteger<T>;

    template<typename T>
        requires Integer<T>
    class immediate_operand : base_operand {
    public:
        constexpr immediate_operand() = delete;
        constexpr immediate_operand(T value) : base_operand(e_operand_type::imm),
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

    template<typename T>
    struct truncate_as {};

    template<>
    struct truncate_as<reg8> {
        using type = std::uint8_t;
    };

    template<>
    struct truncate_as<reg16> {
        using type = std::uint16_t;
    };

    template<>
    struct truncate_as<reg32> {
        using type = std::uint32_t;
    };

    template<>
    struct truncate_as<reg64> {
        using type = std::uint32_t;
    };

    template<>
    struct truncate_as<ereg8> {
        using type = std::uint8_t;
    };

    template<>
    struct truncate_as<ereg16> {
        using type = std::uint16_t;
    };

    template<>
    struct truncate_as<ereg32> {
        using type = std::uint32_t;
    };

    template<>
    struct truncate_as<ereg64> {
        using type = std::uint32_t;
    };

    template<>
    struct truncate_as<reg8lh> {
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

        constexpr memory_operand(e_addressing_type addressing_type, e_displacement_type displacement_type, e_mode mode, T value) : base_operand(e_operand_type::mem),
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

    // template <typename M>
    // struct truncate_as<memory_operand<M>> {
    //     using type = typename truncate_as<typename memory_operand<M>::value_type>::type;
    // };

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
        requires Register<Reg> && (Scale == 1 || Scale == 2 || Scale == 4 || Scale == 8)
    struct scaled_reg {
        Reg reg;
        static constexpr int scale = Scale;

        constexpr scaled_reg(Reg r) : reg(r) {}
    };

    // Concept for scaled registers
    template<typename T>
    concept ScaledRegister = requires {
        typename std::remove_cvref_t<T>;
    } && requires(T t) {
        { t.reg };
        { T::scale } -> std::convertible_to<int>;
    };

    // Sentinel types for "no base" and "no index"
    struct no_base_t {
        static constexpr bool extended = false;
        static constexpr e_register_id id() {
            return e_register_id::unknown;
        }
    };
    struct no_index_t {
        static constexpr bool extended = false;
        static constexpr e_register_id id() {
            return e_register_id::unknown;
        }
    };
    inline constexpr no_base_t no_base{};
    inline constexpr no_index_t no_index{};

    // Represents a full SIB address: base + index*scale + displacement
    template<typename Base, typename Index, int Scale, e_displacement_type DispType = e_displacement_type::disp0>
        requires(Register<Base> || std::same_as<Base, no_base_t>) && (Register<Index> || std::same_as<Index, no_index_t>) && (Scale == 1 || Scale == 2 || Scale == 4 || Scale == 8)
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

    // Concept for address expressions
    template<typename T>
    concept AddressExpression = requires {
        { T::scale } -> std::convertible_to<int>;
        { T::disp_type } -> std::convertible_to<e_displacement_type>;
        { T::needs_sib() } -> std::convertible_to<bool>;
    };

    // Displacement type concepts (for operator+ overloads)
    template<typename T>
    concept Displacement8 = Integer<T> && (sizeof(T) == 1);

    template<typename T>
    concept Displacement32 = Integer<T> && (sizeof(T) > 1 || std::same_as<T, int>);

    // =========================================================================
    // Operator Overloads for Building Address Expressions
    // =========================================================================

    // rbx * 4 -> scaled_reg<Reg, 4>
    template<typename Reg, int Scale>
        requires Register<Reg> && (Scale == 1 || Scale == 2 || Scale == 4 || Scale == 8)
    constexpr auto make_scaled_reg(Reg r) {
        return scaled_reg<Reg, Scale>{ r };
    }

    // Scale constants for use with operator*
    // Usage: rcx * s4 creates scaled_reg<rcx_type, 4>
    template<int N>
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
        requires Register<Reg> && (N == 1 || N == 2 || N == 4 || N == 8)
    constexpr scaled_reg<Reg, N> operator*(Reg r, scale_t<N>) {
        return scaled_reg<Reg, N>{ r };
    }

    template<typename Reg, int N>
        requires Register<Reg> && (N == 1 || N == 2 || N == 4 || N == 8)
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
        requires(Register<Base> || std::same_as<Base, no_base_t>) && (Register<Index> || std::same_as<Index, no_index_t>) && (Scale == 1 || Scale == 2 || Scale == 4 || Scale == 8)
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
              _base(addr.base), _index(addr.index), _displacement(addr.displacement) {}

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

    // Concept for SIB memory operands
    template<typename T>
    concept SIBMemory = requires {
        { T::has_sib } -> std::convertible_to<bool>;
        { T::scale } -> std::convertible_to<int>;
        typename T::base_type;
        typename T::index_type;
    } && T::has_sib;

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