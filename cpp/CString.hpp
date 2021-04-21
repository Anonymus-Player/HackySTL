#pragma once

#include "Utility.hpp"
#include "Limits.hpp"
#include "StackArray.hpp"

namespace hsd
{	
	template <typename CharT>
    class basic_cstring
    {
    private:
        static constexpr bool _compare(const CharT* str, const CharT* substr)
        {
        	for(; *str && *substr; str++, substr++)
        	{
        		if(*str != *substr)
        			return false;
        	}
        	return (*substr == '\0');
        }

		template <typename T>
		static constexpr T _modulus(T num)
		{
			if(num < 0)
				return -num;

			return num;
		}

		template <typename T>
		static constexpr usize _num_len(T num)
		{
			usize _len = 0;
			
			if(num == 0)
			{
				return 1;
			}
			while(num)
			{
				_len++;
				num /= 10;
			}
			return _len;
		}

		static constexpr bool _is_number(CharT letter)
		{
			return letter >= '0' && letter <= '9';
		}

		static constexpr auto _get_whitespace_chars()
		{
			if constexpr (sizeof(CharT) >= sizeof(char16))
			{
				return stack_array{
					static_cast<CharT>(0x09),
					static_cast<CharT>(0x0A),
					static_cast<CharT>(0x0B),
					static_cast<CharT>(0x0C),
					static_cast<CharT>(0x0D),
					static_cast<CharT>(0x20),
					static_cast<CharT>(0x85),
					static_cast<CharT>(0xA0),
					static_cast<CharT>(0x1680),
					static_cast<CharT>(0x180E),
					static_cast<CharT>(0x2000),
					static_cast<CharT>(0x2001),
					static_cast<CharT>(0x2002),
					static_cast<CharT>(0x2003),
					static_cast<CharT>(0x2004),
					static_cast<CharT>(0x2005),
					static_cast<CharT>(0x2006),
					static_cast<CharT>(0x2007),
					static_cast<CharT>(0x2008),
					static_cast<CharT>(0x2009),
					static_cast<CharT>(0x200A),
					static_cast<CharT>(0x2028),
					static_cast<CharT>(0x2029),
					static_cast<CharT>(0x202F),
					static_cast<CharT>(0x205F),
					static_cast<CharT>(0x3000),
					static_cast<CharT>(0) // Terminator
				};
			} 
			else 
			{
				return stack_array{
					static_cast<CharT>(0x09),
					static_cast<CharT>(0x0A),
					static_cast<CharT>(0x0B),
					static_cast<CharT>(0x0C),
					static_cast<CharT>(0x0D),
					static_cast<CharT>(0x20),
					static_cast<CharT>(0x85),
					static_cast<CharT>(0xA0),
					static_cast<CharT>(0) // Terminator
				};
			}
		}
	public:
		static constexpr const auto whitespace_chars_arr = _get_whitespace_chars();
		static constexpr const CharT* whitespace_chars = whitespace_chars_arr.data();

		static constexpr bool iswhitespace(CharT ch)
		{
			return find(whitespace_chars, ch) != nullptr;
		}

        static constexpr const CharT* find(const CharT* str, const CharT* substr)
        {
        	for(; *str != '\0'; str++)
        	{
        		if ((*str == *substr) && _compare(str, substr))
        			return str;
        	}

        	return nullptr;
        }

        static constexpr const CharT* find(const CharT* str, CharT letter)
        {
        	for(; *str != '\0'; str++)
        	{
        		if(*str == letter)
        			return str;
        	}

        	return nullptr;
        }

		static constexpr const CharT* find_rev(const CharT* str, const CharT* substr)
        {
			const CharT* rez = nullptr;

        	for(; *str != '\0'; str++)
        	{
        		if ((*str == *substr) && _compare(str, substr))
        			rez = str;
        	}

        	return rez;
        }

        static constexpr const CharT* find_rev(const CharT* str, CharT letter)
        {
        	const CharT* rez = nullptr;

        	for(; *str != '\0'; str++)
        	{
        		if(*str == letter)
        			rez = str;
        	}

        	return rez;
        }

		static constexpr const CharT* find_rev(const CharT* str, const CharT* substr, usize pos)
        {
        	for(; pos != 0; pos--)
        	{
        		if ((str[pos] == *substr) && _compare(&str[pos], substr))
        			return &str[pos];
        	}

        	return nullptr;
        }

        static constexpr const CharT* find_rev(const CharT* str, CharT letter, usize pos)
        {
        	for(; pos != 0; pos--)
        	{
				auto _bdg_char = str[pos];

        		if(str[pos] == letter)
        			return &str[pos];
        	}

        	return nullptr;
        }

        static constexpr usize length(const CharT* str)
        {
        	usize _iter;

        	for(_iter = 0; str[_iter] != '\0'; _iter++);

        	return _iter;
        }

        static constexpr const CharT* upper(CharT* str)
        {
        	usize _iter;

        	for(_iter = 0; str[_iter] != '\0'; _iter++)
        	{
        		if(str[_iter] >= 'a' && str[_iter] <= 'z')
        			str[_iter] -= 32;
        	}

        	return str;
        }

        static constexpr const CharT* lower(CharT* str)
        {
        	usize _iter;

        	for(_iter = 0; str[_iter] != '\0'; _iter++)
        	{
        		if(str[_iter] >= 'a' && str[_iter] <= 'z')
        			str[_iter] += 32;
        	}

        	return str;
        }

	    static HSD_CONSTEXPR CharT* reverse(const CharT* str, usize size = 0)
	    {
	    	usize _begin = 0;
	        usize _end = size - 1;
			CharT* _buf = nullptr;

	        if(size == 0)
	        {
				_end = length(str);
				
				if(_end != 0)
					_end--;
			}
			
			_buf = new CharT[_end + 2];
			hsd::copy(str, str + _end + 1, _buf);

	    	for(; _begin <= _end; _begin++, _end--)
	    	{
	    		hsd::swap(_buf[_begin], _buf[_end]);
	    	}

	    	return _buf;
	    }

		static constexpr void reverse(CharT*& str, usize size = 0)
	    {
	    	usize _begin = 0;
	        usize _end = size - 1;

	        if(size == 0)
	        {
				_end = length(str);
				if(_end != 0){ _end--; }
			}
	    	for(; _begin <= _end; _begin++, _end--)
	    	{
	    		hsd::swap(str[_begin], str[_end]);
	    	}
	    }

		static HSD_CONSTEXPR CharT* to_string(long num)
		{
			bool _negative = (num < 0);
			usize _len = _num_len(num);
			CharT* _buf = nullptr;

			if(_negative)
			{
				_len += 1;
				_buf = new CharT[_len + 1];
				_buf[_len] = '\0';
				_buf[0] = '-';
				num = -num;
			}
			else
			{
				_buf = new CharT[_len + 1];
				_buf[_len] = '\0';
			}

			if(num == 0)
			{
				_buf[--_len] = '0';
			}

			while(num)
			{
				_buf[--_len] = '0' + (num % 10);
				num /= 10;
			}

			return _buf;
		}

		#if defined(HSD_COMPILER_GCC)
		static HSD_CONSTEXPR CharT* to_string(i128 num)
		{
			bool _negative = (num < 0);
			usize _len = _num_len(num);
			CharT* _buf = nullptr;

			if(_negative)
			{
				_len += 1;
				_buf = new CharT[_len + 1];
				_buf[_len] = '\0';
				_buf[0] = '-';
				num = -num;
			}
			else
			{
				_buf = new CharT[_len + 1];
				_buf[_len] = '\0';
			}

			if(num == 0)
			{
				_buf[--_len] = '0';
			}

			while(num)
			{
				_buf[--_len] = '0' + (num % 10);
				num /= 10;
			}

			return _buf;
		}
		#endif

		static HSD_CONSTEXPR CharT* to_string(i64 num)
		{
			bool _negative = (num < 0);
			usize _len = _num_len(num);
			CharT* _buf = nullptr;

			if(_negative)
			{
				_len += 1;
				_buf = new CharT[_len + 1];
				_buf[_len] = '\0';
				_buf[0] = '-';
				num = -num;
			}
			else
			{
				_buf = new CharT[_len + 1];
				_buf[_len] = '\0';
			}

			if(num == 0)
			{
				_buf[--_len] = '0';
			}

			while(num)
			{
				_buf[--_len] = '0' + static_cast<CharT>(num % 10);
				num /= 10;
			}

			return _buf;
		}

		static HSD_CONSTEXPR CharT* to_string(i32 num)
		{
			bool _negative = (num < 0);
			usize _len = _num_len(num);
			CharT* _buf = nullptr;

			if(_negative)
			{
				_len += 1;
				_buf = new CharT[_len + 1];
				_buf[_len] = '\0';
				_buf[0] = '-';
				num = -num;
			}
			else
			{
				_buf = new CharT[_len + 1];
				_buf[_len] = '\0';
			}

			if(num == 0)
			{
				_buf[--_len] = '0';
			}

			while(num)
			{
				_buf[--_len] = '0' + (num % 10);
				num /= 10;
			}

			return _buf;
		}

		static HSD_CONSTEXPR CharT* to_string(i16 num)
		{
			bool _negative = (num < 0);
			usize _len = _num_len(num);
			CharT* _buf = nullptr;

			if(_negative)
			{
				_len += 1;
				_buf = new CharT[_len + 1];
				_buf[_len] = '\0';
				_buf[0] = '-';
				num = -num;
			}
			else
			{
				_buf = new CharT[_len + 1];
				_buf[_len] = '\0';
			}

			if(num == 0)
			{
				_buf[--_len] = '0';
			}

			while(num)
			{
				_buf[--_len] = '0' + (num % 10);
				num /= 10;
			}

			return _buf;
		}

		static HSD_CONSTEXPR CharT* to_string(ulong num)
		{
			usize _len = _num_len(num);
			CharT* _buf = new CharT[_len + 1];
			_buf[_len] = '\0';
			_buf[0] = '0';

			for(; num != 0; num /= 10)
			{
				_buf[--_len] = '0' + (num % 10);
			}

			return _buf;
		}

		#if defined(HSD_COMPILER_GCC)
		static HSD_CONSTEXPR CharT* to_string(u128 num)
		{
			usize _len = _num_len(num);
			CharT* _buf = new CharT[_len + 1];
			_buf[_len] = '\0';
			_buf[0] = '0';

			for(; num != 0; num /= 10)
			{
				_buf[--_len] = '0' + (num % 10);
			}

			return _buf;
		}
		#endif

		static HSD_CONSTEXPR CharT* to_string(u64 num)
		{
			usize _len = _num_len(num);
			CharT* _buf = new CharT[_len + 1];
			_buf[_len] = '\0';
			_buf[0] = '0';

			for(; num != 0; num /= 10)
			{
				_buf[--_len] = '0' + static_cast<CharT>(num % 10);
			}

			return _buf;
		}

		static HSD_CONSTEXPR CharT* to_string(u32 num)
		{
			usize _len = _num_len(num);
			CharT* _buf = new CharT[_len + 1];
			_buf[_len] = '\0';
			_buf[0] = '0';

			for(; num != 0; num /= 10)
			{
				_buf[--_len] = '0' + (num % 10);
			}

			return _buf;
		}

		static HSD_CONSTEXPR CharT* to_string(u16 num)
		{
			usize _len = _num_len(num);
			CharT* _buf = new CharT[_len + 1];
			_buf[_len] = '\0';
			_buf[0] = '0';

			for(; num != 0; num /= 10)
			{
				_buf[--_len] = '0' + (num % 10);
			}

			return _buf;
		}

		static HSD_CONSTEXPR CharT* to_string(f128 num)
		{
			usize _len = 0;
			i32 _round_num = static_cast<i64>(num);
			bool _negative = (_round_num < 0);
			usize _point_num = _modulus(num - _round_num) * 1000000 + 1;
			
			if(_negative)
				_len = _num_len(_round_num) + 8;
			else
				_len = _num_len(_round_num) + 7;
			
			CharT* _buf = new CharT[_len + 1];
			_buf[_len] = '\0';

			for(; _point_num != 0; _point_num /= 10)
			{
				_buf[--_len] = '0' + (_point_num % 10);
			}
			
			_buf[--_len] = '.';
			const CharT* _round_buf = to_string(_round_num);
			hsd::move(_round_buf, _round_buf + _len, _buf);
			delete[] _round_buf;
			return _buf;
		}

		static HSD_CONSTEXPR CharT* to_string(f64 num)
		{
			usize _len = 0;
			i32 _round_num = static_cast<i64>(num);
			bool _negative = (_round_num < 0);
			usize _point_num = _modulus(num - _round_num) * 1000000 + 1;
			
			if(_negative)
				_len = _num_len(_round_num) + 8;
			else
				_len = _num_len(_round_num) + 7;
			
			CharT* _buf = new CharT[_len + 1];
			_buf[_len] = '\0';

			for(; _point_num != 0; _point_num /= 10)
			{
				_buf[--_len] = '0' + (_point_num % 10);
			}
			
			_buf[--_len] = '.';
			const CharT* _round_buf = to_string(_round_num);
			hsd::move(_round_buf, _round_buf + _len, _buf);
			delete[] _round_buf;
			return _buf;
		}

		static HSD_CONSTEXPR CharT* to_string(f32 num)
		{
			usize _len = 0;
			i64 _round_num = static_cast<i64>(num);
			bool _negative = (_round_num < 0);
			usize _point_num = static_cast<usize>(_modulus(num - static_cast<f32>(_round_num)) * 10000) + 1;
			
			if(_negative)
				_len = _num_len(_round_num) + 6;
			else
				_len = _num_len(_round_num) + 5;

			CharT* _buf = new CharT[_len + 1];
			_buf[_len] = '\0';

			for(; _point_num != 0; _point_num /= 10)
			{
				_buf[--_len] = '0' + static_cast<CharT>(_point_num % 10);
			}
			
			_buf[--_len] = '.';
			const CharT* _round_buf = to_string(_round_num);
			hsd::move(_round_buf, _round_buf + _len, _buf);
			delete[] _round_buf;
			return _buf;
		}
		
		static HSD_CONSTEXPR CharT* to_string(const CharT* str, usize len = 0)
		{
			usize _len = len;
			
			if(len == 0)
				_len = length(str);

			CharT* _buf = new CharT[_len + 1];

			for (usize _index = 0; _index <= _len; _index++)
				_buf[_index] = static_cast<CharT>(str[_index]);

			return _buf;
		}

		static HSD_CONSTEXPR CharT* to_string(CharT letter)
		{
			CharT* _buf = new CharT[2];
			_buf[0] = static_cast<CharT>(letter);
			_buf[1] = '\0';
			return _buf;
		}

		static constexpr isize parse_i(const CharT* str)
		{
			isize _num = 0;
			bool _negative = false;

			for(; *str != '\0' && !_is_number(*str); str++);
			_negative = *(str - 1) == '-';
			
			for(; *str != '\0' && _is_number(*str); str++)
			{
				_num *= 10;
				_num += *str - '0';
			}
			
			return _negative ? -_num : _num;
		}

		static constexpr usize parse_us(const CharT* str)
		{
			usize _num = 0;

			for(; *str != '\0' && !_is_number(*str); str++);
			for(; *str != '\0' && _is_number(*str); str++)
			{
				_num *= 10;
				_num += *str - '0';
			}

			return _num;
		}

		static constexpr f64 parse_f(const CharT* str)
		{
		    f64 _round_num = 0;
			f64 _point_num = 0;
			bool _negative = false;
			usize _num_len = 0;

			for(; *str != '\0' && !_is_number(*str); str++)
			{
				_negative = (*str == '-');
			}

			if(*str == '\0')
				return 0.f;

		    for(; *str != '\0' && *str != '.' && _is_number(*str); str++)
		    {
		        _round_num *= 10;
		        _round_num += *str - '0';
		    }

			if(*str == '\0' || !_is_number(*str))
				return _negative ? -_round_num : _round_num;

		    for(str += 1; *str != '\0' && _is_number(*str); str++, _num_len++)
		    {
		        _point_num *= 10;
		        _point_num += *str - '0';
		    }
		    for(; _num_len != 0; _num_len--, _point_num *= 0.1f) {}
			
			return _negative ? -(_round_num + _point_num) : _round_num + _point_num;
		}

		static constexpr i32 compare(const CharT* lhs, const CharT* rhs)
		{
			usize _index = 0;

			for(; lhs[_index] && rhs[_index]; _index++)
			{
				if(lhs[_index] < rhs[_index])
					return -1;
				else if(lhs[_index] > rhs[_index])
					return 1;
			}
			
			if(lhs[_index] < rhs[_index])
				return -1;
			else if(lhs[_index] > rhs[_index])
				return 1;
			else
				return 0;
		}

		static constexpr i32 compare(const CharT* lhs, const CharT* rhs, usize len)
		{
			usize _index = 0;

			for(; _index < len && lhs[_index] && rhs[_index]; _index++)
			{
				if(lhs[_index] < rhs[_index])
					return -1;
				else if(lhs[_index] > rhs[_index])
					return 1;
			}
			
			if(lhs[_index] < rhs[_index])
				return -1;
			else if(lhs[_index] > rhs[_index])
				return 1;
			else
				return 0;
		}

		static constexpr CharT* copy(CharT* dest, const CharT* src, usize len)
		{
			if (dest == nullptr)
				return nullptr;

			for(; *src && len != 0; len--, dest++, src++)
			{
				*dest = *src;
			}
			return dest;
		}

		static constexpr CharT* copy(CharT* dest, const CharT* src)
		{
			if (dest == nullptr)
				return nullptr;

			char* ptr = dest;

			for(; *src; dest++, src++)
				*dest = *src;
			
			return ptr;
		}

		static constexpr CharT* copy_until(CharT* dest, const CharT* src, CharT letter)
		{
			if (dest == nullptr)
				return nullptr;

			char* ptr = dest;

			for(; *src != '\0' && *src != letter; dest++, src++)
				*dest = *src;
			
			*dest = '\0';
			return ptr;
		}

		static constexpr CharT* add(CharT* dest, const CharT* src)
		{
		    usize _index = length(dest);
			usize _index_helper = 0;

		    for (; src[_index_helper] != '\0'; _index_helper++)
		    {
				dest[_index + _index_helper] = src[_index_helper];
			}

			dest[_index + _index_helper] = '\0';
		    return dest;
		}

		static constexpr CharT* add(CharT* dest, const CharT* src, usize len)
		{
			usize _index = len;
			usize _index_helper = 0;

		    for (; src[_index_helper] != '\0'; _index_helper++)
		    {
				dest[_index + _index_helper] = src[_index_helper];
			}

			dest[_index + _index_helper] = '\0';
		    return dest;
		}
    };

	using cstring = basic_cstring<char>;
	using wcstring = basic_cstring<wchar>;
	using u8cstring = basic_cstring<char8>;
	using u16cstring = basic_cstring<char16>;
	using u32cstring = basic_cstring<char32>;
}