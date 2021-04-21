#pragma once

// Requires C++ 17

#include "../Tuple.hpp"
#include "../Functional.hpp"

namespace hsd
{
    struct default_t {};
    constexpr default_t default_v{};

    template <typename S, typename F>
    struct defaultcall_t;

    template <typename F, typename Result, typename... Args>
    struct defaultcall_t<Result(Args...), F>;

    template <size_t Id, typename F, typename Result, typename... Args, typename T>
    constexpr auto default_cast(defaultcall_t<Result(Args...), F> const&, T&& x);
    template <size_t Id, typename F, typename Result, typename... Args>
    constexpr auto default_cast(defaultcall_t<Result(Args...), F> const& c, default_t);

    template <typename F, typename Result, typename... Args>
    struct defaultcall_t<Result(Args...), F>
    {
        tuple<Args...> default_args;
        F func;
        constexpr defaultcall_t(F&& func, Args&&... args) 
            : default_args(hsd::forward<Args>(args)...), func(hsd::forward<F>(func)) 
        {}

        public:
        template <typename... U>
        constexpr Result operator()(U&&... args) const 
        {
            return [&]<usize... Seq>(index_sequence<Seq...>) 
            {
                if constexpr (requires { 
                    func(default_cast<Seq, F, Result, Args...>(*this, hsd::forward<U>(args))...).unwrap(); 
                }) {
                    return func(default_cast<Seq, F, Result, Args...>(*this, hsd::forward<U>(args))...).unwrap();
                } else
                    return func(default_cast<Seq, F, Result, Args...>(*this, hsd::forward<U>(args))...);
            }(index_sequence_for<Args...>{});
        }

    };  

    template <typename S, typename F, typename... Args>
    defaultcall_t<S, F> make_defaultcall(F&& func, Args&&... args)
    {
        return defaultcall_t<S, F>(hsd::forward<F>(func), hsd::forward<Args>(args)...);
    }

    template <size_t Id, typename F, typename Result, typename... Args, typename T>
    requires (!is_same<T, default_t>::value)
    constexpr auto default_cast(defaultcall_t<Result(Args...), F> const&, T&& x) 
    {
        return hsd::forward<T>(x);
    }

    template <size_t Id, typename F, typename Result, typename... Args>
    constexpr auto default_cast(defaultcall_t<Result(Args...), F> const& c, default_t)
    {
        return c.default_args.template get<Id>();
    }

    template <typename Res, typename... Args>
    defaultcall_t(Res(*)(Args...), Args&&...)
        -> defaultcall_t<Res(Args...), Res(*)(Args...)>;

    template <typename Res, typename... Args>
    defaultcall_t(Res(&)(Args...), Args&&...)
        -> defaultcall_t<Res(Args...), Res(&)(Args...)>;

    template <typename Res, typename... Args> 
    defaultcall_t(function<Res(Args...)>&&, Args&&...)
        -> defaultcall_t<Res(Args...), function<Res(Args...)>>;

    template < typename Func, typename... Args, 
        typename Op = decltype(&remove_reference_t<Func>::operator()) > 
    defaultcall_t(Func&&, Args&&...)
        -> defaultcall_t<typename helper::as_function<Op>::type, Func>;
}
