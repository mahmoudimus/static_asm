#pragma once

#include <cstdint>
#include <tuple>

namespace static_asm::core {

    // The emit functions use GCC-style extended inline assembly to inject raw
    // machine code bytes directly into the instruction stream at compile time.
    //
    // Supported compilers:
    //   - Clang (Linux, macOS)
    //   - GCC (Linux)
    //
    // NOT supported:
    //   - MSVC: Does not support inline assembly for x64 targets
    //   - clang-cl: Uses MSVC codegen which doesn't support this syntax
    //
    // Usage requires -O2 or higher optimization to ensure the asm is inlined.

#if (defined(__clang__) || defined(__GNUC__)) && !defined(_MSC_VER)

    template<typename T, typename E, int Size>
    concept Uint8Array = std::same_as<T, std::array<E, Size>>;

    template<typename T>
        requires Uint8Array<T, typename T::value_type, std::tuple_size_v<T>>
    constexpr inline void emit(T array) {
        for (int i = 0; i < sizeof(array); i++) {
            asm volatile(".byte %c0" ::[a] "i"(static_cast<std::uint8_t>(array[i])));
        }
    }

    // fallback impl, has relocatable constraint as it is mainly used for function ptr
    template<typename T>
    constexpr inline void emit(T value) {
        asm volatile(".long %c0" ::[a] "ri"(value));
    }

#else

    // Stub for unsupported compilers (MSVC, clang-cl)
    // MSVC does not support inline assembly for x64, so emit() cannot be implemented.
    // The library's core functionality (compile-time instruction encoding) still works.
    // Only the emit() feature for direct code injection is unavailable.

    template<typename T, typename E, int Size>
    concept Uint8Array = std::same_as<T, std::array<E, Size>>;

    template<typename T>
        requires Uint8Array<T, typename T::value_type, std::tuple_size_v<T>>
    constexpr inline void emit([[maybe_unused]] T array) {
        static_assert(sizeof(T) == 0,
            "emit() is not supported on this compiler. "
            "MSVC does not support inline assembly for x64. "
            "Use Clang or GCC instead.");
    }

    template<typename T>
    constexpr inline void emit([[maybe_unused]] T value) {
        static_assert(sizeof(T) == 0,
            "emit() is not supported on this compiler. "
            "MSVC does not support inline assembly for x64. "
            "Use Clang or GCC instead.");
    }

#endif

} // namespace static_asm::core