#pragma once

#include <stdio.h>
#include <wchar.h>
#include <locale.h>

#include "_IoOverload.hpp"
#include "SStream.hpp"

namespace hsd
{
    namespace io_detail
    {
        class bufferable
        {
        protected:
            static inline sstream _u8io_buf{4096};
            static inline wsstream _wio_buf{4096};
        };

        template <string_literal str>
        inline void _print(FILE* file_buf = stdout)
        {
            fprintf(file_buf, str.data);
        }

        template <string_literal str>
        inline void _print(bool val, FILE* file_buf = stdout)
        {
            if(val == true)
            {
                fprintf(file_buf, basic_string_literal(str, "true").data);
            }
            else
            {
                fprintf(file_buf, basic_string_literal(str, "false").data);
            }
        }

        template <string_literal str>
        inline void _print(char val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, basic_string_literal(str, "%c").data, val);
        }

        template <string_literal str>
        inline void _print(char8 val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, basic_string_literal(str, "%zc").data, val);
        }

        template <string_literal str>
        inline void _print(i16 val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, basic_string_literal(str, "%hd").data, val);
        }

        template <string_literal str>
        inline void _print(u16 val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, basic_string_literal(str, "%hu").data, val);
        }

        template <string_literal str>
        inline void _print(i32 val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, basic_string_literal(str, "%d").data, val);
        }

        template <string_literal str>
        inline void _print(u32 val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, basic_string_literal(str, "%u").data, val);
        }

        template <string_literal str>
        inline void _print(i64 val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, basic_string_literal(str, "%lld").data, val);
        }

        template <string_literal str>
        inline void _print(u64 val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, basic_string_literal(str, "%llu").data, val);
        }

        template <string_literal str>
        inline void _print(long val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, basic_string_literal(str, "%zd").data, val);
        }

        template <string_literal str>
        inline void _print(ulong val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, basic_string_literal(str, "%zu").data, val);
        }

        template <string_literal str>
        inline void _print(f32 val, FILE* file_buf = stdout)
        {
            auto _res = math::abs(math::floor(val) - val);

            if((_res < 0.0001 && _res != 0) || math::abs(val) > 1.e+10)
            {
                fprintf(file_buf, basic_string_literal(str, "%e").data, val);
            }
            else
            {
                fprintf(file_buf, basic_string_literal(str, "%f").data, val);
            }
        }

        template <string_literal str>
        inline void _print(f64 val, FILE* file_buf = stdout)
        {
            auto _res = math::abs(math::floor(val) - val);

            if((_res < 0.0001 && _res != 0) || math::abs(val) > 1.e+10)
            {
                fprintf(file_buf, basic_string_literal(str, "%e").data, val);
            }
            else
            {
                fprintf(file_buf, basic_string_literal(str, "%lf").data, val);
            }
        }

        template <string_literal str>
        inline void _print(f128 val, FILE* file_buf = stdout)
        {
            auto _res = math::abs(math::floor(val) - val);

            if((_res < 0.0001 && _res != 0) || math::abs(val) > 1.e+10)
            {
                fprintf(file_buf, basic_string_literal(str, "%Le").data, val);
            }
            else
            {
                fprintf(file_buf, basic_string_literal(str, "%Lf").data, val);
            }
        }

        template <string_literal str>
        inline void _print(char* val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, basic_string_literal(str, "%s").data, val);
        }

        template <string_literal str>
        inline void _print(char8* val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, basic_string_literal(str, "%s").data, val);
        }

        template <string_literal str>
        inline void _print(const char* val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, basic_string_literal(str, "%s").data, val);
        }

        template <string_literal str>
        inline void _print(const char8* val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, basic_string_literal(str, "%s").data, val);
        }
        
        template <string_literal str, typename... Args>
        inline void _print(const tuple<Args...>& val, FILE* file_buf = stdout)
        {
            fprintf(file_buf, basic_string_literal(str, "(").data);

            if constexpr(val.size() == 1)
            {
                _print<"">(val.template get<0>(), file_buf);
            }
            else if constexpr(val.size() > 1)
            {
                [&]<usize... Ints>(index_sequence<Ints...>) {
                    ((
                        _print<"">(val.template get<Ints>(), file_buf), 
                        _print<", ">(file_buf)
                    ), ...);
                }(make_index_sequence<val.size() - 1>{});
                
                _print<"">(val.template get<val.size() - 1>());
            }

            fprintf(file_buf, ")");
        }

        template <wstring_literal str>
        inline void _print(FILE* file_buf = stdout)
        {
            fwprintf(file_buf, str.data);
        }

        template <wstring_literal str>
        inline void _print(bool val, FILE* file_buf = stdout)
        {
            if(val == true)
            {
                fwprintf(file_buf, basic_string_literal(str, L"true").data);
            }
            else
            {
                fwprintf(file_buf, basic_string_literal(str, L"false").data);
            }
        }

        template <wstring_literal str>
        inline void _print(wchar val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, basic_string_literal(str, L"%lc").data, val);
        }

        template <wstring_literal str>
        inline void _print(char val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, basic_string_literal(str, L"%c").data, val);
        }

        template <wstring_literal str>
        inline void _print(char8 val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, basic_string_literal(str, L"%zc").data, val);
        }

        template <wstring_literal str>
        inline void _print(i16 val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, basic_string_literal(str, L"%hd").data, val);
        }

        template <wstring_literal str>
        inline void _print(u16 val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, basic_string_literal(str, L"%hu").data, val);
        }

        template <wstring_literal str>
        inline void _print(i32 val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, basic_string_literal(str, L"%d").data, val);
        }

        template <wstring_literal str>
        inline void _print(u32 val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, basic_string_literal(str, L"%u").data, val);
        }

        template <wstring_literal str>
        inline void _print(i64 val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, basic_string_literal(str, L"%lld").data, val);
        }

        template <wstring_literal str>
        inline void _print(u64 val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, basic_string_literal(str, L"%llu").data, val);
        }

        template <wstring_literal str>
        inline void _print(long val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, basic_string_literal(str, L"%zd").data, val);
        }

        template <wstring_literal str>
        inline void _print(ulong val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, basic_string_literal(str, L"%zu").data, val);
        }

        template <wstring_literal str>
        inline void _print(f32 val, FILE* file_buf = stdout)
        {
            auto _res = math::abs(math::floor(val) - val);

            if((_res < 0.0001 && _res != 0) || math::abs(val) > 1.e+10)
            {
                fwprintf(file_buf, basic_string_literal(str, L"%e").data, val);
            }
            else
            {
                fwprintf(file_buf, basic_string_literal(str, L"%f").data, val);
            }
        }

        template <wstring_literal str>
        inline void _print(f64 val, FILE* file_buf = stdout)
        {
            auto _res = math::abs(math::floor(val) - val);

            if((_res < 0.0001 && _res != 0) || math::abs(val) > 1.e+10)
            {
                fwprintf(file_buf, basic_string_literal(str, L"%e").data, val);
            }
            else
            {
                fwprintf(file_buf, basic_string_literal(str, L"%lf").data, val);
            }
        }

        template <wstring_literal str>
        inline void _print(f128 val, FILE* file_buf = stdout)
        {
            auto _res = math::abs(math::floor(val) - val);

            if((_res < 0.0001 && _res != 0) || math::abs(val) > 1.e+10)
            {
                fwprintf(file_buf, basic_string_literal(str, L"%Le").data, val);
            }
            else
            {
                fwprintf(file_buf, basic_string_literal(str, L"%Lf").data, val);
            }
        }

        template <wstring_literal str>
        inline void _print(char* val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, basic_string_literal(str, L"%s").data, val);
        }

        template <wstring_literal str>
        inline void _print(char8* val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, basic_string_literal(str, L"%s").data, val);
        }

        template <wstring_literal str>
        inline void _print(wchar* val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, basic_string_literal(str, L"%ls").data, val);
        }

        template <wstring_literal str>
        inline void _print(const char* val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, basic_string_literal(str, L"%s").data, val);
        }

        template <wstring_literal str>
        inline void _print(const char8* val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, basic_string_literal(str, L"%s").data, val);
        }

        template <wstring_literal str>
        inline void _print(const wchar* val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, basic_string_literal(str, L"%ls").data, val);
        }

        template <wstring_literal str, typename... Args>
        inline void _print(const tuple<Args...>& val, FILE* file_buf = stdout)
        {
            fwprintf(file_buf, basic_string_literal(str, L"(").data);

            if constexpr(val.size() == 1)
            {
                _print<L"">(val.template get<0>(), file_buf);
            }
            else if constexpr(val.size() > 1)
            {
                [&]<usize... Ints>(index_sequence<Ints...>) {
                    ((
                        _print<L"">(val.template get<Ints>(), file_buf), 
                        _print<L", ">(file_buf)
                    ), ...);
                }(make_index_sequence<val.size() - 1>{});
                
                _print<L"">(val.template get<val.size() - 1>());
            }

            fwprintf(file_buf, L")");
        }
    }
} // namespace hsd
