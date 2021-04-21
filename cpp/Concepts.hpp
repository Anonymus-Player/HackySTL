#pragma once

#include "TypeTraits.hpp"

namespace hsd
{
    template <typename T>
    concept IsIntegral = is_integral<T>::value;
    template <typename T>
    concept IsFloat = is_floating_point<T>::value;
    template <typename T>
    concept IsSigned = is_signed<T>::value;
    template <typename T>
    concept IsUnsigned = is_unsigned<T>::value;
    template <typename T>
    concept IsNatural = IsIntegral<T> && IsUnsigned<T>;
    template <typename T>
    concept IsNumber = IsIntegral<T> || IsFloat<T>;
    template <typename T>
    concept IsObject = is_object<T>::value;
    template <typename T>
    concept IsVoid = is_void<T>::value;
    template <typename T>
    concept IsReference = is_reference<T>::value;
    template <typename T>
    concept IsRvalueRef = is_rvalue_reference<T>::value;
    template <typename T>
    concept IsLvalueRef = is_lvalue_reference<T>::value;
    template <typename T>
    concept IsConst = is_const<T>::value;
    template <typename T>
    concept IsArray = is_array<T>::value;
    template <typename T>
    concept IsFunction = is_function<T>::value;
    template <typename T, typename U>
    concept IsSame = is_same<T, U>::value;

    template < typename Func, typename... Args >
    concept Invocable = requires(Func func, Args... args) 
    {
        func(args...);
    };

    template < typename RetType, typename Func, typename... Args >
    concept InvocableRet = ( 
        requires(Func func, Args... args){ {func(args...)} -> IsSame<RetType>; } ||
        requires(Func func, Args... args){ {func(args...).unwrap()} -> IsSame<RetType>; }
    );

    template <typename T>
    concept BasicIterable = (
        requires(T value) { *value; }  &&
        requires(T lhs, T rhs) { {lhs != rhs} -> IsSame<bool>; } &&
        requires(T lhs, T rhs) { {lhs == rhs} -> IsSame<bool>; }
    );

    template <typename T>
    concept ForwardIterable = (
        requires(T value) { value++; } && BasicIterable<T>
    );

    template <typename T>
    concept ReverseIterable = (
        requires(T value) { value--; } && BasicIterable<T>
    );

    template <typename T>
    concept BidirectionalIterable = (
        ReverseIterable<T> &&
        ForwardIterable<T>
    );

    template <typename Container>
    concept IsForwardContainer = (
        requires(Container value) 
        { 
            {value.begin()} -> IsSame<decltype(value.end())>;
        } && requires(Container value) 
        {
            ForwardIterable<decltype(value.end())>;
        } && requires(Container value) 
        {
            {value.size()} -> IsSame<usize>;
        }
    );

    template <typename Container>
    concept IsReverseContainer = (
        requires(Container value) 
        { 
            {value.rbegin()} -> IsSame<decltype(value.rend())>;
        } && requires(Container value) 
        {
            ReverseIterable<decltype(value.rend())>;
        } && requires(Container value) 
        {
            {value.size()} -> IsSame<usize>;
        }
    );

    template <typename T>
    concept IsContainer = IsForwardContainer<T> || IsReverseContainer<T>;

    template <typename T>
    concept IsBidirectionalContainer = IsForwardContainer<T> && IsReverseContainer<T>;
} // namespace hsd
