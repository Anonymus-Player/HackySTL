#include <Io.hpp>

// custom struct for printing
struct test 
{
    hsd::i32 a = 1, b = 0;
    char c[30] = "sample text";
    hsd::f64 d = 4.2;

    test() = default;

    test(hsd::i32 first, hsd::i32 second, const char* third, hsd::f64 forth)
        : a{first}, b{second}, d{forth}
    {
        hsd::cstring::copy(c, third);
    }
};

template <hsd::string_literal str>
inline void _print(test& t, FILE* ptr = stdout)
{
    if(ptr == stdout)
    {
        hsd::io_detail::_print<str>();
        hsd::io::print<"Struct Test contains:\n[\n    a = {}\n    b = {}\n    c = {}\n    d = {}\n]">(t.a, t.b, t.c, t.d);
    }
    else if(ptr == stderr)
    {
        hsd::io_detail::_print<str>(stderr);
        hsd::io::err_print<"Struct Test contains:\n[\n    a = {}\n    b = {}\n    c = {}\n    d = {}\n]">(t.a, t.b, t.c, t.d);
    }
}

template <hsd::wstring_literal str>
inline void _print(test& t, FILE* ptr = stdout)
{
    if(ptr == stdout)
    {
        // Optional: You can use hsd::io::print<L"{}">(str) as well
        hsd::io_detail::_print<str>();
        hsd::io::print<L"Struct Test contains:\n[\n    a = {}\n    b = {}\n    c = {}\n    d = {}\n]">(t.a, t.b, t.c, t.d);
    }
    else if(ptr == stderr)
    {
        hsd::io_detail::_print<str>(stderr);
        hsd::io::err_print<L"Struct Test contains:\n[\n    a = {}\n    b = {}\n    c = {}\n    d = {}\n]">(t.a, t.b, t.c, t.d);
    }
}

inline void _parse(hsd::pair<const char*, hsd::usize>& str, test& t)
{
    sscanf(str.first, "%d%d%lf", &t.a, &t.b, &t.d);
}

int main()
{
    hsd::wsstream str{256};
    hsd::i32 x, z;
    hsd::f32 y;
    hsd::io::print<L"hello, {} and other words\n">(123.2);
    str.write_data<L"hello, {} and other words\n">(hsd::string("123.2"));
    
    hsd::io::err_print<L"{}\n">(test{});
    hsd::io::print<L"{}\n">(test{21, 1, "how is this possible", 69.42342});
    auto t = hsd::io::read_line().unwrap().parse<test>();
    
    hsd::io::read_line().unwrap().set_data(x, y, z).unwrap();
    hsd::io::print<L"{}, {}, {}\n">(x, y, z);
    
    auto file = hsd::file("test.txt", hsd::file::options::text::read);
    
    auto c = file.read_line().unwrap().parse<hsd::i32>();
}