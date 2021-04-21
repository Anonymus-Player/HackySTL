#include <Functional.hpp>

// Implementations for "defaultarg.cpp"
static int func() noexcept 
{
    return 4;
}

extern "C" int bop(int a, int b) noexcept 
{
    return a * b;
};

extern "C" hsd::function<int()> f(func);
