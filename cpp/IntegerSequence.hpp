#pragma once

#include "TypeTraits.hpp"
#include "Types.hpp"

namespace hsd
{
    template < typename T, T... Ints >
    struct integer_sequence
    {
        static constexpr usize size() noexcept
        {
            return sizeof...(Ints);
        }
    };

    template < typename T, T To, T Prev, T... Next >
    struct make_integer_sequence_helper
        : hsd::conditional_t< Prev == To, integer_sequence< T, Next... >, 
        make_integer_sequence_helper< T, To, Prev - 1, Prev - 1, Next... >>
    {};

    template <usize... Next>
    using index_sequence = integer_sequence<usize, Next...>;

    template <usize Size>
    using make_index_sequence = make_integer_sequence_helper<usize, 0, Size>;
    
    template <typename T, T Size>
    using make_integer_sequence = make_integer_sequence_helper<T, 0, Size>;
    
    template <typename... T>
    using index_sequence_for = make_index_sequence<sizeof...(T)>;
    
    namespace ranged
    {
        template < typename T, T From, T To > requires(From <= To)
        using make_integer_sequence = make_integer_sequence_helper< T, From, To >;

        template < usize From, usize To > requires(From <= To)
        using make_index_sequence = make_integer_sequence_helper< usize, From, To >;
    } // namespace range

    namespace inverse
    {
        template < typename T, T To, T Prev, T... Next >
        struct make_inverse_sequence_helper
            : hsd::conditional_t< Prev == To, integer_sequence< T, Next... >, 
            make_inverse_sequence_helper< T, To, Prev + 1, Prev, Next... >>
        {};

        template <usize Size>
        using make_index_sequence = make_inverse_sequence_helper<usize, Size, 0>;

        template <typename T, T Size>
        using make_integer_sequence = make_inverse_sequence_helper<T, Size, 0>;

        template <typename... T>
        using index_sequence_for = inverse::make_index_sequence<sizeof...(T)>;

        namespace ranged
        {
            template < typename T, T From, T To > requires(From >= To)
            using make_integer_sequence = make_inverse_sequence_helper< T, From, To >;

            template < usize From, usize To > requires(From >= To)
            using make_index_sequence = make_inverse_sequence_helper< usize, From, To >;
        } // namespace range
        
    } // namespace inverse
}