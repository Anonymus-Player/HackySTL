#include <CompressedPair.hpp>
#include <Pair.hpp>

#include <iostream>

struct empty {};

// constexpr implies inline
static constexpr empty empty_v{};

int main() {
    hsd::compressed_pair<int, float> a(3, 4.f);
    hsd::compressed_pair<int, float> b(3, 4);

    hsd::compressed_pair<int, empty> c(1, empty_v);
    hsd::compressed_pair<empty, int> d(empty_v, 1);
    hsd::compressed_pair<empty, empty> e;

    static_assert(sizeof(a) == sizeof(hsd::pair<int, float>));
    static_assert(sizeof(c) == sizeof(int));
    static_assert(sizeof(d) == sizeof(int));
    static_assert(sizeof(e) == sizeof(empty) * 2);
    // it fails because e has a size of 1
    // there is no such thing as sizeof(something) == 0

    std::cout << '(' << a.first() << ", " << a.get<1>() << ")\n";
    std::cout << '(' << b.get<0>() << ", " << b.second() << ")\n";
    std::cout << '(' << c.first() << ", empty)\n";

    // Compile-time error
    //a.get<2>();
}