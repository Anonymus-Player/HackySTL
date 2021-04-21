#pragma once

#include "Result.hpp"
#include "_IoOverload.hpp"
#include "StackArray.hpp"
#include "Vector.hpp"
#include "CString.hpp"

namespace hsd
{
    namespace sstream_detail
    {
        template <usize N, typename CharT>
        inline auto split_data(const CharT* str, usize size)
        {
            static_vector< pair<const CharT*, usize>, N > _buf;
            const CharT* _iter_f = str;
            const CharT* _iter_s = basic_cstring<CharT>::find(_iter_f, ' ');

            while (_iter_s != nullptr && *_iter_s != '\0')
            {
                _buf.emplace_back(_iter_f, static_cast<usize>(_iter_s - _iter_f));
                _iter_f = _iter_s + 1;
                _iter_s = basic_cstring<CharT>::find(_iter_f, ' ');
            }
            
            _buf.emplace_back(_iter_f, static_cast<usize>((str + size) - _iter_f));
            return _buf;
        }

        template <basic_string_literal fmt, usize N>
        static constexpr auto split_literal()
            -> Result< 
                static_vector< pair<
                    const typename decltype(fmt)::char_type*, usize>, N > 
                , runtime_error >
        {
            using char_type = typename decltype(fmt)::char_type;
            using buf_type = static_vector< pair<const char_type*, usize>, N >;

            buf_type _buf;
            const char_type* _iter_f = fmt.data;
            const char_type* _iter_s = basic_cstring<char_type>::find(_iter_f, '{');

            if(_iter_s != nullptr && *(_iter_s + 1) != '}')
            {
                return runtime_error{"invalid character after \'{\'"};
            }
            while (_iter_s != nullptr && *_iter_s != '\0')
            {
                _buf.emplace_back(_iter_f, static_cast<usize>(_iter_s - _iter_f));
                _iter_f = _iter_s + 2;
                _iter_s = basic_cstring<char_type>::find(_iter_f, '{');

                if(_iter_s != nullptr && *(_iter_s + 1) != '}')
                    return runtime_error{"invalid character after \'{\'"};
            }

            _buf.emplace_back(_iter_f, static_cast<usize>(fmt.data + fmt.size() - _iter_f));
            return _buf;
        }
    
        template <>
        inline void _parse<char>(pair<const char*, usize>& str, char& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            sscanf_s(str.first, "%c", &val);
            #else
            sscanf(str.first, "%c", &val);
            #endif
        }

        template <>
        inline void _parse<char>(pair<const char*, usize>& str, uchar& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            sscanf_s(str.first, "%c", &val);
            #else
            sscanf(str.first, "%c", &val);
            #endif
        }

        template <>
        inline void _parse<char>(pair<const char*, usize>& str, i16& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            sscanf_s(str.first, "%hd", &val);
            #else
            sscanf(str.first, "%hd", &val);
            #endif
        }

        template <>
        inline void _parse<char>(pair<const char*, usize>& str, u16& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            sscanf_s(str.first, "%hu", &val);
            #else
            sscanf(str.first, "%hu", &val);
            #endif
        }

        template <>
        inline void _parse<char>(pair<const char*, usize>& str, i32& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            sscanf_s(str.first, "%d", &val);
            #else
            sscanf(str.first, "%d", &val);
            #endif
        }

        template <>
        inline void _parse<char>(pair<const char*, usize>& str, u32& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            sscanf_s(str.first, "%u", &val);
            #else
            sscanf(str.first, "%u", &val);
            #endif
        }

        template <>
        inline void _parse<char>(pair<const char*, usize>& str, long& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            sscanf_s(str.first, "%zd", &val);
            #else
            sscanf(str.first, "%zd", &val);
            #endif
        }

        template <>
        inline void _parse<char>(pair<const char*, usize>& str, ulong& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            sscanf_s(str.first, "%zu", &val);
            #else
            sscanf(str.first, "%zu", &val);
            #endif
        }

        template <>
        inline void _parse<char>(pair<const char*, usize>& str, i64& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            sscanf_s(str.first, "%lld", &val);
            #else
            sscanf(str.first, "%lld", &val);
            #endif
        }

        template <>
        inline void _parse<char>(pair<const char*, usize>& str, u64& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            sscanf_s(str.first, "%llu", &val);
            #else
            sscanf(str.first, "%llu", &val);
            #endif
        }

        template <>
        inline void _parse<char>(pair<const char*, usize>& str, f32& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            sscanf_s(str.first, "%f", &val);
            #else
            sscanf(str.first, "%f", &val);
            #endif
        }

        template <>
        inline void _parse<char>(pair<const char*, usize>& str, f64& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            sscanf_s(str.first, "%lf", &val);
            #else
            sscanf(str.first, "%lf", &val);
            #endif
        }

        template <>
        inline void _parse<char>(pair<const char*, usize>& str, f128& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            sscanf_s(str.first, "%Lf", &val);
            #else
            sscanf(str.first, "%Lf", &val);
            #endif
        }

        template <>
        inline void _parse<wchar>(pair<const wchar*, usize>& str, char& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            swscanf_s(str.first, L"%c", &val);
            #else
            swscanf(str.first, L"%c", &val);
            #endif
        }

        template <>
        inline void _parse<wchar>(pair<const wchar*, usize>& str, uchar& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            swscanf_s(str.first, L"%c", &val);
            #else
            swscanf(str.first, L"%c", &val);
            #endif
        }

        template <>
        inline void _parse<wchar>(pair<const wchar*, usize>& str, wchar& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            swscanf_s(str.first, L"%lc", &val);
            #else
            swscanf(str.first, L"%lc", &val);
            #endif
        }

        template <>
        inline void _parse<wchar>(pair<const wchar*, usize>& str, i16& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            swscanf_s(str.first, L"%hd", &val);
            #else
            swscanf(str.first, L"%hd", &val);
            #endif
        }

        template <>
        inline void _parse<wchar>(pair<const wchar*, usize>& str, u16& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            swscanf_s(str.first, L"%hu", &val);
            #else
            swscanf(str.first, L"%hu", &val);
            #endif
        }

        template <>
        inline void _parse<wchar>(pair<const wchar*, usize>& str, i32& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            swscanf_s(str.first, L"%d", &val);
            #else
            swscanf(str.first, L"%d", &val);
            #endif
        }

        template <>
        inline void _parse<wchar>(pair<const wchar*, usize>& str, u32& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            swscanf_s(str.first, L"%d", &val);
            #else
            swscanf(str.first, L"%d", &val);
            #endif
        }

        template <>
        inline void _parse<wchar>(pair<const wchar*, usize>& str, long& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            swscanf_s(str.first, L"%zd", &val);
            #else
            swscanf(str.first, L"%zd", &val);
            #endif
        }

        template <>
        inline void _parse<wchar>(pair<const wchar*, usize>& str, ulong& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            swscanf_s(str.first, L"%zu", &val);
            #else
            swscanf(str.first, L"%zu", &val);
            #endif
        }

        template <>
        inline void _parse<wchar>(pair<const wchar*, usize>& str, i64& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            swscanf_s(str.first, L"%lld", &val);
            #else
            swscanf(str.first, L"%lld", &val);
            #endif
        }

        template <>
        inline void _parse<wchar>(pair<const wchar*, usize>& str, u64& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            swscanf_s(str.first, L"%llu", &val);
            #else
            swscanf(str.first, L"%llu", &val);
            #endif
        }

        template <>
        inline void _parse<wchar>(pair<const wchar*, usize>& str, f32& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            swscanf_s(str.first, L"%f", &val);
            #else
            swscanf(str.first, L"%f", &val);
            #endif
        }

        template <>
        inline void _parse<wchar>(pair<const wchar*, usize>& str, f64& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            swscanf_s(str.first, L"%lf", &val);
            #else
            swscanf(str.first, L"%lf", &val);
            #endif
        }

        template <>
        inline void _parse<wchar>(pair<const wchar*, usize>& str, f128& val)
        {
            #if defined(HSD_PLATFORM_WINDOWS)
            swscanf_s(str.first, L"%Lf", &val);
            #else
            swscanf(str.first, L"%Lf", &val);
            #endif
        }

        inline auto _sub_from(i32& from, i32 amount)
            -> Result< void, runtime_error >
        {
            if(amount < 0)
            {
                return runtime_error{"Buffer too small"};
            }
            else
            {
                from -= amount;
            }

            return {};
        }

        inline auto _sub_from(usize& from, i32 amount)
            -> Result< void, runtime_error >
        {
            if(amount < 0)
            {
                return runtime_error{"Buffer too small"};
            }
            else
            {
                from -= static_cast<usize>(amount);
            }

            return {};
        }

        template <string_literal str>
        inline i32 _write(pair<char*, usize> dest)
        {
            return snprintf(dest.first, dest.second, str.data);
        }

        template <string_literal str>
        inline i32 _write(char val, pair<char*, usize> dest)
        {
            return snprintf(dest.first, dest.second, basic_string_literal(str, "%c").data, val);
        }

        template <string_literal str>
        inline i32 _write(char8 val, pair<char*, usize> dest)
        {
            return snprintf(dest.first, dest.second, basic_string_literal(str, "%zc").data, val);
        }

        template <string_literal str>
        inline i32 _write(i16 val, pair<char*, usize> dest)
        {
            return snprintf(dest.first, dest.second, basic_string_literal(str, "%hd").data, val);
        }

        template <string_literal str>
        inline i32 _write(u16 val, pair<char*, usize> dest)
        {
            return snprintf(dest.first, dest.second, basic_string_literal(str, "%hu").data, val);
        }

        template <string_literal str>
        inline i32 _write(i32 val, pair<char*, usize> dest)
        {
            return snprintf(dest.first, dest.second, basic_string_literal(str, "%d").data, val);
        }

        template <string_literal str>
        inline i32 _write(u32 val, pair<char*, usize> dest)
        {
            return snprintf(dest.first, dest.second, basic_string_literal(str, "%u").data, val);
        }

        template <string_literal str>
        inline i32 _write(i64 val, pair<char*, usize> dest)
        {
            return snprintf(dest.first, dest.second, basic_string_literal(str, "%lld").data, val);
        }

        template <string_literal str>
        inline i32 _write(u64 val, pair<char*, usize> dest)
        {
            return snprintf(dest.first, dest.second, basic_string_literal(str, "%llu").data, val);
        }

        template <string_literal str>
        inline i32 _write(long val, pair<char*, usize> dest)
        {
            return snprintf(dest.first, dest.second, basic_string_literal(str, "%zd").data, val);
        }

        template <string_literal str>
        inline i32 _write(ulong val, pair<char*, usize> dest)
        {
            return snprintf(dest.first, dest.second, basic_string_literal(str, "%zu").data, val);
        }

        template <string_literal str>
        inline i32 _write(f32 val, pair<char*, usize> dest)
        {
            auto _res = math::abs(math::floor(val) - val);

            if((_res < 0.0001 && _res != 0) || math::abs(val) > 1.e+10)
            {
                return snprintf(dest.first, dest.second, basic_string_literal(str, "%e").data, val);
            }
            else
            {
                return snprintf(dest.first, dest.second, basic_string_literal(str, "%f").data, val);
            }
        }

        template <string_literal str>
        inline i32 _write(f64 val, pair<char*, usize> dest)
        {
            auto _res = math::abs(math::floor(val) - val);

            if((_res < 0.0001 && _res != 0) || math::abs(val) > 1.e+10)
            {
                return snprintf(dest.first, dest.second, basic_string_literal(str, "%e").data, val);
            }
            else
            {
                return snprintf(dest.first, dest.second, basic_string_literal(str, "%lf").data, val);
            }
        }

        template <string_literal str>
        inline i32 _write(f128 val, pair<char*, usize> dest)
        {
            auto _res = math::abs(math::floor(val) - val);

            if((_res < 0.0001 && _res != 0) || math::abs(val) > 1.e+10)
            {
                return snprintf(dest.first, dest.second, basic_string_literal(str, "%Le").data, val);
            }
            else
            {
                return snprintf(dest.first, dest.second, basic_string_literal(str, "%Lf").data, val);
            }
        }

        template <string_literal str>
        inline i32 _write(char* val, pair<char*, usize> dest)
        {
            return snprintf(dest.first, dest.second, basic_string_literal(str, "%s").data, val);
        }

        template <string_literal str>
        inline i32 _write(const char* val, pair<char*, usize> dest)
        {
            return snprintf(dest.first, dest.second, basic_string_literal(str, "%s").data, val);
        }

        template <string_literal str>
        inline i32 _write(const char8* val, pair<char*, usize> dest)
        {
            return snprintf(dest.first, dest.second, basic_string_literal(str, "%s").data, val);
        }
        
        template <string_literal str, typename... Args>
        inline i32 _write(const tuple<Args...>& val, pair<char*, usize> dest)
        {
            i32 _len = dest.second;

            [&]<usize... Ints>(index_sequence<Ints...>) {
                (
                    (_sub_from(_len, _write<"">(val.template get<Ints>(), 
                    {dest.first + (dest.second - _len), _len}))), ...
                );
            }(make_index_sequence<val.size()>{});

            return dest.second - _len;
        }

        template <wstring_literal str>
        inline i32 _write(pair<wchar*, usize> dest)
        {
            return swprintf(dest.first, dest.second, str.data);
        }

        template <wstring_literal str>
        inline i32 _write(wchar val, pair<wchar*, usize> dest)
        {
            return swprintf(dest.first, dest.second, basic_string_literal(str, L"%lc").data, val);
        }

        template <wstring_literal str>
        inline i32 _write(char val, pair<wchar*, usize> dest)
        {
            return swprintf(dest.first, dest.second, basic_string_literal(str, L"%c").data, val);
        }

        template <wstring_literal str>
        inline i32 _write(char8 val, pair<wchar*, usize> dest)
        {
            return swprintf(dest.first, dest.second, basic_string_literal(str, L"%zc").data, val);
        }

        template <wstring_literal str>
        inline i32 _write(i16 val, pair<wchar*, usize> dest)
        {
            return swprintf(dest.first, dest.second, basic_string_literal(str, L"%hd").data, val);
        }

        template <wstring_literal str>
        inline i32 _write(u16 val, pair<wchar*, usize> dest)
        {
            return swprintf(dest.first, dest.second, basic_string_literal(str, L"%hu").data, val);
        }

        template <wstring_literal str>
        inline i32 _write(i32 val, pair<wchar*, usize> dest)
        {
            return swprintf(dest.first, dest.second, basic_string_literal(str, L"%d").data, val);
        }

        template <wstring_literal str>
        inline i32 _write(u32 val, pair<wchar*, usize> dest)
        {
            return swprintf(dest.first, dest.second, basic_string_literal(str, L"%u").data, val);
        }

        template <wstring_literal str>
        inline i32 _write(i64 val, pair<wchar*, usize> dest)
        {
            return swprintf(dest.first, dest.second, basic_string_literal(str, L"%lld").data, val);
        }

        template <wstring_literal str>
        inline i32 _write(u64 val, pair<wchar*, usize> dest)
        {
            return swprintf(dest.first, dest.second, basic_string_literal(str, L"%llu").data, val);
        }

        template <wstring_literal str>
        inline i32 _write(long val, pair<wchar*, usize> dest)
        {
            return swprintf(dest.first, dest.second, basic_string_literal(str, L"%zd").data, val);
        }

        template <wstring_literal str>
        inline i32 _write(ulong val, pair<wchar*, usize> dest)
        {
            return swprintf(dest.first, dest.second, basic_string_literal(str, L"%zu").data, val);
        }

        template <wstring_literal str>
        inline i32 _write(f32 val, pair<wchar*, usize> dest)
        {
            auto _res = math::abs(math::floor(val) - val);

            if((_res < 0.0001 && _res != 0) || math::abs(val) > 1.e+10)
            {
                return swprintf(dest.first, dest.second, basic_string_literal(str, L"%e").data, val);
            }
            else
            {
                swprintf(dest.first, dest.second, basic_string_literal(str, L"%f").data, val);
            }
        }

        template <wstring_literal str>
        inline i32 _write(f64 val, pair<wchar*, usize> dest)
        {
            auto _res = math::abs(math::floor(val) - val);

            if((_res < 0.0001 && _res != 0) || math::abs(val) > 1.e+10)
            {
                return swprintf(dest.first, dest.second, basic_string_literal(str, L"%e").data, val);
            }
            else
            {
                return swprintf(dest.first, dest.second, basic_string_literal(str, L"%lf").data, val);
            }
        }

        template <wstring_literal str>
        inline i32 _write(f128 val, pair<wchar*, usize> dest)
        {
            auto _res = math::abs(math::floor(val) - val);

            if((_res < 0.0001 && _res != 0) || math::abs(val) > 1.e+10)
            {
                return swprintf(dest.first, dest.second, basic_string_literal(str, L"%Le").data, val);
            }
            else
            {
                return swprintf(dest.first, dest.second, basic_string_literal(str, L"%Lf").data, val);
            }
        }

        template <wstring_literal str>
        inline i32 _write(char* val, pair<wchar*, usize> dest)
        {
            return swprintf(dest.first, dest.second, basic_string_literal(str, L"%s").data, val);
        }

        template <wstring_literal str>
        inline i32 _write(const char* val, pair<wchar*, usize> dest)
        {
            return swprintf(dest.first, dest.second, basic_string_literal(str, L"%s").data, val);
        }

        template <wstring_literal str>
        inline i32 _write(char8* val, pair<wchar*, usize> dest)
        {
            return swprintf(dest.first, dest.second, basic_string_literal(str, L"%s").data, val);
        }

        template <wstring_literal str>
        inline i32 _write(const char8* val, pair<wchar*, usize> dest)
        {
            return swprintf(dest.first, dest.second, basic_string_literal(str, L"%s").data, val);
        }

        template <wstring_literal str>
        inline i32 _write(wchar* val, pair<wchar*, usize> dest)
        {
            return swprintf(dest.first, dest.second, basic_string_literal(str, L"%ls").data, val);
        }

        template <wstring_literal str>
        inline i32 _write(const wchar* val, pair<wchar*, usize> dest)
        {
            return swprintf(dest.first, dest.second, basic_string_literal(str, L"%ls").data, val);
        }

        template <wstring_literal str, typename... Args>
        inline i32 _write(const tuple<Args...>& val, pair<wchar*, usize> dest)
        {
            i32 _len = dest.second;

            [&]<usize... Ints>(index_sequence<Ints...>) {
                (
                    (_sub_from(_len, _write<L" ">(val.template get<Ints>(), 
                    {dest.first + (dest.second - _len), _len}))), ...
                );
            }(make_index_sequence<val.size()>{});

            return dest.second - _len;
        }
    } // namespace sstream_detail
} // namespace hsd
