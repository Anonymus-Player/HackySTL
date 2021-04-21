#pragma once

#include "_IoDetail.hpp"
#include "String.hpp"

namespace hsd
{
    class io : private io_detail::bufferable
    {
    public:
        static void flush()
        {
            fflush(stdout);
        }

        static void err_flush()
        {
            fflush(stderr);
        }

        static auto read_line()
            -> Result< reference<sstream>, runtime_error >
        {
            do
            {
                _u8io_buf.reset_data();
                char* _str = fgets(_u8io_buf.data(), 4096, stdin);

                if(_str == nullptr)
                {
                    return runtime_error{"Input requirements not satisfied"};
                }
            } while (_u8io_buf.c_str()[0] == '\n');

            return {_u8io_buf};
        }

        static sstream& read()
        {
            do
            {
                _u8io_buf.reset_data();
                scanf("%s", _u8io_buf.data());
            } while (_u8io_buf.c_str()[0] == '\n');

            return _u8io_buf;
        }

        static auto wread_line()
            -> Result< reference<wsstream>, runtime_error >
        {
            do
            {
                _wio_buf.reset_data();
                wchar* _str = fgetws(_wio_buf.data(), 4096, stdin);

                if(_str == nullptr)
                {
                    return runtime_error{"Input requirements not satisfied"};
                }
            }while(_wio_buf.c_str()[0] == '\n');

            return {_wio_buf};
        }

        static wsstream& wread()
        {
            do
            {
                _wio_buf.reset_data();
                wscanf(L"%ls", _wio_buf.data());
            }while(_wio_buf.c_str()[0] == '\n');

            return _wio_buf;
        }

        template < basic_string_literal fmt, typename... Args >
        static void print(Args&&... args)
        {
            using char_type = decltype(fmt)::char_type;
            using io_detail::_print;
            constexpr auto _fmt_buf = sstream_detail::split_literal<fmt, sizeof...(Args) + 1>().unwrap();
            static_assert(_fmt_buf.size() == sizeof...(Args) + 1, "Arguments don\'t match");

            constexpr auto _len = _fmt_buf[sizeof...(Args)].second;
            constexpr basic_string_literal<char_type, _len + 1> _last{
                _fmt_buf[sizeof...(Args)].first, _len
            };

            [&]<usize... Ints>(index_sequence<Ints...>)
            {
                (_print<basic_string_literal< char_type, _fmt_buf[Ints].second + 1 >{
                    _fmt_buf[Ints].first, _fmt_buf[Ints].second}>(args), ...);
            }(make_index_sequence<sizeof...(Args)>{});

            _print<_last>();
        }

        template < basic_string_literal fmt, typename... Args >
        static void err_print(Args&&... args)
        {
            using char_type = decltype(fmt)::char_type;
            using io_detail::_print;
            constexpr auto _fmt_buf = sstream_detail::split_literal<fmt, sizeof...(Args) + 1>().unwrap();
            static_assert(_fmt_buf.size() == sizeof...(Args) + 1, "Arguments don\'t match");

            constexpr auto _len = _fmt_buf[sizeof...(Args)].second;
            constexpr basic_string_literal<char_type, _len + 1> _last{
                _fmt_buf[sizeof...(Args)].first, _len
            };

            [&]<usize... Ints>(index_sequence<Ints...>)
            {
                (_print<basic_string_literal< char_type, _fmt_buf[Ints].second + 1 >{
                    _fmt_buf[Ints].first, _fmt_buf[Ints].second}>(args, stderr), ...);
            }(make_index_sequence<sizeof...(Args)>{});

            _print<_last>(stderr);
        }
    };

    class file : private io_detail::bufferable
    {
    private:
        FILE* _file_buf = nullptr;
        const char* _file_mode;

        bool only_write()
        {
            return (cstring::compare(_file_mode, "w") == 0
            || cstring::compare(_file_mode, "wb") == 0);
        }

        bool only_read()
        {
            return (cstring::compare(_file_mode, "r") == 0
            || cstring::compare(_file_mode, "rb") == 0);
        }

    public:
        struct options
        {
            struct text
            {
                static inline constexpr char read[] = "r";
                static inline constexpr char write[] = "w";
                static inline constexpr char append[] = "a";
                static inline constexpr char read_write[] = "r+";
                static inline constexpr char rw_create[] = "w+";
                static inline constexpr char rw_append[] = "a+";
            };

            struct binary
            {
                static inline constexpr char read[] = "rb";
                static inline constexpr char write[] = "wb";
                static inline constexpr char append[] = "ab";
                static inline constexpr char read_write[] = "r+b";
                static inline constexpr char rw_create[] = "w+b";
                static inline constexpr char rw_append[] = "a+b";
            };
        };

        file(const char* file_path, const char* open_option = hsd::file::options::text::read)
        {
            _file_buf = fopen(file_path, open_option);
            _file_mode = open_option;
            
            if(_file_buf == nullptr)
            {
                hsd_fputs_check(stderr, "File not found");
                abort();
            }
        }

        ~file()
        {
            fclose(_file_buf);
        }

        Result< void, runtime_error > flush()
        {
            if(only_read())
            {
                fflush(_file_buf);
            }
            else
            {
                return runtime_error{"Cannot flush in write or read-write mode"};
            }

            return {};
        }

        Result< reference<sstream>, runtime_error > read_line()
        {
            _u8io_buf.reset_data();
            
            if(only_write())
            {
                return runtime_error{"Cannot read file. It is in write mode"};
            }
            if(fgets(_u8io_buf.data(), 4096, _file_buf) == nullptr)
            { 
                _u8io_buf.reset_data();
            }
            
            return {_u8io_buf};
        }

        Result< reference<wsstream>, runtime_error > wread_line()
        {
            _wio_buf.reset_data();
            
            if(only_write())
            {
                return runtime_error{"Cannot read file. It is in write mode"};
            }
            if(fgetws(_wio_buf.data(), 4096, _file_buf) == nullptr)
            { 
                _wio_buf.reset_data();
            }
            
            return {_wio_buf};
        }

        Result< reference<sstream>, runtime_error > read()
        {
            _u8io_buf.reset_data();

            if(only_write())
            {
                return runtime_error{"Cannot read file. It is in write mode"};
            }
            if(fscanf(_file_buf, "%s", _u8io_buf.data()) == EOF)
            {
                _u8io_buf.reset_data();
            }
            
            return {_u8io_buf};
        }

        Result< reference<wsstream>, runtime_error > wread()
        {
            _wio_buf.reset_data();

            if(only_write())
            {
                return runtime_error{"Cannot read file. It is in write mode"};
            }
            if(fwscanf(_file_buf, L"%ls", _wio_buf.data()) == EOF)
            {
                _wio_buf.reset_data();
            }

            return {_wio_buf};
        }

        template < basic_string_literal fmt, typename... Args >
        Result< void, runtime_error > print(Args&&... args)
        {
            if(only_read())
                return runtime_error{"Cannot write file. It is in read mode"};

            using char_type = decltype(fmt)::char_type;
            using io_detail::_print;
            constexpr auto _fmt_buf = sstream_detail::split_literal<fmt, sizeof...(Args) + 1>();
            static_assert(_fmt_buf.size() == sizeof...(Args) + 1, "Arguments don\'t match");

            constexpr auto _len = _fmt_buf[sizeof...(Args)].second;
            constexpr basic_string_literal<char_type, _len + 1> _last{
                _fmt_buf[sizeof...(Args)].first, _len
            };

            [&]<usize... Ints>(index_sequence<Ints...>)
            {
                (_print<basic_string_literal< char_type, _fmt_buf[Ints].second + 1 >{
                    _fmt_buf[Ints].first, _fmt_buf[Ints].second}>(args, _file_buf), ...);
            }(make_index_sequence<sizeof...(Args)>{});

            _print<_last>(_file_buf);
            return {};
        }
    };
} // namespace hsd
