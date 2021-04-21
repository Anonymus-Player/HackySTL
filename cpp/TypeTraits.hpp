#pragma once

#include <type_traits>
#include "Types.hpp"

namespace hsd
{
    template <typename T, T v>
    struct literal_constant
    {
        using type = T;
        static inline constexpr T value = v;
    };
    
    using false_type = literal_constant<bool, false>;
    using true_type = literal_constant<bool, true>;

    template <bool, typename>
    struct enable_if
    {};

    template <typename T>
    struct enable_if<true, T>
    {
        using type = T;
    };

    template <bool, typename>
    struct disable_if
    {};

    template <typename T>
    struct disable_if<false, T>
    {
        using type = T;
    };

    template <typename... Cond> requires(sizeof...(Cond) > 1)
    struct conjunction
    {
        static constexpr bool value = (Cond::value && ...);
    };

    template <typename... Cond> requires(sizeof...(Cond) > 1)
    struct disjunction
    {
        static constexpr bool value = (Cond::value || ...);
    };

    template <typename Cond>
    struct negation
    {
        static constexpr bool value = !Cond::value;
    };

    template < bool Cond, typename IfTrue, typename IfFalse >
    struct conditional
    {
        using type = IfTrue;
    };
    
    template < typename IfTrue, typename IfFalse >
    struct conditional< false, IfTrue, IfFalse >
    {
        using type = IfFalse;
    };

    template <typename>
    struct is_array
        : public false_type 
    {};

    template <typename T, usize size>
    struct is_array<T[size]>
        : public true_type 
    {};

    template <typename T>
    struct is_array<T[]> 
        : public true_type 
    {};

    template <typename, typename>
    struct is_same 
        : public false_type
    {};
    
    template <typename T>
    struct is_same<T, T>
        : public true_type
    {};

    template <typename T> 
    struct is_const                
        : public false_type
    {};

    template <typename T> 
    struct is_const<const T>       
        : public true_type
    {};

    template <typename>
    struct is_lvalue_reference
        : public false_type
    {};

    template <typename T>
    struct is_lvalue_reference<T&>
        : public true_type
    {};

    template <typename>
    struct is_rvalue_reference
        : public false_type
    {};

    template <typename T>
    struct is_rvalue_reference<T&&>
        : public true_type 
    {};

    template <typename T>
    struct is_reference
        : public disjunction<
            is_rvalue_reference<T>, 
            is_lvalue_reference<T> >
    {};

    namespace helper
    {
        template <typename>
        struct is_void
            : public false_type
        {};
        
        template <>
        struct is_void<void>
            : public true_type
        {};

        template <typename>
        struct is_integral
            : public false_type
        {};

        template <>
        struct is_integral<bool>
            : public true_type
        {};

        template <>
        struct is_integral<char>
            : public true_type
        {};
        
        template <>
        struct is_integral<char8>
            : public true_type
        {};

        template <>
        struct is_integral<char16>
            : public true_type
        {};

        template <>
        struct is_integral<char32>
            : public true_type
        {};

        template <>
        struct is_integral<i16>
            : public true_type
        {};

        template <>
        struct is_integral<i32>
            : public true_type
        {};

        template <>
        struct is_integral<i64>
            : public true_type
        {};

        #if defined(HSD_COMPILER_GCC)
        
        template <>
        struct is_integral<i128>
            : public true_type
        {};
        
        #endif

        template <>
        struct is_integral<long>
            : public true_type
        {};

        template <>
        struct is_integral<uchar>
            : public true_type
        {};

        template <>
        struct is_integral<u16>
            : public true_type
        {};

        template <>
        struct is_integral<u32>
            : public true_type
        {};

        template <>
        struct is_integral<u64>
            : public true_type
        {};

        #if defined(HSD_COMPILER_GCC)

        template <>
        struct is_integral<u128>
            : public true_type
        {};

        #endif

        template <>
        struct is_integral<ulong>
            : public true_type
        {};

        template <typename>
        struct is_floating_point
            : public false_type
        {};

        template <>
        struct is_floating_point<f32>
            : public true_type
        {};

        template <>
        struct is_floating_point<f64>
            : public true_type
        {};

        template <>
        struct is_floating_point<f128>
            : public true_type
        {};

        template <typename T>
        struct is_number
            : disjunction< is_integral<T>, is_floating_point<T> >
        {};

        template < typename T, bool = is_number<T>::value >
        struct is_signed : public false_type 
        {};

        template <typename T>
        struct is_signed<T, true>
            : public literal_constant< bool, T(-1) < T(0) >
        {};

        template <typename T>
        struct is_unsigned
            : public negation<is_signed<T>>
        {};
    }

    template <typename T>
    struct remove_reference
    {
        using type = T;
    };

    template <typename T>
    struct remove_reference<T&>
    {
        using type = T;
    };

    template <typename T>
    struct remove_reference<T&&>
    {
        using type = T;
    };

    template <typename T>
    struct remove_array
    {
        using type = T;
    };

    template <typename T>
    struct remove_array<T[]>
    {
        using type = T;
    };

    template <typename T, usize N>
    struct remove_array<T[N]>
    {
        using type = T;
    };

    template <typename T> 
    struct remove_cv                   
    {
        typedef T type;
    };

    template <typename T> 
    struct remove_cv<const T>          
    {
        typedef T type; 
    };

    template <typename T> 
    struct remove_cv<volatile T>       
    {
        typedef T type; 
    };

    template <typename T> 
    struct remove_cv<const volatile T> 
    {
        typedef T type; 
    };
    
    template <typename T> 
    struct remove_const                
    {
        typedef T type; 
    };

    template <typename T> 
    struct remove_const<const T>       
    {
        typedef T type; 
    };

    template <typename T> 
    struct remove_volatile             
    {
        typedef T type; 
    };

    template <typename T> 
    struct remove_volatile<volatile T> 
    {
        typedef T type;
    };

    namespace va_args
    {
        namespace regular
        {
            template <typename>
            struct is_function 
                : false_type
            {};
    
            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...,...)>
                : true_type 
            {};
        } // namespace regular

        namespace cv
        {
            template <typename>
            struct is_function 
                : false_type
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) const> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) volatile>
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) const volatile>
                : true_type
            {};
        } // namespace cv

        namespace ref
        {
            template <typename>
            struct is_function 
                : false_type
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) &> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) const &> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) volatile &> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) const volatile &> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) &&> 
                : true_type 
            {};
            
            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) const &&> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) volatile &&> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) const volatile &&> 
                : true_type 
            {};
        } // namespace ref

        namespace noexp
        {
            template <typename>
            struct is_function 
                : false_type
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) noexcept> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) const noexcept> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) volatile noexcept>
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) const volatile noexcept>
                : true_type
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) & noexcept> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) const & noexcept> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) volatile & noexcept> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) const volatile & noexcept> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) && noexcept> 
                : true_type 
            {};
            
            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) const && noexcept> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) volatile && noexcept> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args..., ...) const volatile && noexcept> 
                : true_type 
            {};
        } // namespace noexp

        template <typename T>
        struct is_va_args_function
            : disjunction<
                regular::is_function<T>, cv::is_function<T>, 
                ref::is_function<T>, noexp::is_function<T> >
        {};
        
    } // namespace va_args

    namespace reg_args
    {
        namespace regular
        {
            template <typename>
            struct is_function 
                : false_type 
            {};
    
            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...)> 
                : true_type 
            {};
        } // namespace regular
        
        namespace cv
        {
            template <typename>
            struct is_function 
                : false_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) const> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) volatile>
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) const volatile>
                : true_type
            {};
        } // namespace cv

        namespace ref
        {
            template <typename>
            struct is_function 
                : false_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) &> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) const &> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) volatile &> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) const volatile &> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) &&> 
                : true_type 
            {};
            
            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) const &&> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) volatile &&> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) const volatile &&> 
                : true_type 
            {};
        } // namespace ref
        
        namespace noexp
        {
            template <typename>
            struct is_function 
                : false_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) noexcept> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) const noexcept> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) volatile noexcept>
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) const volatile noexcept>
                : true_type
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) & noexcept> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) const & noexcept> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) volatile & noexcept> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) const volatile & noexcept> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) && noexcept> 
                : true_type 
            {};
            
            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) const && noexcept> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) volatile && noexcept> 
                : true_type 
            {};

            template < typename Ret, typename... Args >
            struct is_function<Ret(Args...) const volatile && noexcept> 
                : true_type 
            {};
        } // namespace noexp

        template <typename T>
        struct is_reg_args_function
            : disjunction<
                regular::is_function<T>, cv::is_function<T>, 
                ref::is_function<T>, noexp::is_function<T> >
        {};
    } // namespace reg_args

    template <typename T>
    struct is_function
        : disjunction<
            reg_args::is_reg_args_function<T>, 
            va_args::is_va_args_function<T> >
    {};

    template<typename T>
    struct is_object
        : public negation< 
            disjunction< is_function<T>, is_reference<T>,
            helper::is_void< typename remove_cv<T>::type > > 
        >::type
    {};

    namespace sfinae
    {
        template <typename T>
        struct type_identity
        {
            using type = T;
        };

        template <typename T>
        constexpr auto try_add_pointer(int) -> type_identity< typename remove_reference<T>::type* >;
        template <typename T>
        constexpr auto try_add_pointer(...) -> type_identity<T>;

        template <typename T> 
        T&& declval(int);

        template <typename T>
        T declval(long);
    } // namespace sfinae

    template <typename T>
    struct add_pointer
        : decltype(sfinae::try_add_pointer<T>(0)) 
    {};

    template <typename T>
    struct decay 
    {
    private:
        using U = typename remove_reference<T>::type;

    public:
        using type = typename conditional< 
            is_array<U>::value,
            typename remove_array<U>::type*,
            typename conditional< 
                is_function<U>::value,
                typename add_pointer<U>::type,
                typename remove_cv<U>::type
            >::type
        >::type;
    };

    template <typename T>
    auto declval() -> decltype(sfinae::declval<T>(0));

    template < typename Condition, typename Value >
    using ResolvedType = typename enable_if< Condition::value, Value >::type;

    template < typename Condition, typename Value >
    using UnResolvedType = typename disable_if< Condition::value, Value >::type;

    template < typename From, typename To, typename Result >
	using EnableIfConvertible = ResolvedType< std::is_convertible<From, To>, Result >;

    template < typename From, typename To, typename Result >
	using DisableIfConvertible = UnResolvedType< std::is_convertible<From, To>, Result >;

    template < typename First, typename Second, typename Result >
	using EnableIfSame = ResolvedType< is_same<First, Second>, Result >;

    template < typename First, typename Second, typename Result >
	using DisableIfSame = ResolvedType< is_same<First, Second>, Result >;

    template <typename T>
    using is_integral = helper::is_integral< typename remove_cv<T>::type >;

    template <typename T>
    using is_floating_point = helper::is_floating_point< typename remove_cv<T>::type >;

    template <typename T>
    using is_number = helper::is_number< typename remove_cv<T>::type >;

    template <typename T>
    using is_signed = helper::is_signed< typename remove_cv<T>::type >;

    template <typename T>
    using is_unsigned = helper::is_unsigned< typename remove_cv<T>::type >;

    template <typename T>
    using is_void = helper::is_void< typename remove_cv<T>::type >;

    template < bool Cond, typename IfTrue, typename IfFalse >
    using conditional_t = typename conditional< Cond, IfTrue, IfFalse >::type;

    template <typename T>
    using remove_array_t = typename remove_array<T>::type;

    template <typename T>
    using remove_cv_t = typename remove_cv<T>::type;

    template <typename T>
    using remove_cvref_t = typename remove_cv<typename remove_reference<T>::type>::type;

    template <typename T>
    using remove_const_t = typename remove_const<T>::type;

    template <typename T>
    using remove_volatile_t = typename remove_volatile<T>::type;

    template <typename T>
    using remove_reference_t = typename remove_reference<T>::type;

    template <typename T>
    using decay_t = typename decay<T>::type;

    template < bool Cond, typename T = void >
    using enable_if_t = typename enable_if<Cond, T>::type;

    template < bool Cond, typename T = void >
    using disable_if_t = typename disable_if<Cond, T>::type;
} // namespace hsd
