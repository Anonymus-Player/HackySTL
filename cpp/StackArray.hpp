#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "Utility.hpp"

namespace hsd
{
    template < typename T, usize N >
    struct stack_array
    {
        T _array[N]{};
        using iterator = T*;
        using const_iterator = const T*;

        constexpr T& operator[](usize index)
        {
            return _array[index];
        }
        
        constexpr const T& operator[](usize index) const
        {
            return _array[index];
        }

        constexpr T& at(usize index)
        {
            if(index >= N)
            {
                puts("Tried to access elements out of bounds");
                abort();
            }

            return {_array[index]};
        }

        constexpr const T& at(usize index) const
        {
            if(index >= N)
            {
                puts("Tried to access elements out of bounds");
                abort();
            }

            return {_array[index]};
        }

        template < usize U, usize L >
        constexpr auto gen_range()
        {
            static_assert(L - U <= N, "Out of range\n");

            return [&]<usize... Ints>(index_sequence<Ints...>)
            {
                return stack_array<T, L - U>{_array[Ints]...};
            }(make_index_sequence<L - U>{});
        }

        constexpr usize size()
        {
            return N;
        }

        constexpr usize size() const
        {
            return N;
        }

        constexpr iterator data()
        {
            return _array;
        }

        constexpr const_iterator data() const
        {
            return _array;
        }

        constexpr iterator begin()
        {
            return data();
        }

        constexpr const_iterator begin() const
        {
            return cbegin();
        }

        constexpr iterator end()
        {
            return begin() + size();
        }

        constexpr const_iterator end() const
        {
            return cend();
        }

        constexpr const_iterator cbegin() const
        {
            return data();
        }

        constexpr const_iterator cend() const
        {
            return cbegin() + size();
        }
    };

    template < typename L, typename... U > stack_array(L, U...) -> stack_array<L, 1 + sizeof...(U)>;

    template <typename T, usize N>
    static constexpr void swap(stack_array<T, N>& first, stack_array<T, N>& second)
    {
        for(usize _index = 0; _index < N; _index++)
        {
            swap(first[_index], second[_index]);
        }
    }
}