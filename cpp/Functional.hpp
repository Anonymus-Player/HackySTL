#pragma once

#include "Tuple.hpp"
#include "Result.hpp"
#include "Concepts.hpp"

namespace hsd
{
    template <typename> class function;
    
    namespace func_detail
    {
        template < typename Func, typename Res, typename... Args >
        concept IsFunction = (
            !is_same<Func, function<Res(Args...)>>::value && 
            InvocableRet<Res, Func, Args...>
        );

        struct bad_function
        {
            const char* operator()() const
            {
                return "Bad function";
            }
        };

        template <typename T>
        struct store_ref
        {
            using type = T;
        };
        
        template <typename T>
        struct store_ref<T&>
        {
            using type = reference<T>;
        };

        template <typename T>
        using store_ref_t = typename store_ref<remove_cv_t<T>>::type;
    } // namespace func_detail

    template < typename ResultType, typename... Args >
    class function<ResultType(Args...)> 
    {
    private:
        struct callable_base
        {
            usize _instances = 1;
            virtual ResultType operator()(Args&&...) = 0;
            virtual ~callable_base() {}
        };

        template <typename Func>
        struct callable : callable_base
        {
            Func _func;
            callable(Func func)
                : _func{func}
            {}

            virtual ResultType operator()(Args&&... args) override
            {
                return _func(hsd::forward<Args>(args)...);
            }
        };
        
        callable_base* _func_impl = nullptr;

        HSD_CONSTEXPR void reset()
        {
            if(_func_impl != nullptr)
            {
                if(_func_impl->_instances == 1)
                {
                    delete _func_impl;
                    _func_impl = nullptr;
                }
                else
                    _func_impl->_instances--;
            }
        }
    public:
        function() = default;

        template <typename Func>
        requires (func_detail::IsFunction<Func, ResultType, Args...>)
        HSD_CONSTEXPR function(Func);

        constexpr function(const function&);

        constexpr function(function&& other)
        {
            hsd::swap(_func_impl, other._func_impl);
        }

        constexpr function(NullType) {}

        HSD_CONSTEXPR ~function()
        {
            reset();
        }

        HSD_CONSTEXPR function& operator=(const function& other)
        {
            reset();
            _func_impl = other._func_impl;
            
            if(_func_impl != nullptr)
                _func_impl->instances++;
            
            return *this;
        }

        constexpr function& operator=(function&& other)
        {
            hsd::swap(_func_impl, other._func_impl);
            return *this;
        }

        template <typename Func>
        requires (func_detail::IsFunction<Func, ResultType, Args...>)
        HSD_CONSTEXPR function& operator=(Func&& func)
        {
            reset();
            _func_impl = new callable<Func>(forward<Func>(func));
            return *this;
        }

        HSD_CONSTEXPR function& operator=(NullType)
        {
            reset();
            return *this;
        }

        constexpr auto operator()(Args&&... args) 
            -> Result<
                func_detail::store_ref_t<ResultType>, 
                func_detail::bad_function >
        requires (!is_void<ResultType>::value)
        {
            if(_func_impl == nullptr)
                return func_detail::bad_function{};

            return {_func_impl->operator()(hsd::forward<Args>(args)...)};
        }

        constexpr auto operator()(Args&&... args) 
            -> Result< void, func_detail::bad_function >
        requires (is_void<ResultType>::value)
        {
            if(_func_impl == nullptr)
                return func_detail::bad_function{};

            _func_impl->operator()(hsd::forward<Args>(args)...);
            return {};
        }

        constexpr auto operator()(Args&&... args) const
            -> Result<
                const func_detail::store_ref_t<ResultType>, 
                func_detail::bad_function >
        requires (!is_void<ResultType>::value)
        {
            if(_func_impl == nullptr)
                return func_detail::bad_function{};

            return {_func_impl->operator()(hsd::forward<Args>(args)...)};
        }

        constexpr auto operator()(Args&&... args) const
            -> Result< void, func_detail::bad_function >
        requires (is_void<ResultType>::value)
        {
            if(_func_impl == nullptr)
                return func_detail::bad_function{};

            _func_impl->operator()(hsd::forward<Args>(args)...);
            return {};
        }
    };

    namespace helper
    {
        template <typename>
        struct as_function {};

        template <typename Res, typename Scope, bool Case, typename... Args>
        struct as_function<Res(Scope::*)(Args...) noexcept(Case)>
        {
            using type = Res(Args...);
        };

        template <typename Res, typename Scope, bool Case, typename... Args>
        struct as_function<Res(Scope::*)(Args...)& noexcept(Case)>
        { 
            using type = Res(Args...);
        };

        template <typename Res, typename Scope, bool Case, typename... Args>
        struct as_function<Res(Scope::*)(Args...) const noexcept(Case)>
        { 
            using type = Res(Args...);
        };

        template <typename Res, typename Scope, bool Case, typename... Args>
        struct as_function<Res(Scope::*)(Args...) const& noexcept(Case)>
        { 
            using type = Res(Args...);
        };
    }

    template < typename Res, typename... Args >
    constexpr function<Res(Args...)>::function(const function& other)
    {
        _func_impl = other._func_impl;
        
        if(_func_impl != nullptr)
            _func_impl->_instances++;
    }

    template < typename Res, typename... Args >
    template < typename Func >
    requires (func_detail::IsFunction<Func, Res, Args...>)
    HSD_CONSTEXPR function<Res(Args...)>::function(Func func)
    {
        _func_impl = new callable<Func>(func);
    }

    template < typename Func, typename T, typename... Args >
    requires (std::is_member_function_pointer_v<Func>)
    static HSD_CONSTEXPR auto bind(Func func, T&& value, Args&&... args)
    {
        return [=, value{move(value)}]() mutable {
            if constexpr(requires {value.*(func(std::declval<Args>()...)).unwrap();})
            {
                return (value.*func)(forward<Args>(args)...).unwrap();
            }
            else
            {
                return (value.*func)(forward<Args>(args)...);
            }
        };
    }

    template < typename Func, typename T, typename... Args >
    requires (std::is_member_function_pointer_v<Func>)
    static HSD_CONSTEXPR auto bind(Func func, T&& value, hsd::tuple<Args...>&& args)
    {
        return [=, value{move(value)}]() mutable {
            return [&]<usize... Ints>(hsd::index_sequence<Ints...>)
            {
                if constexpr(requires {(value.*func)(std::declval<Args>()...).unwrap();})
                {
                    return (value.*func)(hsd::forward<Args>(args.template get<Ints>())...).unwrap();
                }
                else
                {
                    return (value.*func)(hsd::forward<Args>(args.template get<Ints>())...);
                }
            }(hsd::index_sequence_for<Args...>{});
        };
    }

    template < typename Func, typename T, typename... Args >
    requires (std::is_member_function_pointer_v<Func>)
    static HSD_CONSTEXPR auto bind(Func func, T& value, Args&&... args)
    {
        return [=, &value]{
            if constexpr(requires {value.*(func(std::declval<Args>()...)).unwrap();})
            {
                return (value.*func)(forward<Args>(args)...).unwrap();
            }
            else
            {
                return (value.*func)(forward<Args>(args)...);
            }
        };
    }

    template < typename Func, typename T, typename... Args >
    requires (std::is_member_function_pointer_v<Func>)
    static HSD_CONSTEXPR auto bind(Func func, T& value, hsd::tuple<Args...>&& args)
    {
        return [=, &value]{
            return [&]<usize... Ints>(hsd::index_sequence<Ints...>)
            {
                if constexpr(requires {(value.*func)(std::declval<Args>()...).unwrap();})
                {
                    return (value.*func)(hsd::forward<Args>(args.template get<Ints>())...).unwrap();
                }
                else
                {
                    return (value.*func)(hsd::forward<Args>(args.template get<Ints>())...);
                }
            }(hsd::index_sequence_for<Args...>{});
        };
    }

    template < typename Func, typename... Args >
    requires (!std::is_member_function_pointer_v<Func>)
    static HSD_CONSTEXPR auto bind(Func func, Args&&... args)
    {
        return [=]{
            if constexpr(requires {func(std::declval<Args>()...).unwrap();})
            {
                return func(forward<Args>(args)...).unwrap();
            }
            else
            {
                return func(forward<Args>(args)...);
            }
        };
    }

    template < typename Func, typename... Args >
    requires (!std::is_member_function_pointer_v<Func>)
    static HSD_CONSTEXPR auto bind(Func func, hsd::tuple<Args...>&& args)
    {
        return [=]{
            return [&]<usize... Ints>(hsd::index_sequence<Ints...>)
            {
                if constexpr(requires {func(std::declval<Args>()...).unwrap();})
                {
                    return func(hsd::forward<Args>(args.template get<Ints>())...).unwrap();
                }
                else
                {
                    return func(hsd::forward<Args>(args.template get<Ints>())...);
                }
            }(hsd::index_sequence_for<Args...>{});
        };
    }

    template < typename Rez, typename... Args > 
    function(Rez(*)(Args...)) -> function<Rez(Args...)>;
    
    template < typename Func, typename Op = decltype(&Func::operator()) > 
    function(Func) -> function<typename helper::as_function<Op>::type>;
} // namespace hsd