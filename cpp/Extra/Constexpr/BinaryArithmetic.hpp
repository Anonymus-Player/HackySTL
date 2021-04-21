#pragma once

#include <TypeTraits.hpp>

#include "BooleanLogic.hpp"

namespace hsd 
{
    // Complex operations
    template <Bit S, Bit C>
    struct sum_carry_pair 
    {
        using sum = S;
        using carry = C;
    };

    template <Bit A, Bit B>
    using half_adder = sum_carry_pair<op_xor<A, B>, op_and<A, B>>;

    template <Bit A, Bit B, Bit C>
    struct _full_adder 
    {
        using partial1 = half_adder<A, B>;
        using partial2 = half_adder<typename partial1::sum, C>;
        using partial3 = op_xor<typename partial1::carry, typename partial2::carry>;

        using type = sum_carry_pair<typename partial2::sum, partial3>;
    };

    template <Bit A, Bit B, Bit C>
    using full_adder = typename _full_adder<A, B, C>::type;

    template <typename F, typename R>
    struct cons_pair 
    {
        using first = F;
        using rest = R;
    };

    struct nil {};

    namespace binary_helper
    {
        template <typename T>
        struct is_number : false_type {};

        template <Bit F, typename R>
        struct is_number<cons_pair<F, R>> : is_number<R> {};

        template <>
        struct is_number<nil> : true_type {};
    } // namespace binary_helper

    template <typename T>
    concept Number = binary_helper::is_number<T>::value;

    // Gamma operations
    namespace natural 
    {
        using zero = nil;
        using one = cons_pair<bit::one, nil>;
        using two = cons_pair<bit::zero, cons_pair<bit::one, nil> >;
        using three = cons_pair<bit::one, cons_pair<bit::one, nil> >;
        using four = cons_pair<bit::zero, cons_pair<bit::zero, cons_pair<bit::one, nil> > >;
    }

    template <Number A, Number B, Bit C>
    struct _ripple_adder 
    {
        // General case
        using partial = full_adder<typename A::first, typename B::first, C>;

        using type = cons_pair<
            typename partial::sum, 
            typename _ripple_adder<
                typename A::rest, 
                typename B::rest, 
                typename partial::carry
            >::type>;
    };

    template <> 
    struct _ripple_adder<nil, nil, bit::zero> 
    { 
        using type = nil; 
    };

    template <> 
    struct _ripple_adder<nil, nil, bit::one> 
    { 
        using type = cons_pair<bit::one, nil>; 
    };

    template <Bit F, Number R, Bit C>
    struct _ripple_adder<cons_pair<F, R>, nil, C> 
    {
        using partial = half_adder<F, C>;
        using type = cons_pair<
            typename partial::sum, 
            typename _ripple_adder<R, nil, 
            typename partial::carry>::type>;
    };

    template <Bit F, Number R, Bit C>
    struct _ripple_adder<nil, cons_pair<F, R>, C> : _ripple_adder<cons_pair<F, R>, nil, C> {};

    template <Number A, Number B, Bit C>
    using ripple_adder = typename _ripple_adder<A, B, C>::type;

    template <Number N>
    struct number_bits;

    template <Number N>
    constexpr usize number_bits_v = number_bits<N>::value;

    template <>
    struct number_bits<nil>
        : literal_constant<usize, 0> {};
    
    template <Bit F, Number R>
    struct number_bits<cons_pair<F, R>>
        : literal_constant<usize, 1 + number_bits_v<R> > {};

    template <Number N, usize I>
    struct number_get_bit;

    template <Number N, usize I>
    constexpr usize number_get_bit_v = number_get_bit<N, I>::value;

    template <usize I>
    struct number_get_bit<nil, I> {}; // No value

    template <Bit F, Number R>
    struct number_get_bit<cons_pair<F, R>, 0>
        : literal_constant<bool, is_one_v<F> > {};
    
    template <Bit F, Number R, usize I>
    struct number_get_bit<cons_pair<F, R>, I>
        : number_get_bit<R, I-1> {};
}
