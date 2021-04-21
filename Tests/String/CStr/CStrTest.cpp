#include <CString.hpp>
#include <stdio.h>

template<typename CharT>
struct PseudoString
{
    CharT* _cstr;
    hsd::usize _size;

    constexpr PseudoString(const CharT* cstr)
    {
        _size = hsd::basic_cstring<CharT>::length(cstr);
        _cstr = new CharT[_size + 1];
        hsd::copy(cstr, cstr + _size + 1, _cstr);
    }

    constexpr ~PseudoString()
    {
        delete[] _cstr;
    }
};

int main()
{
    PseudoString test = "ana";
    hsd::cstring::reverse(test._cstr, test._size);
    printf("%s\n", test._cstr);
}