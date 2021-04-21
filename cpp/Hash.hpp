#pragma once

#include "Utility.hpp"
#include "Concepts.hpp"

namespace hsd
{
    template < typename HashType, typename T = void >
    struct hash
    {
        // this is an alias for HashType
        using ResultType = HashType;
    };

    template < typename HashType, typename T >
    requires (ForwardIterable<T> || ReverseIterable<T>)
    struct hash<HashType, T>
    {
        using ResultType = HashType;

        // Use the FNV-1a algorithm
        static constexpr ResultType get_hash(T begin)
        {
            HashType offset_basis = 0;
            HashType prime = 0;

            if constexpr(sizeof(HashType) == sizeof(u64))
            {
                offset_basis = 14'695'981'039'346'656'037u;
                prime = 1'099'511'628'211u;
            }
            else if constexpr(sizeof(HashType) == sizeof(u32)) 
            {
                offset_basis = 2'166'136'261u;
                prime = 16'777'619u;
            }

            HashType hash = offset_basis;

            while(*begin != '\0') 
            {
                hash = (hash ^ static_cast<HashType>(*begin)) * prime;
                begin++;
            }

            return hash;
        }

        static constexpr ResultType get_hash(T begin, T end)
        requires (ForwardIterable<T>)
        {
            HashType offset_basis = 0;
            HashType prime = 0;

            if constexpr(sizeof(HashType) == sizeof(u64))
            {
                offset_basis = 14'695'981'039'346'656'037u;
                prime = 1'099'511'628'211u;
            }
            else if constexpr(sizeof(HashType) == sizeof(u32)) 
            {
                offset_basis = 2'166'136'261u;
                prime = 16'777'619u;
            }

            HashType hash = offset_basis;

            while(begin != end) 
            {
                hash = (hash ^ static_cast<HashType>(*begin)) * prime;
                begin++;
            }

            return hash;
        }
    };

    template < typename HashType, typename T >
    requires (is_integral<T>::value)
    struct hash<HashType, T>
    {
        using ResultType = HashType;

        static constexpr ResultType get_hash(T number)
        {
            return static_cast<HashType>(number);
        }
    };
} // namespace hsd