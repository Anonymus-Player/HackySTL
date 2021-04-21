# String manipulation support

## String manipulation introduced in the library
Implemented in header [CString.hpp](../cpp/CString.hpp)

## CString
### Definition:
```cpp
template <typename CharType>
class basic_cstring;
```

### Description:
Reimplementation of C's `string.h` header for string manipulation using C++20 features

### Aliases
| Alias | Type |
| :---- | :--- |
| `cstring` | `basic_cstring<char>` |
| `wcstring` | `basic_cstring<wchar>` |
| `u8cstring` | `basic_cstring<char8>` |
| `u16cstring` | `basic_cstring<char16>` |
| `u32cstring` | `basic_cstring<char32>` |

### Public members:
| Member | Value |
| :----- | :---- |
| `whitespace_chars_arr` | `/*implementation defined*/` |
| `whitespace_chars` | `/*implementation defined*/` |

### Member functions:
| Function | Arguments | Return type | Description |
| :------- | :-------- | :---------- | :---------- |
| `iswhitespace` | `CharType ch` | `bool` | Checks if `ch` is a whitespace character by the unicode standards |
| `find` | `const CharType* str`, `const CharType* substr` | `const CharType*` | Searches through `str` for the first `substr` occurrences |
| `find` | `const CharType* str`, `CharType* letter` | `const CharType*` | Searches through `str` for the first `letter` occurrences |
| `find_rev` | `const CharType* str`, `const CharType* substr` | `const CharType*` | Searches through `str` for the first `substr` occurrences in reverse |
| `find_rev` | `const CharType* str`, `CharType* letter` | `const CharType*` | Searches through `str` for the first `letter` occurrences in reverse |
| `find_rev` | `const CharType* str`, `const CharType* substr`, `usize pos` | `const CharType*` | Searches through `str` for the first `substr` occurrences in reverse from a given position |
| `find_rev` | `const CharType* str`, `CharType letter`, `usize pos` | `const CharType*` | Searches through `str` for the first `letter` occurrences in reverse from a give position |
| `length` | `const CharType* str` | `usize` | Measures the length of the string from 0 to where the null terminator is |
| `upper` | `CharType* str` | `const CharType*` | Transforms the lower ascii characters (a-z) to the upper ones (A-Z) |
| `lower` | `CharType* str` | `const CharType*` | Transforms the upper ascii characters (A-Z) to the lower ones (a-z) |
| `reverse` | `const CharType* str`, `usize size = 0` | `CharType*` | Allocates a new string in the memory and copies the characters from `str` to the new buffer in reverse |
| `reverse` | `CharType*& str`, `usize size = 0` | `void` | Switches the characters inside `str` in a claw manner |
| `to_string` | `long/i128(gcc only)/i64/i32/i16/ulong/u128(gcc only)/u64/u32/u16/f128/f64/f32 num` | `CharType*` | Transforms a number into a string |
| `to_string` | `const CharType* str`, `usize len = 0` | `CharType*` | Copies a string into a newly created one |
| `to_string` | `CharType letter` | `CharType*` | Transforms a letter into a string |
| `parse_i` | `const CharType* str` | `isize` | Transforms a string into a integral number |
| `parse_us` | `const CharType* str` | `usize` | Transforms a string into a natural number |
| `parse_f` | `const CharType* str` | `f64` | Transforms a string into a real number |
| `compare` | `const CharType* lhs`, `const CharType* rhs` | `i32` | Compares to strings, if they the first one is grater than the second one it return `1`, if the second one is greater it returns `-1` otherwise it return `0` |
| `compare` | `const CharType* lhs`, `const CharType* rhs`, `usize len` | `i32` | Compares to strings in the range of `0 -> len`, if they the first one is grater than the second one it return `1`, if the second one is greater it returns `-1` otherwise it return `0` |
| `copy` | `CharType* dest`, `const CharType* src` | `CharType*` | Copies the contents of one string into the other |
| `copy` | `CharType* dest`, `const CharType* src`, `usize len` | `CharType*` | Copies the contents of one string into the other in the range of `0 -> len` |
| `add` | `CharType* dest`, `const CharType* src` | `CharType*` | Adds the contents of one string into the other |
| `add` | `CharType* dest`, `const CharType* src`, `usize len` | `CharType*` | Adds the contents of one string into the other in the range of `0 -> len` |

### Example:
```cpp
#include <CString.hpp>
#include <Io.hpp>

template <typename CharType>
struct PseudoString
{
    CharType* _cstr;
    hsd::usize _size;

    constexpr PseudoString(hsd::usize len)
        : _size{_len}
    {
        _cstr = new CharType[_size + 1];
    }

    constexpr PseudoString(const CharType* cstr)
    {
        _size = hsd::basic_cstring<CharType>::length(cstr);
        _cstr = new CharType[_size + 1];
        hsd::copy(cstr, cstr + _size + 1, _cstr);
    }

    constexpr ~PseudoString()
    {
        delete[] _cstr;
    }
};

int main()
{
    PseudoString test = "Test string";
    hsd::io::print<"Finding \'r\' in \"{}\": {}\n">(test._cstr, hsd::cstring::find(test._cstr, 'r'));
    hsd::io::print<"Finding \"str\" in \"{}\": {}\n">(test._cstr, hsd::cstring::find(test._cstr, "str"));
    hsd::io::print<"Finding \'r\' in reverse in \"{}\": \"{}\"\n">(test._cstr, hsd::cstring::find_rev(test._cstr, 'r'));
    hsd::io::print<"Finding \"str\"in reverse in \"{}\": \"{}\"\n">(test._cstr, hsd::cstring::find_rev(test._cstr, "str"));
    hsd::io::print<"Lowering the string: \"{}\"\n">(hsd::cstring::lower(test._cstr));
    hsd::io::print<"Uprearing the string: \"{}\"\n">(hsd::cstring::upper(test._cstr));
    hsd::io::print<"Reversing the string: \"{}\"\n">(hsd::cstring::reverse(test._cstr, _test._length + 1));
}
```