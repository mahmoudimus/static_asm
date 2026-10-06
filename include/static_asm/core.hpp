#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

namespace static_asm::core {

    // ─────────────────────────────────────────────────────────────────────────────
    //  Concepts
    // ─────────────────────────────────────────────────────────────────────────────

    template<class T>
    concept ByteLike = std::same_as<std::remove_cvref_t<T>, std::uint8_t>;

    template<class T>
    concept FixedByteArray = requires {
        requires std::same_as<typename T::value_type, std::uint8_t>;
        { std::tuple_size<T>::value } -> std::convertible_to<std::size_t>;
        { std::declval<T>()[0] } -> ByteLike;
    };

    // ─────────────────────────────────────────────────────────────────────────────
    //  assemble – concatenate byte arrays at compile time
    // ─────────────────────────────────────────────────────────────────────────────

    namespace detail {

        template<std::size_t Total, class... Arrays>
        constexpr auto concat_impl(const Arrays&... arrays) {
            std::array<std::uint8_t, Total> result{};

            std::size_t idx = 0;
            auto fill = [&]<std::size_t N>(const std::array<std::uint8_t, N>& a) {
                for (std::size_t i = 0; i < N; ++i) {
                    result[idx++] = a[i];
                }
            };

            (fill.template operator()<std::tuple_size_v<Arrays>>(arrays), ...);

            return result;
        }

        template<class... Arrays>
        constexpr auto concat(const Arrays&... arrays) {
            constexpr std::size_t total = (std::size_t{ 0 } + ... + std::tuple_size_v<Arrays>);
            return concat_impl<total>(arrays...);
        }

        // Empty case
        constexpr auto concat() {
            return std::array<std::uint8_t, 0>{};
        }

    } // namespace detail

    // Public interface – accepts arrays
    template<class... Ts>
        requires(FixedByteArray<Ts> && ...)
    constexpr auto assemble(const Ts&... inputs) {
        return detail::concat(inputs...);
    }

    // Convenience overload for raw literal arrays
    template<std::size_t... Ns>
    constexpr auto assemble(const std::uint8_t (&... arr)[Ns]) {
        return detail::concat(std::to_array(arr)...);
    }

    // ─────────────────────────────────────────────────────────────────────────────
    //  emit – inject raw machine code via inline assembly
    // ─────────────────────────────────────────────────────────────────────────────
    //
    // The emit functions use GCC-style extended inline assembly to inject raw
    // machine code bytes directly into the instruction stream at compile time.
    //
    // Supported compilers and targets:
    //   - Clang or GCC targeting x86 or x86-64
    //
    // NOT supported:
    //   - MSVC: Does not support inline assembly for x64 targets
    //   - clang-cl: Uses MSVC codegen which doesn't support this syntax
    //   - Non-x86 targets: These bytes are x86 instructions
    //
    // Usage requires -O2 or higher optimization to ensure the asm is inlined.

#if (defined(__clang__) || defined(__GNUC__)) && !defined(_MSC_VER) && (defined(__i386__) || defined(__x86_64__))

    namespace detail {

        // Helper to emit bytes using the most efficient directive
        template<std::size_t N>
        inline void emit_bytes(const std::array<std::uint8_t, N>& bytes) {
            if constexpr (N == 0) {
                // nothing
            } else if constexpr (N == 1) {
                asm volatile(".byte %c0" ::"i"(bytes[0]));
            } else if constexpr (N == 2) {
                std::uint16_t v = static_cast<std::uint16_t>(bytes[0]) |
                                  (static_cast<std::uint16_t>(bytes[1]) << 8);
                asm volatile(".short %c0" ::"i"(v));
            } else if constexpr (N == 3) {
                asm volatile(".byte %c0, %c1, %c2" ::"i"(bytes[0]), "i"(bytes[1]), "i"(bytes[2]));
            } else if constexpr (N == 4) {
                std::uint32_t v = static_cast<std::uint32_t>(bytes[0]) |
                                  (static_cast<std::uint32_t>(bytes[1]) << 8) |
                                  (static_cast<std::uint32_t>(bytes[2]) << 16) |
                                  (static_cast<std::uint32_t>(bytes[3]) << 24);
                asm volatile(".long %c0" ::"i"(v));
            } else if constexpr (N <= 8) {
                std::uint64_t v = 0;
                for (std::size_t i = 0; i < N; ++i) {
                    v |= static_cast<std::uint64_t>(bytes[i]) << (i * 8);
                }
                if constexpr (N == 8) {
                    asm volatile(".quad %c0" ::"i"(v));
                } else {
                    // For 5-7 bytes, emit as .long + remaining bytes
                    std::uint32_t lo = static_cast<std::uint32_t>(v);
                    asm volatile(".long %c0" ::"i"(lo));
                    if constexpr (N == 5) {
                        asm volatile(".byte %c0" ::"i"(bytes[4]));
                    } else if constexpr (N == 6) {
                        std::uint16_t hi = static_cast<std::uint16_t>(v >> 32);
                        asm volatile(".short %c0" ::"i"(hi));
                    } else if constexpr (N == 7) {
                        std::uint16_t hi = static_cast<std::uint16_t>(v >> 32);
                        asm volatile(".short %c0" ::"i"(hi));
                        asm volatile(".byte %c0" ::"i"(bytes[6]));
                    }
                }
            } else {
                // For larger arrays, emit in chunks
                constexpr std::size_t chunks = N / 8;
                constexpr std::size_t remainder = N % 8;

                [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                    (
                        [&] {
                            std::uint64_t v = 0;
                            for (std::size_t j = 0; j < 8; ++j) {
                                v |= static_cast<std::uint64_t>(bytes[Is * 8 + j]) << (j * 8);
                            }
                            asm volatile(".quad %c0" ::"i"(v));
                        }(),
                        ...);
                }(std::make_index_sequence<chunks>{});

                if constexpr (remainder > 0) {
                    std::array<std::uint8_t, remainder> tail{};
                    for (std::size_t i = 0; i < remainder; ++i) {
                        tail[i] = bytes[chunks * 8 + i];
                    }
                    emit_bytes<remainder>(tail);
                }
            }
        }

    } // namespace detail

    template<FixedByteArray T>
    inline void emit(const T& code) {
        constexpr std::size_t N = std::tuple_size_v<T>;
        if constexpr (N > 0) {
            detail::emit_bytes<N>(code);
        }
    }

    // Overload for specific array size (common case optimization)
    template<std::size_t N>
    inline void emit(const std::array<std::uint8_t, N>& code) {
        if constexpr (N > 0) {
            detail::emit_bytes<N>(code);
        }
    }

    // Raw array literal convenience
    template<std::size_t N>
    inline void emit(const std::uint8_t (&code)[N]) {
        emit(std::to_array(code));
    }

    // Fallback for function pointers / relocatable values
    template<typename T>
        requires(!FixedByteArray<T>)
    inline void emit(T value) {
        asm volatile(".long %c0" ::"ri"(value));
    }

#else

    // Stub for unsupported compilers or targets.
    // The library's core functionality (compile-time instruction encoding) still works.
    // Only the emit() feature for direct code injection is unavailable.

    template<FixedByteArray T>
    inline void emit([[maybe_unused]] const T& code) {
        static_assert(sizeof(T) == 0,
            "core::emit requires GCC or Clang targeting x86 or x86-64.");
    }

    template<std::size_t N>
    inline void emit([[maybe_unused]] const std::array<std::uint8_t, N>& code) {
        static_assert(N != N,
            "core::emit requires GCC or Clang targeting x86 or x86-64.");
    }

    template<typename T>
        requires(!FixedByteArray<T>)
    inline void emit([[maybe_unused]] T value) {
        static_assert(sizeof(T) == 0,
            "core::emit requires GCC or Clang targeting x86 or x86-64.");
    }

#endif

} // namespace static_asm::core
