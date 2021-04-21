#include <TypeTraits.hpp>

struct foo {
    foo() {}
    ~foo() = delete;

    int x;
};

//decltype(foo{}.x) a;
decltype(hsd::declval<foo>().x) a;

int main() {}