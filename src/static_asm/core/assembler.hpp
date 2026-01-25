#pragma once

#include <array>
#include <cstddef>
#include <numeric>

namespace static_asm::core {

    // https://stackoverflow.com/questions/25068481/c11-constexpr-flatten-list-of-stdarray-into-array
    template<typename T, size_t... sz>
    inline constexpr auto assemble(std::array<T, sz>... ar) {
        constexpr size_t NB_ARRAY = sizeof...(ar);

        T* datas[NB_ARRAY] = { &ar[0]... };
        constexpr size_t lengths[NB_ARRAY] = { ar.size()... };

        constexpr size_t FLATLENGTH = std::accumulate(lengths, lengths + NB_ARRAY, 0);

        std::array<T, FLATLENGTH> flat_a = { 0 };

        size_t index = 0;
        for (size_t i = 0; i < NB_ARRAY; i++) {
            for (size_t j = 0; j < lengths[i]; j++) {
                flat_a[index] = datas[i][j];
                index++;
            }
        }

        return flat_a;
    }

} // namespace static_asm::core