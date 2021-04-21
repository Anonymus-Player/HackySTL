#include <Io.hpp>
#include <IntegerSequence.hpp>

static void print(size_t S)
{
    hsd::io::print<"{} ">(S);
}

template<typename T, T... ints>
static void print_sequence(hsd::integer_sequence<T, ints...> int_seq)
{
    hsd::io::print<"The sequence of size {}: ">(int_seq.size());
    (print(ints), ...);
    hsd::io::print<"\n">();
}

int main()
{
    print_sequence(hsd::integer_sequence<unsigned, 9, 2, 5, 1, 9, 1, 6>{});
    print_sequence(hsd::make_integer_sequence<int, 20>{});
    print_sequence(hsd::make_index_sequence<10>{});
    print_sequence(hsd::index_sequence_for<float, hsd::io, char>{});
    print_sequence(hsd::ranged::make_index_sequence<5, 10>{});
    print_sequence(hsd::inverse::make_index_sequence<10>{});
    print_sequence(hsd::inverse::ranged::make_index_sequence<10, 5>{});
}