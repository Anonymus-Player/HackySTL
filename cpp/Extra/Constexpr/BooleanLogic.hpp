#pragma once

#include <type_traits>
#include "../../TypeTraits.hpp"

namespace hsd 
{
    namespace bit 
    {
        // Basic values
        struct bitval {};
        struct zero : bitval {};
        struct one : bitval {};
    }

    // Basic traits
    template <typename T>
    using is_bit = ::std::is_base_of<bit::bitval, T>;

    template <typename T>
    concept Bit = is_bit<T>::value;

    template <typename T> 
    struct is_zero;

    template <> 
    struct is_zero<bit::zero> : true_type 
    {};

    template <> 
    struct is_zero<bit::one> : false_type 
    {};

    template <typename T> 
    static constexpr inline bool is_zero_v = is_zero<T>::value;

    template <typename T> 
    struct is_one;

    template <> 
    struct is_one<bit::zero> : false_type 
    {};
    
    template <> 
    struct is_one<bit::one> : true_type 
    {};

    template <typename T> 
    static constexpr inline bool is_one_v = is_one<T>::value;

    // Operations
    template <Bit A, Bit B, Bit V1, Bit V2, Bit V3, Bit V4>
    struct _binary_operation 
    {
        using type = conditional_t<
            is_zero_v<B>,
            conditional_t<
                is_zero_v<A>,
                V1, V2
            >,
            conditional_t<
                is_zero_v<A>,
                V3, V4
            >
        >;
    };

    template <Bit A, Bit B, Bit V1, Bit V2, Bit V3, Bit V4>
    using binary_operation = typename _binary_operation<A, B, V1, V2, V3, V4>::type;

    template <Bit A, Bit B>
    using op_or = binary_operation<A, B, bit::zero, bit::one, bit::one, bit::one>;

    template <Bit A, Bit B>
    using op_and = binary_operation<A, B, bit::zero, bit::zero, bit::zero, bit::one>;

    template <Bit A, Bit B>
    using op_xor = binary_operation<A, B, bit::zero, bit::one, bit::one, bit::zero>;


    // Serialization traits
    template <typename V>
    struct serialize_value;

    template <typename V>
    constexpr auto serialize_value_v = serialize_value<V>::value;
}
