#pragma once

#include "Reference.hpp"

#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(HSD_COMPILER_MSVC)
    #define HSD_FUNCTION_NAME __FUNCSIG__
#elif defined(HSD_COMPILER_GCC) || defined(HSD_COMPILER_CLANG)
    #define HSD_FUNCTION_NAME __PRETTY_FUNCTION__
#else
    #define HSD_FUNCTION_NAME __builtin_FUNCTION()
#endif

#define hsd_fprint_check(stream, format, ...)\
do {\
    if(fprintf(stream, format, __VA_ARGS__) == -1)\
        fwprintf(stream, L##format, __VA_ARGS__);\
}while(0)

#define hsd_fputs_check(stream, format)\
do {\
    if(fprintf(stream, format "\n") == -1)\
        fwprintf(stream, L##format L"\n");\
}while(0)

namespace hsd
{
    namespace Result_detail
    {
        template <typename From, typename To>
        concept is_same = hsd::is_same<From, To>::value;

        template <typename T>
        using InvokeType = decltype(std::declval<T>()());

        template < typename Func, typename Result >
        concept UnwrapInvocable = requires(Func a) 
        {
            {a()} -> is_same<Result>;
        };

        template <typename T>
        concept IsReference = (requires {typename T::value_type;} && (
            hsd::is_same<T, reference<typename T::value_type>>::value ||
            hsd::is_same<T, const reference<typename T::value_type>>::value
        ));

        template <typename T>
        concept IsString = requires(T str) 
        {
            {str.c_str()} -> is_same<const char*>;
        };
    } // namespace Result_detail

    struct bad_optional_access
    {
        const char* operator()() const
        {
            return "Acessed an uninitialized value";
        }
    };

    struct ok_value {};
    struct err_value {};
    
    class runtime_error
    {
    protected:
        const char* _err = nullptr;
        
    public:
        constexpr runtime_error(const char* error)
            : _err{error}
        {}

        constexpr const char* operator()() const
        {
            return _err;
        }
    };

    template < typename Ok, typename Err >
    class Result
    {
    private:
        union
        {
            Ok _ok_data;
            Err _err_data;
        };

        bool _initialized = false;
        
    public:
        HSD_CONSTEXPR Result& operator=(const Result&) = delete;
        HSD_CONSTEXPR Result& operator=(Result&&) = delete;

        template <typename T>
        requires (std::is_constructible_v<Ok, T&&>)
        HSD_CONSTEXPR Result(T&& value, ok_value = {})
            : _ok_data{forward<T>(value)}, _initialized{true}
        {}

        template <typename T>
        requires (std::is_constructible_v<Err, T&&>)
        HSD_CONSTEXPR Result(T&& value, err_value = {})
            : _err_data{forward<T>(value)}, _initialized{false}
        {}

        Result(const Result& other)
            : _initialized{other._initialized}
        {
            if(_initialized)
            {
                new(&_ok_data) Ok(other._ok_data);
            }
            else
            {
                new(&_err_data) Err(other._err_data);
            }
        }

        Result(Result&& other)
            : _initialized{other._initialized}
        {
            if(_initialized)
            {
                new(&_ok_data) Ok(move(other._ok_data));
            }
            else
            {
                new(&_err_data) Err(move(other._err_data));
            }
        }

        HSD_CONSTEXPR ~Result()
        {
            if(_initialized)
            {
                _ok_data.~Ok();
            }
            else
            {
                _err_data.~Err();
            }
        }

        constexpr bool is_ok() const
        {
            return _initialized;
        }

        explicit constexpr operator bool() const
        {
            return _initialized;
        }

        constexpr decltype(auto) unwrap(
            const char* func = __builtin_FUNCTION(),
            const char* file_name = __builtin_FILE(), 
            usize line = __builtin_LINE())
        {
            if(_initialized)
            {
                if constexpr(Result_detail::IsReference<Ok>)
                {
                    return static_cast<typename Ok::value_type&>(_ok_data);
                }
                else
                {
                    return release(_ok_data);
                }
            }
            else
            {
                if constexpr(requires {_err_data();})
                {
                    static_assert(
                        is_same<const char*, Result_detail::InvokeType<Err>>::value, 
                        "Error: Invoke result type is not same with \'const char*\'"
                    );
                    
                    hsd_fprint_check(
                        stderr, "Got type error in file:"
                        "\n%s\nInvoked from: %s at line:"
                        " %zu\nWith the Err result value:"
                        " %s\n", file_name, func, line, 
                        _err_data()
                    );
                }
                else if constexpr(Result_detail::IsString<Err>)
                {
                    hsd_fprint_check(
                        stderr, "Got type error in file:"
                        "\n%s\nInvoked from: %s at line:"
                        " %zu\nWith the Err value: %s\n",
                        file_name, func, line, 
                        _err_data.c_str()
                    );
                }
                else
                {
                    hsd_fprint_check(
                        stderr, "Got type error in file:"
                        "\n%s\nInvoked from: %s at line:"
                        " %zu\nWith the Err value: %s\n",
                        file_name, func, line, _err_data
                    );
                }

                abort();
            }
        }

        constexpr decltype(auto) expect(
            const char* message = "Got an Error instead",
            const char* func = __builtin_FUNCTION(),
            const char* file_name = __builtin_FILE(), 
            usize line = __builtin_LINE())
        {
            if(_initialized)
            {
                if constexpr(Result_detail::IsReference<Ok>)
                {
                    return static_cast<typename Ok::value_type&>(_ok_data);
                }
                else
                {
                    return release(_ok_data);
                }
            }
            else
            {
                hsd_fprint_check(
                    stderr, "Got type error in file:"
                    "\n%s\nInvoked from: %s at line:"
                    " %zu\nWith the message: %s\n",
                    file_name, func, line, message
                );
                abort();
            }
        }

        template <typename... Args>
        constexpr decltype(auto) unwrap_or(Args&&... args)
        requires (Result_detail::IsReference<Ok>)
        {
            if(_initialized)
            {
                return static_cast<typename Ok::value_type&>(_ok_data);
            }
            else
            {
                if constexpr(is_const<Ok>::value)
                {
                    return static_cast<const typename Ok::value_type&>(
                        Ok{forward<Args>(args)...}
                    );
                }
                else
                {
                    return static_cast<typename Ok::value_type&>(
                        Ok{forward<Args>(args)...}
                    );
                }
            }
        } 

        template <typename... Args>
        constexpr decltype(auto) unwrap_or(Args&&... args)
        requires (!Result_detail::IsReference<Ok>)
        {
            if(_initialized)
            {
                return release(_ok_data);
            }
            else
            {
                return Ok{forward<Args>(args)...};
            }
        } 
        
        constexpr decltype(auto) unwrap_or_default()
        requires (std::is_default_constructible_v<Ok>)
        {
            if(_initialized)
            {
                return release(_ok_data);
            }
            else
            {
                return Ok{};
            }
        }

        template <typename Func>
        requires (Result_detail::UnwrapInvocable<Func, Ok>)
        constexpr decltype(auto) unwrap_or_else(Func&& func)
        {
            if(_initialized)
            {
                if constexpr(Result_detail::IsReference<Ok>)
                {
                    return static_cast<typename Ok::value_type&>(_ok_data);
                }
                else
                {
                    return release(_ok_data);
                }
            }
            else
            {
                return func();
            }
        }

        constexpr decltype(auto) unwrap_err(
            const char* func = __builtin_FUNCTION(),
            const char* file_name = __builtin_FILE(),
            usize line = __builtin_LINE())
        {
            if(!_initialized)
            {
                if constexpr(Result_detail::IsReference<Err>)
                {
                    if constexpr(is_const<Err>::value)
                    {
                        return static_cast<const typename Err::value_type&>(_err_data);
                    }
                    else
                    {
                        return static_cast<typename Err::value_type&>(_err_data);
                    }
                }
                else
                {
                    return release(_err_data);
                }
            }
            else
            {
                hsd_fprint_check(
                    stderr, "Got type error in file:\n"
                    "%s\nInvoked from: %s at line: %zu"
                    "\nExpected Err, got Ok instead\n",
                    file_name, func, line
                );
                abort();
            }
        }

        constexpr decltype(auto) expect_err(
            const char* message = "Got a Value instead",
            const char* func = __builtin_FUNCTION(), 
            const char* file_name = __builtin_FILE(), 
            usize line = __builtin_LINE())
        {
            if(!_initialized)
            {
                if constexpr(Result_detail::IsReference<Err>)
                {
                    if constexpr(is_const<Err>::value)
                    {
                        return static_cast<const typename Err::value_type&>(_err_data);
                    }
                    else
                    {
                        return static_cast<typename Err::value_type&>(_err_data);
                    }
                }
                else
                {
                    return release(_err_data);
                }
            }
            else
            {
                hsd_fprint_check(
                    stderr, "Got type error in file:"
                    "\n%s\nInvoked from: %s at line:"
                    " %zu\nWith the message: %s\n",
                    file_name, func, line, message
                );
                abort();
            }
        }
    };

    template <typename Err>
    class Result<void, Err>
    {
    private:
        union {
            Err _err_data;
        };
        
        bool _initialized = false;

    public:
        HSD_CONSTEXPR Result(const Result&) = delete;
        HSD_CONSTEXPR Result(Result&&) = delete;
        HSD_CONSTEXPR Result& operator=(const Result&) = delete;
        HSD_CONSTEXPR Result& operator=(Result&&) = delete;
        constexpr void unwrap_or_default() = delete;
        constexpr void unwrap_or() = delete;

        HSD_CONSTEXPR Result()
            : _initialized{true}
        {}

        template <typename T>
        requires (std::is_constructible_v<Err, T&&>)
        HSD_CONSTEXPR Result(T&& value)
            : _err_data{forward<Err>(value)}, _initialized{false}
        {}

        HSD_CONSTEXPR ~Result()
        {
            if(!_initialized)
            {
                _err_data.~Err();
            }
        }
        
        constexpr bool is_ok() const
        {
            return _initialized;
        }

        explicit constexpr operator bool() const
        {
            return _initialized;
        }

        constexpr void unwrap(
            const char* func = __builtin_FUNCTION(),
            const char* file_name = __builtin_FILE(),
            usize line = __builtin_LINE())
        {
            if(!_initialized)
            {
                if constexpr(requires {_err_data();})
                {
                    static_assert(
                        is_same<const char*, Result_detail::InvokeType<Err>>::value, 
                        "Error: Invoke result type is not same with \'const char*\'"
                    );
                    
                    hsd_fprint_check(
                        stderr, "Got type error in file:"
                        "\n%s\nInvoked from: %s at line:"
                        " %zu\nWith the Err result value:"
                        " %s\n", file_name, func, line, 
                        _err_data()
                    );
                }
                else if constexpr(Result_detail::IsString<Err>)
                {
                    hsd_fprint_check(
                        stderr, "Got type error in file:"
                        "\n%s\nInvoked from: %s at line:"
                        " %zu\nWith the Err value: %s\n",
                        file_name, func, line, 
                        _err_data.c_str()
                    );
                }
                else
                {
                    hsd_fprint_check(
                        stderr, "Got type error in file:"
                        "\n%s\nInvoked from: %s at line:"
                        " %zu\nWith the Err value: %s\n",
                        file_name, func, line, _err_data
                    );
                }
                
                abort();
            }
        }

        constexpr void expect(
            const char* message = "Got an Error instead",
            const char* func = __builtin_FUNCTION(),
            const char* file_name = __builtin_FILE(),
            usize line = __builtin_LINE())
        {
            if(!_initialized)
            {
                hsd_fprint_check(
                    stderr, "Got type error in file:"
                    "\n%s\nInvoked from: %s at line:"
                    " %zu\nWith the message: %s\n",
                    file_name, func, line, message
                );
                abort();
            }
        }

        template <typename Func>
        requires (Result_detail::UnwrapInvocable<Func, void>)
        constexpr void unwrap_or_else(Func&& func)
        {
            if(!_initialized)
            {
                func();
            }
        }

        constexpr decltype(auto) unwrap_err(
            const char* func = __builtin_FUNCTION(),
            const char* file_name = __builtin_FILE(),
            usize line = __builtin_LINE())
        {
            if(_initialized)
            {
                hsd_fprint_check(
                    stderr, "Got type error in file:"
                    "\n%s\nInvoked from: %s at line:"
                    " %zu\n", file_name, func, line
                );
                abort();
            }
            else
            {
                if constexpr(Result_detail::IsReference<Err>)
                {
                    return static_cast<typename Err::value_type&>(_err_data);
                }
                else
                {
                    return release(_err_data);
                }
            }
        }

        constexpr decltype(auto) expect_err(
            const char* message = "Got a Value instead",
            const char* func = __builtin_FUNCTION(),
            const char* file_name = __builtin_FILE(), 
            usize line = __builtin_LINE())
        {
            if(_initialized)
            {
                hsd_fprint_check(
                    stderr, "Got type error in file:"
                    "\n%s\nInvoked from: %s at line:"
                    " %zu\nWith the message: %s\n",
                    file_name, func, line, message);
                abort();
            }
        }
    };

    template <typename T>
    using optional = Result<T, bad_optional_access>;
} // namespace hsd
