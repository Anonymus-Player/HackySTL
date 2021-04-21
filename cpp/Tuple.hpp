#pragma once

#include "IntegerSequence.hpp"
#include "Pair.hpp"

namespace hsd
{
    template <typename... T>
    class tuple
    {
    public:
        static consteval usize size()
        {
            return sizeof...(T);
        }
    };

    template <typename... Args> static constexpr auto make_tuple(Args&&... args);

    template < typename Tuple1, typename Tuple2 >
    using is_same_tuple = is_same<Tuple1, Tuple2>;

    template <typename T>
    class tuple<T>
    {   
    private:   
        T _first;       

    public:
        constexpr tuple() = default;

        constexpr tuple(const T& first) 
            : _first{first}
        {}

        constexpr tuple(const tuple& other)
            : _first{other._first}
        {}

        constexpr tuple& operator=(const tuple& other)
        {
            _first = other._first;
            return *this;
        }

        template <typename... Args>
        constexpr auto operator+(const tuple<Args...>& rhs) const
        {
            return [&]<usize... Ints>(index_sequence<Ints...>)
            {
                [](auto... args) {
                    static_assert(
                        is_same_tuple<
                            tuple<T, Args...>, 
                            tuple<decltype(args)...>
                        >::value, "Invalid concatenation"
                    );
                }(get<0>(), rhs.template get<Ints>()...);

                return make_tuple<T, Args...>(get<0>(), rhs.template get<Ints>()...);
            }(index_sequence_for<Args...>{});
        }

        template <usize N> requires (N == 0)
        constexpr auto get() const
        {
            return _first;
        }

        static consteval usize size()
        {
            return 1;
        }
    };

    template < typename T, typename... Rest >
    class tuple<T, Rest...>
    {   
    private:   
        T _first;
        tuple<Rest...> _rest;        

    public:
        constexpr tuple() = default;

        constexpr tuple(const T& first, const Rest&... rest) 
            : _first(first), _rest(rest...)
        {}

        constexpr tuple(const tuple& other)
            : _first{other._first}, _rest{other._rest}
        {}

        constexpr tuple& operator=(const tuple& other)
        {
            _first = other._first;
            _rest = other._rest;
            return *this;
        }

        template <typename... Args>
        constexpr auto operator+(const tuple<Args...>& rhs) const
        {
            return [&]< usize... Ints1, usize... Ints2 >
                (index_sequence<Ints1...>, index_sequence<Ints2...>)
            {
                [](auto... args) {
                    static_assert(
                        is_same_tuple<
                            tuple<T, Rest..., Args...>, 
                            tuple<decltype(args)...>
                        >::value, "Invalid concatenation"
                    );
                }(get<Ints1>()..., rhs.template get<Ints2>()...);

                return make_tuple<T, Rest..., Args...>(
                    get<Ints1>()..., rhs.template get<Ints2>()...
                );
            }(index_sequence_for<T, Rest...>{}, index_sequence_for<Args...>{});
        }

        template <usize N>
        constexpr auto get() const
        {
            if constexpr(N == 0)
            {
                return _first;
            }
            else
            {
                return _rest.template get<N - 1>();
            }
        }

        static consteval usize size()
        {
            return 1 + sizeof...(Rest);
        }
    };

    template <typename... UTypes> tuple(UTypes...) -> tuple<UTypes...>;
    template < typename T1, typename T2 > tuple(pair<T1, T2>) -> tuple<T1, T2>;

    template <typename... Args>
    static constexpr auto make_tuple(Args&&... args)
    {
        return tuple<decay_t<Args>...>(hsd::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto tie(Args&... args)
    {
        return tuple<decay_t<Args>&...>(args...);
    }

    template < typename Func, typename T, typename... Args, usize... Is >
    requires (std::is_member_function_pointer_v<Func>)
    static constexpr auto apply_impl(Func&& func, T&& value, index_sequence<Is...>, const tuple<Args...>& args) 
    {
        return (value.*func)(args.template get<Is>()...);
    }
    
    template < typename Func, typename T, typename... Args >
    requires (std::is_member_function_pointer_v<Func>)
    static constexpr auto apply(Func&& func, T&& value, const tuple<Args...>& args)
    {
        return apply_impl(forward<Func>(func), forward<T>(value), make_index_sequence<sizeof...(Args)>{}, args);
    }

    template < typename Func, typename T, typename... Args, usize... Is >
    requires (std::is_member_function_pointer_v<Func>)
    static constexpr auto apply_impl(Func&& func, T& value, index_sequence<Is...>, const tuple<Args...>& args) 
    {
        return (value.*func)(args.template get<Is>()...);
    }
    
    template < typename Func, typename T, typename... Args >
    requires (std::is_member_function_pointer_v<Func>)
    static constexpr auto apply(Func&& func, T& value, const tuple<Args...>& args)
    {
        return apply_impl(forward<Func>(func), value, make_index_sequence<sizeof...(Args)>{}, args);
    }

    template < typename Func, typename... Args, usize... Is >
    requires (!std::is_member_function_pointer_v<Func>)
    static constexpr auto apply_impl(Func&& func, index_sequence<Is...>, const tuple<Args...>& args) 
    {
        return func(args.template get<Is>()...);
    }
    
    template < typename Func, typename... Args >
    requires (!std::is_member_function_pointer_v<Func>)
    static constexpr auto apply(Func&& func, const tuple<Args...>& args)
    {
        return apply_impl(forward<Func>(func), make_index_sequence<sizeof...(Args)>{}, args);
    }
}