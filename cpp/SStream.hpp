#pragma once

#include "_SStreamDetail.hpp"

namespace hsd
{
    
    template <typename CharT>
    class basic_sstream
    {
    private:
        CharT* _data = nullptr;
        usize _capacity = 0;
		usize _size = 0;

    public:
		using iterator = CharT*;
        using const_iterator = const CharT*;
        basic_sstream(const basic_sstream&) = delete;
        basic_sstream& operator=(const basic_sstream& other) = delete;

        basic_sstream(usize size)
        {
            _data = new CharT[size + 1];
            _data[size] = '\0';
            _capacity = size;
        }

        ~basic_sstream()
        {
            delete[] _data;
        }

        void add_raw_data(const CharT* raw_data)
        {
            _size += static_cast<usize>(
                sstream_detail::_write<"">(raw_data, {_data + _size, _capacity - _size})
            );
            _data[_size++] = static_cast<CharT>(' ');
            _data[_size] = static_cast<CharT>('\0');
        }

		template <typename... Args>
		Result<void, runtime_error> set_data(Args&... args)
		{
            using sstream_detail::_parse;
            auto _data_set = sstream_detail::split_data<sizeof...(Args)>(_data, _capacity);

            if(sizeof...(Args) > _data_set.size())
            {
                return runtime_error{"Input too small to parse"};
            }
            else
            {
                [&]<usize... Ints>(index_sequence<Ints...>)
                {
                    (_parse(_data_set[Ints], args), ...);
                }(make_index_sequence<sizeof...(Args)>{});
            }

            return {};
		}

        template <typename T>
        T parse()
        {
            T _value{};
            using sstream_detail::_parse;
            auto _str = pair{c_str(), size()};
            _parse(_str, _value);
            return _value;
        }

        template < basic_string_literal fmt, typename... Args >
		void write_data(Args&&... args)
		{
            using char_type = typename decltype(fmt)::char_type;
            using sstream_detail::_write;
            _size = _capacity;
            
            constexpr auto _fmt_buf = sstream_detail::split_literal<fmt, sizeof...(Args) + 1>().unwrap();
            static_assert(_fmt_buf.size() == sizeof...(Args) + 1, "Arguments don\'t match");

            constexpr auto _fmt_last_len = _fmt_buf[sizeof...(Args)].second;
            constexpr basic_string_literal<char_type, _fmt_last_len + 1> _last{
                _fmt_buf[sizeof...(Args)].first, _fmt_last_len
            };

            [&]<usize... Ints>(index_sequence<Ints...>) {
                (
                    (sstream_detail::_sub_from(_size, _write<
                    basic_string_literal< char_type, _fmt_buf[Ints].second + 1 >{
                        _fmt_buf[Ints].first, _fmt_buf[Ints].second
                    }>(args, {_data + (_capacity - _size), _size})).unwrap(), ...)
                );
            }(make_index_sequence<sizeof...(Args)>{});

            usize _last_len = static_cast<usize>(_write<_last>({_data + (_capacity - _size), _size}));
            _size = _capacity - _size + _last_len;
		}

		void pop_back()
        {
            _data[--_size] = '\0';
        }

        void clear()
        {
            if(_data != nullptr)
                delete[] _data;
    
            _data = new CharT[1];
            _data[0] = '\0';
            _capacity = 0;
        }

        void reset_data()
        {
            _data[0] = '\0';
            _size = 0;
        }

        usize capacity() const
        {
            return _capacity;
        }

        usize size() const
        {
            return _size;
        }

        iterator data()
        {
            return _data;
        }

        const_iterator c_str() const
        {
            return _data;
        }
    
        iterator begin()
        {
            return data();
        }

        iterator begin() const
        {
            return cbegin();
        }

        iterator end()
        {
            return begin() + size();
        }

        const_iterator end() const
        {
            return cend();
        }

        const_iterator cbegin() const
        {
            return c_str();
        }

        const_iterator cend() const
        {
            return cbegin() + size();
        }
    };
    
    using sstream = basic_sstream<char>;
	using wsstream = basic_sstream<wchar>;
    using u8sstream = basic_sstream<char8>;
    using u16sstream = basic_sstream<char16>;
    using u32sstream = basic_sstream<char32>;
} // namespace hsd
