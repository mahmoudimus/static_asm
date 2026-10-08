// static_asm - typed labels and symbolic fragments for core::assemble
// SPDX-License-Identifier: BSL-1.0 OR MIT
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <tuple>
#include <type_traits>
#include <utility>

#include "../core.hpp"
#include "gen/instruction.g.hpp"
#include "operands.hpp"
#include "relaxation.hpp"

namespace static_asm::x86 {

    template<std::size_t N>
    struct fixed_string {
        char chars[N]{};

        constexpr fixed_string(const char (&text)[N]) {
            for (std::size_t i = 0; i < N; ++i) {
                chars[i] = text[i];
            }
        }
    };

    template<std::size_t N>
    fixed_string(const char (&)[N]) -> fixed_string<N>;

    template<typename Label, typename... Parts>
    struct labeled_fragment {
        std::tuple<Parts...> parts;
    };

    template<fixed_string Name>
    struct label_t {
        template<typename... Parts>
        constexpr auto assemble(const Parts&... parts) const {
            return labeled_fragment<label_t, Parts...>{ std::tuple<Parts...>{ parts... } };
        }
    };

    template<fixed_string Name>
    inline constexpr label_t<Name> label{};

    template<typename Label, std::size_t Width>
    struct label_memory {};

    template<fixed_string Name>
    constexpr auto byte_ptr(label_t<Name>) {
        return label_memory<label_t<Name>, 8>{};
    }

    template<fixed_string Name>
    constexpr auto word_ptr(label_t<Name>) {
        return label_memory<label_t<Name>, 16>{};
    }

    template<fixed_string Name>
    constexpr auto dword_ptr(label_t<Name>) {
        return label_memory<label_t<Name>, 32>{};
    }

    template<fixed_string Name>
    constexpr auto qword_ptr(label_t<Name>) {
        return label_memory<label_t<Name>, 64>{};
    }

    constexpr auto db(std::uint8_t value) {
        return std::array<std::uint8_t, 1>{ value };
    }

    constexpr auto dw(std::uint16_t value) {
        return std::array<std::uint8_t, 2>{ static_cast<std::uint8_t>(value), static_cast<std::uint8_t>(value >> 8) };
    }

    constexpr auto dd(std::uint32_t value) {
        return std::array<std::uint8_t, 4>{ static_cast<std::uint8_t>(value), static_cast<std::uint8_t>(value >> 8),
            static_cast<std::uint8_t>(value >> 16), static_cast<std::uint8_t>(value >> 24) };
    }

    constexpr auto dq(std::uint64_t value) {
        return std::array<std::uint8_t, 8>{ static_cast<std::uint8_t>(value), static_cast<std::uint8_t>(value >> 8),
            static_cast<std::uint8_t>(value >> 16), static_cast<std::uint8_t>(value >> 24),
            static_cast<std::uint8_t>(value >> 32), static_cast<std::uint8_t>(value >> 40),
            static_cast<std::uint8_t>(value >> 48), static_cast<std::uint8_t>(value >> 56) };
    }

    template<typename Label, std::uint8_t ShortOpcode, bool NearPrefix, std::uint8_t NearOpcode>
    struct branch_ref {
        using label_type = Label;

        template<typename Block>
        static constexpr void append(Block& block, typename Block::label_id target) {
            block.put_branch(ShortOpcode, NearPrefix, NearOpcode, target);
        }
    };

    template<typename Label, std::size_t N>
    struct rip_ref {
        using label_type = Label;
        static constexpr std::size_t byte_count = N;
        std::array<std::uint8_t, N> bytes;
        std::size_t displacement_offset;
    };

    namespace detail {

        template<typename Label>
        struct label_marker {
            using label_type = Label;
        };

        template<typename T>
        inline constexpr bool is_symbolic_v = false;

        template<typename Label, typename... Parts>
        inline constexpr bool is_symbolic_v<labeled_fragment<Label, Parts...>> = true;

        template<typename Label, std::uint8_t ShortOpcode, bool NearPrefix, std::uint8_t NearOpcode>
        inline constexpr bool is_symbolic_v<branch_ref<Label, ShortOpcode, NearPrefix, NearOpcode>> = true;

        template<typename Label, std::size_t N>
        inline constexpr bool is_symbolic_v<rip_ref<Label, N>> = true;

        template<typename T>
        inline constexpr bool is_marker_v = false;

        template<typename Label>
        inline constexpr bool is_marker_v<label_marker<Label>> = true;

        template<typename T>
        inline constexpr bool is_branch_v = false;

        template<typename Label, std::uint8_t ShortOpcode, bool NearPrefix, std::uint8_t NearOpcode>
        inline constexpr bool is_branch_v<branch_ref<Label, ShortOpcode, NearPrefix, NearOpcode>> = true;

        template<typename T>
        inline constexpr bool is_rip_ref_v = false;

        template<typename Label, std::size_t N>
        inline constexpr bool is_rip_ref_v<rip_ref<Label, N>> = true;

        template<std::size_t Width>
        constexpr auto rip_placeholder(std::int32_t displacement) {
            if constexpr (Width == 8) {
                return byte_ptr(registers::rip + displacement);
            } else if constexpr (Width == 16) {
                return word_ptr(registers::rip + displacement);
            } else if constexpr (Width == 32) {
                return dword_ptr(registers::rip + displacement);
            } else {
                static_assert(Width == 64);
                return qword_ptr(registers::rip + displacement);
            }
        }

        template<typename Label, std::size_t Width, typename Encoder>
        constexpr auto make_rip_ref(Encoder encoder) {
            constexpr std::int32_t marker = 0x13579BDF;
            const auto bytes = encoder(rip_placeholder<Width>(0));
            const auto marked = encoder(rip_placeholder<Width>(marker));
            constexpr std::size_t n = std::tuple_size_v<decltype(bytes)>;
            constexpr std::array<std::uint8_t, 4> pattern{ 0xDF, 0x9B, 0x57, 0x13 };
            for (std::size_t i = 0; i + 4 <= n; ++i) {
                bool match = true;
                for (std::size_t j = 0; j < n; ++j) {
                    if (j >= i && j < i + 4) {
                        match &= bytes[j] == 0 && marked[j] == pattern[j - i];
                    } else {
                        match &= bytes[j] == marked[j];
                    }
                }
                if (match) {
                    return rip_ref<Label, n>{ bytes, i };
                }
            }
            std::abort(); // The encoder did not produce a four-byte RIP displacement.
        }

        template<typename T, typename Label>
        inline constexpr bool is_marker_for_v = false;

        template<typename DefinedLabel, typename Label>
        inline constexpr bool is_marker_for_v<label_marker<DefinedLabel>, Label> = std::is_same_v<DefinedLabel, Label>;

        template<typename T>
        constexpr auto flatten(const T& value) {
            return std::tuple<T>{ value };
        }

        template<typename Label, typename... Parts>
        constexpr auto flatten(const labeled_fragment<Label, Parts...>& fragment) {
            return std::tuple_cat(std::tuple<label_marker<Label>>{},
                std::apply([](const auto&... parts) {
                    return std::tuple_cat(flatten(parts)...);
                },
                    fragment.parts));
        }

        template<typename Label, typename Tuple>
        consteval std::size_t label_id() {
            return []<std::size_t... Is>(std::index_sequence<Is...>) consteval {
                constexpr std::array<bool, sizeof...(Is)> markers{ is_marker_v<std::tuple_element_t<Is, Tuple>>... };
                constexpr std::array<bool, sizeof...(Is)> matches{ is_marker_for_v<std::tuple_element_t<Is, Tuple>, Label>... };
                constexpr std::size_t count = (std::size_t{ 0 } + ... + static_cast<std::size_t>(is_marker_for_v<std::tuple_element_t<Is, Tuple>, Label>));
                static_assert(count == 1, "every referenced label must have exactly one definition");
                std::size_t rank = 0;
                for (std::size_t i = 0; i < sizeof...(Is); ++i) {
                    if (matches[i]) {
                        return rank;
                    }
                    rank += markers[i];
                }
                return std::size_t{ 0 };
            }(std::make_index_sequence<std::tuple_size_v<Tuple>>{});
        }

        template<typename Tuple, std::size_t... Is>
        consteval std::size_t byte_pool_size(std::index_sequence<Is...>) {
            return (std::size_t{ 0 } + ... + ([]<typename T>() consteval {
                if constexpr (core::FixedByteArray<T>) {
                    return std::tuple_size_v<T>;
                } else if constexpr (is_rip_ref_v<T>) {
                    return T::byte_count;
                } else {
                    return std::size_t{ 0 };
                }
            }.template operator()<std::tuple_element_t<Is, Tuple>>()));
        }

        template<typename Tuple, std::size_t... Is>
        consteval std::size_t label_count(std::index_sequence<Is...>) {
            return (std::size_t{ 0 } + ... + static_cast<std::size_t>(is_marker_v<std::tuple_element_t<Is, Tuple>>));
        }

        template<typename Tuple>
        using block_for = relaxation_engine<
            byte_pool_size<Tuple>(std::make_index_sequence<std::tuple_size_v<Tuple>>{}) + 1,
            std::tuple_size_v<Tuple> + 1,
            label_count<Tuple>(std::make_index_sequence<std::tuple_size_v<Tuple>>{}) + 1>;

        template<typename Tuple, typename Block, typename T>
        constexpr void append_actual(Block& block, const T& value) {
            if constexpr (is_marker_v<T>) {
                block.bind(label_id<typename T::label_type, Tuple>());
            } else if constexpr (is_branch_v<T>) {
                T::append(block, label_id<typename T::label_type, Tuple>());
            } else if constexpr (is_rip_ref_v<T>) {
                if (value.displacement_offset + 4 > value.bytes.size()) {
                    std::abort();
                }
                block.put_rip(value.bytes, label_id<typename T::label_type, Tuple>(),
                    value.bytes.size() - value.displacement_offset - 4);
            } else if constexpr (core::FixedByteArray<T>) {
                std::array<std::uint8_t, std::tuple_size_v<T>> bytes{};
                for (std::size_t i = 0; i < bytes.size(); ++i) {
                    bytes[i] = value[i];
                }
                block.put(bytes);
            } else {
                static_assert(sizeof(T) == 0, "unsupported symbolic assembly fragment");
            }
        }

        template<typename Tuple, typename Block, typename T>
        constexpr void append_dummy(Block& block) {
            if constexpr (is_marker_v<T>) {
                block.bind(label_id<typename T::label_type, Tuple>());
            } else if constexpr (is_branch_v<T>) {
                T::append(block, label_id<typename T::label_type, Tuple>());
            } else if constexpr (is_rip_ref_v<T>) {
                block.put_rip(std::array<std::uint8_t, T::byte_count>{},
                    label_id<typename T::label_type, Tuple>());
            } else if constexpr (core::FixedByteArray<T>) {
                block.put(std::array<std::uint8_t, std::tuple_size_v<T>>{});
            } else {
                static_assert(sizeof(T) == 0, "unsupported symbolic assembly fragment");
            }
        }

        template<typename Tuple>
        consteval auto dummy_block() {
            block_for<Tuple> block;
            [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                (append_dummy<Tuple, block_for<Tuple>, std::tuple_element_t<Is, Tuple>>(block), ...);
            }(std::make_index_sequence<std::tuple_size_v<Tuple>>{});
            return block;
        }

    } // namespace detail

    template<std::size_t N, typename Fragments>
    struct symbolic_program : std::array<std::uint8_t, N> {
        using base = std::array<std::uint8_t, N>;

        constexpr explicit symbolic_program(const base& bytes)
            : base(bytes) {}

        template<fixed_string Name>
        constexpr std::size_t offset_of(label_t<Name>) const {
            constexpr auto block = detail::dummy_block<Fragments>();
            return block.offset_of(detail::label_id<label_t<Name>, Fragments>());
        }

        friend constexpr bool operator==(const symbolic_program& lhs, const base& rhs) {
            return static_cast<const base&>(lhs) == rhs;
        }
    };

} // namespace static_asm::x86

namespace std {
    template<std::size_t N, typename Fragments>
    struct tuple_size<static_asm::x86::symbolic_program<N, Fragments>> : integral_constant<std::size_t, N> {};
} // namespace std

namespace static_asm::core {
    template<typename... Ts>
        requires((x86::detail::is_symbolic_v<Ts> || ...))
    constexpr auto assemble(const Ts&... inputs) {
        auto fragments = std::tuple_cat(x86::detail::flatten(inputs)...);
        using fragment_tuple = decltype(fragments);
        constexpr auto layout = x86::detail::dummy_block<fragment_tuple>();
        constexpr std::size_t n = layout.size();
        x86::detail::block_for<fragment_tuple> block;
        std::apply([&](const auto&... parts) {
            (x86::detail::append_actual<fragment_tuple>(block, parts), ...);
        },
            fragments);
        return x86::symbolic_program<n, fragment_tuple>{ block.template assemble<n>() };
    }
} // namespace static_asm::core

namespace static_asm::x86::instructions {

#define STATIC_ASM_LABEL_BRANCH(Name, Short, Has0F, Near)            \
    template<fixed_string LabelName>                                 \
    constexpr auto Name(label_t<LabelName>) {                        \
        return branch_ref<label_t<LabelName>, Short, Has0F, Near>{}; \
    }

    STATIC_ASM_LABEL_BRANCH(jmp, 0xEB, false, 0xE9)
    STATIC_ASM_LABEL_BRANCH(call, 0x00, false, 0xE8)
    STATIC_ASM_LABEL_BRANCH(jo, 0x70, true, 0x80)
    STATIC_ASM_LABEL_BRANCH(jno, 0x71, true, 0x81)
    STATIC_ASM_LABEL_BRANCH(jb, 0x72, true, 0x82)
    STATIC_ASM_LABEL_BRANCH(jc, 0x72, true, 0x82)
    STATIC_ASM_LABEL_BRANCH(jnae, 0x72, true, 0x82)
    STATIC_ASM_LABEL_BRANCH(jae, 0x73, true, 0x83)
    STATIC_ASM_LABEL_BRANCH(jnb, 0x73, true, 0x83)
    STATIC_ASM_LABEL_BRANCH(jnc, 0x73, true, 0x83)
    STATIC_ASM_LABEL_BRANCH(jz, 0x74, true, 0x84)
    STATIC_ASM_LABEL_BRANCH(jnz, 0x75, true, 0x85)
    STATIC_ASM_LABEL_BRANCH(je, 0x74, true, 0x84)
    STATIC_ASM_LABEL_BRANCH(jne, 0x75, true, 0x85)
    STATIC_ASM_LABEL_BRANCH(jbe, 0x76, true, 0x86)
    STATIC_ASM_LABEL_BRANCH(jna, 0x76, true, 0x86)
    STATIC_ASM_LABEL_BRANCH(ja, 0x77, true, 0x87)
    STATIC_ASM_LABEL_BRANCH(jnbe, 0x77, true, 0x87)
    STATIC_ASM_LABEL_BRANCH(js, 0x78, true, 0x88)
    STATIC_ASM_LABEL_BRANCH(jns, 0x79, true, 0x89)
    STATIC_ASM_LABEL_BRANCH(jp, 0x7A, true, 0x8A)
    STATIC_ASM_LABEL_BRANCH(jpe, 0x7A, true, 0x8A)
    STATIC_ASM_LABEL_BRANCH(jnp, 0x7B, true, 0x8B)
    STATIC_ASM_LABEL_BRANCH(jpo, 0x7B, true, 0x8B)
    STATIC_ASM_LABEL_BRANCH(jl, 0x7C, true, 0x8C)
    STATIC_ASM_LABEL_BRANCH(jnge, 0x7C, true, 0x8C)
    STATIC_ASM_LABEL_BRANCH(jge, 0x7D, true, 0x8D)
    STATIC_ASM_LABEL_BRANCH(jnl, 0x7D, true, 0x8D)
    STATIC_ASM_LABEL_BRANCH(jle, 0x7E, true, 0x8E)
    STATIC_ASM_LABEL_BRANCH(jng, 0x7E, true, 0x8E)
    STATIC_ASM_LABEL_BRANCH(jg, 0x7F, true, 0x8F)
    STATIC_ASM_LABEL_BRANCH(jnle, 0x7F, true, 0x8F)

#undef STATIC_ASM_LABEL_BRANCH

#define STATIC_ASM_LABEL_MEMORY_BINARY(Name)                                \
    template<typename Op, typename Label, std::size_t Width>                \
    constexpr auto Name(const Op& op, label_memory<Label, Width>) {         \
        return detail::make_rip_ref<Label, Width>([&](const auto& memory) { \
            return Name(op, memory);                                        \
        });                                                                 \
    }                                                                       \
    template<typename Label, std::size_t Width, typename Op>                \
    constexpr auto Name(label_memory<Label, Width>, const Op& op) {         \
        return detail::make_rip_ref<Label, Width>([&](const auto& memory) { \
            return Name(memory, op);                                        \
        });                                                                 \
    }

    STATIC_ASM_LABEL_MEMORY_BINARY(mov)
    STATIC_ASM_LABEL_MEMORY_BINARY(lea)
    STATIC_ASM_LABEL_MEMORY_BINARY(add)
    STATIC_ASM_LABEL_MEMORY_BINARY(adc)
    STATIC_ASM_LABEL_MEMORY_BINARY(sub)
    STATIC_ASM_LABEL_MEMORY_BINARY(sbb)
    STATIC_ASM_LABEL_MEMORY_BINARY(cmp)
    STATIC_ASM_LABEL_MEMORY_BINARY(and_)
    STATIC_ASM_LABEL_MEMORY_BINARY(or_)
    STATIC_ASM_LABEL_MEMORY_BINARY(xor_)
    STATIC_ASM_LABEL_MEMORY_BINARY(xchg)
    STATIC_ASM_LABEL_MEMORY_BINARY(test)

#undef STATIC_ASM_LABEL_MEMORY_BINARY

#define STATIC_ASM_LABEL_MEMORY_SOURCE(Name)                                \
    template<typename Op, typename Label, std::size_t Width>                \
    constexpr auto Name(const Op& op, label_memory<Label, Width>) {         \
        return detail::make_rip_ref<Label, Width>([&](const auto& memory) { \
            return Name(op, memory);                                        \
        });                                                                 \
    }

    STATIC_ASM_LABEL_MEMORY_SOURCE(bsf)
    STATIC_ASM_LABEL_MEMORY_SOURCE(bsr)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmova)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovnbe)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovae)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovnb)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovnc)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovb)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovc)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovnae)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovbe)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovna)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmove)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovz)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovg)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovnle)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovge)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovnl)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovl)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovnge)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovle)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovng)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovne)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovnz)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovno)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovnp)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovpo)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovns)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovo)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovp)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovpe)
    STATIC_ASM_LABEL_MEMORY_SOURCE(cmovs)
    STATIC_ASM_LABEL_MEMORY_SOURCE(imul)
    STATIC_ASM_LABEL_MEMORY_SOURCE(movzx)
    STATIC_ASM_LABEL_MEMORY_SOURCE(movsx)
    STATIC_ASM_LABEL_MEMORY_SOURCE(movsxd)

#undef STATIC_ASM_LABEL_MEMORY_SOURCE

#define STATIC_ASM_LABEL_MEMORY_DEST(Name)                                  \
    template<typename Label, std::size_t Width, typename Op>                \
    constexpr auto Name(label_memory<Label, Width>, const Op& op) {         \
        return detail::make_rip_ref<Label, Width>([&](const auto& memory) { \
            return Name(memory, op);                                        \
        });                                                                 \
    }

    STATIC_ASM_LABEL_MEMORY_DEST(bt)
    STATIC_ASM_LABEL_MEMORY_DEST(btc)
    STATIC_ASM_LABEL_MEMORY_DEST(btr)
    STATIC_ASM_LABEL_MEMORY_DEST(bts)
    STATIC_ASM_LABEL_MEMORY_DEST(shl)
    STATIC_ASM_LABEL_MEMORY_DEST(shr)
    STATIC_ASM_LABEL_MEMORY_DEST(sal)
    STATIC_ASM_LABEL_MEMORY_DEST(sar)
    STATIC_ASM_LABEL_MEMORY_DEST(rol)
    STATIC_ASM_LABEL_MEMORY_DEST(ror)
    STATIC_ASM_LABEL_MEMORY_DEST(rcl)
    STATIC_ASM_LABEL_MEMORY_DEST(rcr)

#undef STATIC_ASM_LABEL_MEMORY_DEST

    template<typename Reg, typename Label, std::size_t Width, typename Imm>
    constexpr auto imul(const Reg& reg, label_memory<Label, Width>, const Imm& imm) {
        return detail::make_rip_ref<Label, Width>([&](const auto& memory) {
            return imul(reg, memory, imm);
        });
    }

#define STATIC_ASM_LABEL_MEMORY_UNARY(Name)                                \
    template<typename Label, std::size_t Width>                            \
    constexpr auto Name(label_memory<Label, Width>) {                      \
        return detail::make_rip_ref<Label, Width>([](const auto& memory) { \
            return Name(memory);                                           \
        });                                                                \
    }

    STATIC_ASM_LABEL_MEMORY_UNARY(inc)
    STATIC_ASM_LABEL_MEMORY_UNARY(dec)
    STATIC_ASM_LABEL_MEMORY_UNARY(neg)
    STATIC_ASM_LABEL_MEMORY_UNARY(not_)
    STATIC_ASM_LABEL_MEMORY_UNARY(mul)
    STATIC_ASM_LABEL_MEMORY_UNARY(imul)
    STATIC_ASM_LABEL_MEMORY_UNARY(div)
    STATIC_ASM_LABEL_MEMORY_UNARY(idiv)
    STATIC_ASM_LABEL_MEMORY_UNARY(shl)
    STATIC_ASM_LABEL_MEMORY_UNARY(shr)
    STATIC_ASM_LABEL_MEMORY_UNARY(sal)
    STATIC_ASM_LABEL_MEMORY_UNARY(sar)
    STATIC_ASM_LABEL_MEMORY_UNARY(rol)
    STATIC_ASM_LABEL_MEMORY_UNARY(ror)
    STATIC_ASM_LABEL_MEMORY_UNARY(rcl)
    STATIC_ASM_LABEL_MEMORY_UNARY(rcr)
    STATIC_ASM_LABEL_MEMORY_UNARY(call)
    STATIC_ASM_LABEL_MEMORY_UNARY(jmp)
    STATIC_ASM_LABEL_MEMORY_UNARY(push)
    STATIC_ASM_LABEL_MEMORY_UNARY(pop)

#undef STATIC_ASM_LABEL_MEMORY_UNARY

} // namespace static_asm::x86::instructions
