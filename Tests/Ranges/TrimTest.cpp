#include <Io.hpp>
#include <String.hpp>
#include <StringView.hpp>
#include <Range.hpp>

hsd::string trim(hsd::string_view const in)
{
    hsd::string str;
    auto rev_view = in
        | hsd::views::drop_while(hsd::cstring::iswhitespace)
        | hsd::views::reverse
        | hsd::views::drop_while(hsd::cstring::iswhitespace);

    for(auto& val : rev_view | hsd::views::reverse)
        str.push_back(val);

    return str;
}
 
int main()
{
    const auto s = trim(" \f\n\t\r\vHello, C++20!\f\n\t\r\v ");
    hsd::io::print<"\"{}\"\n">(s);
 
    static constexpr hsd::stack_array v = {0, 1, 2, 3, 4, 5};
    for (int n : v | hsd::views::drop_while([](int i) { return i < 3; })) {
        hsd::io::print<"{} ">(n);
    }

    puts("");
}