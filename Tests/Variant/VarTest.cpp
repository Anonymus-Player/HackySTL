#include <Variant.hpp>
#include <Io.hpp>

template <typename T>
struct verbose 
{
    verbose() 
    {
        hsd::io::print<"{} default constructor\n">(T::name());
    }

    ~verbose() 
    {
        hsd::io::print<"{} destructor\n">(T::name());
    }

    verbose(verbose const&) 
    {
        hsd::io::print<"{} copy constructor\n">(T::name());
    }

    verbose(verbose&&) 
    {
        hsd::io::print<"{} move constructor\n">(T::name());
    }

    auto& operator=(verbose const&) 
    {
        hsd::io::print<"{} copy assignment\n">(T::name());
        return *this;
    }

    auto& operator=(verbose&&) 
    {
        hsd::io::print<"{} move assignment\n">(T::name());
        return *this;
    }
    
protected:
    verbose(int) {} // Silent constructor
};

template <>
struct verbose<void> 
    : verbose<verbose<void>> 
{
    constexpr static char const* name() 
    {
        return "verbose"; 
    }
};

struct A : verbose<A> 
{
    constexpr static char const* name() 
    {
        return "A"; 
    }
};
struct B : verbose<B> 
{
    constexpr static char const* name() 
    {
        return "B"; 
    }
    
    int _a;
    
    B(int a) 
        : verbose<B>(0), _a(a) 
    {
        hsd::io::print<"B constructor\n">();
    }
};

constexpr bool operator==(A const&, A const&) noexcept 
{
    return true; 
}

constexpr bool operator==(B const& a, B const& b) noexcept 
{
    return a._a == b._a;
}

int main() 
{
    hsd::variant<int, float> va1(5);
    hsd::variant<A, B> va2(5);
    hsd::variant<A, B> va3{A()};
    auto va4(va2);
    
    hsd::io::print<"{}\n">(hsd::variant_size<decltype(va4)>::value);

    hsd::io::print<"{}\n">(va1.index());
    hsd::io::print<"{}\n">(va2.index());
    hsd::io::print<"{}\n">(va3.index());
    hsd::io::print<"{}\n">(va4.index());

    hsd::io::print<"{}\n">(va1.get<0>().unwrap());
    
    auto _res = va1.get<float>();

    if(_res) 
    {
        // This actually throws 
        hsd::io::print<"{}\n">(_res.unwrap());
    } 
    else
    {
        hsd::io::err_print<"Caught error: {}\n">(_res.unwrap_err()());
    }

    if (auto* val = hsd::get_if<float>(&va1)) 
    {
        hsd::io::print<"Got value: {}\n">(*val);
    } 
    else 
    {
        hsd::io::print<"No value of type float in va1\n">();
    }
    if (auto* val = va2.get_if<B>()) 
    {
        hsd::io::print<"B inside va2: {}\n">(val->_a);
    } 
    else 
    {
        hsd::io::print<"No B in va2\n">();
    }

    hsd::io::print<"va2 == va3: {}\n">(va2 == va3);
    va2 = va3;
    hsd::io::print<"(after assignment) {}\n">(va2 == va3);
    va3 = hsd::variant<A, B>(6);
    hsd::io::print<"(after new assignment)\nva3 == va4: {}\n">(va3 == va4);
    va3.get<B>().unwrap()._a = 5;
    hsd::io::print<"(after modification) {}\n">(va3 == va4);

    hsd::variant<A, B> va5;

    va5.visit([](auto&&) 
    {
        hsd::io::print<"Auto deduction: {}\n">(HSD_FUNCTION_NAME + 28);
    });

    va5 = B(42);

    va5.visit([](auto&&) 
    {
        hsd::io::print<"Auto deduction: {}\n">(HSD_FUNCTION_NAME + 28);
    });

    hsd::variant<verbose<void>, verbose<void>> va6;
    auto va7 = va6;
}
