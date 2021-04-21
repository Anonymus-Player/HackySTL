#pragma once

#include <bitset>

#include <TypeTraits.hpp>
#include <IntegerSequence.hpp>

#include "BooleanLogic.hpp"
#include "BinaryArithmetic.hpp"

namespace hsd {
    template <usize N>
    struct proxy_bitset {
        uchar data[N] {};

        constexpr void set(usize i, bool v) {
            data[i] = v;
        }

        constexpr operator std::bitset<N>() const {
            std::bitset<N> b;
            for (usize i = 0; i < N; ++i)
                b.set(i, data[i]);
            return b;
        }

        constexpr auto bs() const {
            return static_cast<std::bitset<N> >(*this);
        }
    };

    template <Number v>
    constexpr auto number_value() {
        constexpr usize bits = number_bits_v<v>;
        proxy_bitset<bits> s;
        // Compile-time loop tiem
        [&]<usize... i>(index_sequence<i...>) {
            (s.set(i, number_get_bit_v<v, i>), ...);
        }(make_index_sequence<bits>());
        return s;
    }

    template <>
    struct serialize_value<bit::zero>
        : literal_constant<bool, 0> {};

    template <>
    struct serialize_value<bit::one>
        : literal_constant<bool, 1> {};

    template <Number v>
    struct serialize_value<v>
        : literal_constant<proxy_bitset<number_bits_v<v> >, number_value<v>()> {};
}
