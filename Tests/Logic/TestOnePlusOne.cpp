#include <Extra/Constexpr/BinaryArithmetic.hpp>
#include <Extra/Constexpr/BooleanSerialize.hpp>

#include <iostream>

template <hsd::Number v>
auto serialize_bitset() {
    return hsd::serialize_value_v<v>.bs();
}

int main() 
{
    static_assert(hsd::is_same<hsd::ripple_adder<hsd::natural::one, hsd::natural::one, hsd::bit::zero>, hsd::natural::two>::value);

    std::cout << "natural::one is " << serialize_bitset<hsd::natural::one>() << '\n';
    std::cout << "natural::two is " << serialize_bitset<hsd::natural::two>() << '\n';
    std::cout << "natural::three is " << serialize_bitset<hsd::natural::three>() << '\n';
    std::cout << "natural::four is " << serialize_bitset<hsd::natural::four>() << '\n';
    std::cout << "natural::two + natural::four is " <<
        serialize_bitset<hsd::ripple_adder<hsd::natural::two, hsd::natural::four, hsd::bit::zero> >() << '\n';
    std::cout << "5 + natural::four is " <<
        serialize_bitset<hsd::ripple_adder<
            hsd::cons_pair<hsd::bit::one, hsd::cons_pair<hsd::bit::zero, hsd::cons_pair<hsd::bit::one, hsd::nil> > >,
        hsd::natural::four, hsd::bit::zero> >() << '\n';
}
