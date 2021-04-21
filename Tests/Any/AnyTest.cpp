#include <Io.hpp>
#include <Any.hpp>

struct S
{
    int _a;
    char _b;
    float _c;
    const char* _d;

    S(int a, char b, float c, const char* d) : _a(a), _b(b), _c(c), _d(d) {}
};

int main()
{
    // any type
    hsd::any a = 1;
    hsd::io::print<"{}\n">(a.cast_to<int>().unwrap());
    a = 3.14;
    hsd::io::print<"{}\n">(a.cast_to<double>().unwrap());
    a = true;
    hsd::io::print<"{}\n">(a.cast_to<bool>().unwrap());
 
    // bad cast
    a = 1;
    hsd::io::print<"{}\n">(a.cast_to<float>().unwrap_err()());
 
    // has value
    a = 1;
    
    if (a.has_value())
        hsd::io::print<"Has value\n">();
 
    // reset
    a.reset();
    
    if (!a.has_value())
        hsd::io::print<"No value\n">();
 
    // pointer to contained data
    a = 1;
    
    if (int* i = a.cast_if<int>())
        hsd::io::print<"{}\n">(*i);

    // holds a given type
    a.emplace<S>(1, 'c', 4.2f, "str");
    
    if (a.holds_type<S>())
        hsd::io::print<"Holds a S\n">();
    
    if (a.holds_type<int>())
        hsd::io::print<"Holds an int\n">;
}