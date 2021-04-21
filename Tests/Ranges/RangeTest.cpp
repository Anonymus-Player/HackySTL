#include <Range.hpp>
#include <Vector.hpp>
#include <List.hpp>
#include <UnorderedMap.hpp>
#include <Io.hpp>

int main()
{
    {
        puts("hsd::vector:");
        constexpr auto is_even = [](auto val){ return (val & 1) == 0; };
        constexpr auto square = [](auto val){ return val * val; };
        constexpr auto less = [](auto val){ return val < 5; };
        hsd::vector vals = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}};

        puts("Forward:");

        for(auto& val : vals)
            hsd::io::print<"{} ">(val);

        puts("\nForward, dropped 3 elements:");

        for(auto& val : vals | hsd::views::drop(3))
            hsd::io::print<"{} ">(val);

        puts("\nForward, dropped until 5 elements:");

        for(auto& val : vals | hsd::views::drop_while(less))
            hsd::io::print<"{} ">(val);

        puts("\nForward, took 3 elements:");

        for(auto& val : vals | hsd::views::take(3))
            hsd::io::print<"{} ">(val);

        puts("\nForward, took until 5 elements:");

        for(auto& val : vals | hsd::views::take_while(less))
            hsd::io::print<"{} ">(val);

        puts("\nForward, filtred:");

        for(auto& val : vals | hsd::ranges::filter(is_even))
            hsd::io::print<"{} ">(val);

        puts("\nForward, transformed:");

        for(auto& val : vals | hsd::ranges::transform(square))
            hsd::io::print<"{} ">(val);

        puts("\nReversed:");

        for(auto& val : vals | hsd::views::reverse)
            hsd::io::print<"{} ">(val);

        puts("\nReversed, dropped 5 elements:");

        for(auto& val : vals | hsd::views::reverse | hsd::views::drop(5))
            hsd::io::print<"{} ">(val);

        puts("\nReversed, dropped until 5 elements:");

        for(auto& val : vals | hsd::views::reverse | hsd::views::drop_while(less))
            hsd::io::print<"{} ">(val);

        puts("\nReversed, took 5 elements:");

        for(auto& val : vals | hsd::views::reverse | hsd::views::take(5))
            hsd::io::print<"{} ">(val);

        puts("\nReversed, took until 5 elements:");

        for(auto& val : vals | hsd::views::reverse | hsd::views::take_while(less))
            hsd::io::print<"{} ">(val);

        puts("\nReversed, filtred:");

        for(auto& val : vals | hsd::views::reverse | hsd::ranges::filter(is_even))
            hsd::io::print<"{} ">(val);

        puts("\nReversed, transformed:");

        for(auto& val : vals | hsd::views::reverse | hsd::ranges::transform(square))
            hsd::io::print<"{} ">(val);

        puts("\nRandom:");

        for(auto& val : vals | hsd::views::random)
            hsd::io::print<"{} ">(val);

        puts("\nRandom, dropped 2 elements:");

        for(auto& val : vals | hsd::views::random | hsd::views::drop(2))
            hsd::io::print<"{} ">(val);

        puts("\nRandom, dropped until 5 elements:");

        for(auto& val : vals | hsd::views::random | hsd::views::drop_while(less))
            hsd::io::print<"{} ">(val);

        puts("\nRandom, took 2 elements:");

        for(auto& val : vals | hsd::views::random | hsd::views::take(2))
            hsd::io::print<"{} ">(val);

        puts("\nRandom, took until 5 elements:");

        for(auto& val : vals | hsd::views::random | hsd::views::take_while(less))
            hsd::io::print<"{} ">(val);

        puts("\nRandom, filtred:");

        for(auto& val : vals | hsd::views::random | hsd::ranges::filter(is_even))
            hsd::io::print<"{} ">(val);

        puts("\nRandom, transformed:");

        for(auto& val : vals | hsd::views::random | hsd::ranges::transform(square))
            hsd::io::print<"{} ">(val);

        puts("");
    }

    {
        puts("hsd::list:");
        constexpr auto is_even = [](auto val){ return (val & 1) == 0; };
        constexpr auto square = [](auto val){ return val * val; };
        constexpr auto less = [](auto val){ return val < 5; };
        hsd::list vals = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}};

        puts("Forward:");

        for(auto& val : vals)
            hsd::io::print<"{} ">(val);

        puts("\nForward, dropped 3 elements:");

        for(auto& val : vals | hsd::views::drop(3))
            hsd::io::print<"{} ">(val);

        puts("\nForward, dropped until 5 elements:");

        for(auto& val : vals | hsd::views::drop_while(less))
            hsd::io::print<"{} ">(val);

        puts("\nForward, took 3 elements:");

        for(auto& val : vals | hsd::views::take(3))
            hsd::io::print<"{} ">(val);

        puts("\nForward, took until 5 elements:");

        for(auto& val : vals | hsd::views::take_while(less))
            hsd::io::print<"{} ">(val);

        puts("\nForward, filtred:");

        for(auto& val : vals | hsd::ranges::filter(is_even))
            hsd::io::print<"{} ">(val);

        puts("\nForward, transformed:");

        for(auto& val : vals | hsd::ranges::transform(square))
            hsd::io::print<"{} ">(val);

        puts("\nReversed:");

        for(auto& val : vals | hsd::views::reverse)
            hsd::io::print<"{} ">(val);

        puts("\nReversed, dropped 5 elements:");

        for(auto& val : vals | hsd::views::reverse | hsd::views::drop(5))
            hsd::io::print<"{} ">(val);

        puts("\nReversed, dropped until 5 elements:");

        for(auto& val : vals | hsd::views::reverse | hsd::views::drop_while(less))
            hsd::io::print<"{} ">(val);

        puts("\nReversed, took 5 elements:");

        for(auto& val : vals | hsd::views::reverse | hsd::views::take(5))
            hsd::io::print<"{} ">(val);

        puts("\nReversed, took until 5 elements:");

        for(auto& val : vals | hsd::views::reverse | hsd::views::take_while(less))
            hsd::io::print<"{} ">(val);

        puts("\nReversed, filtred:");

        for(auto& val : vals | hsd::views::reverse | hsd::ranges::filter(is_even))
            hsd::io::print<"{} ">(val);

        puts("\nReversed, transformed:");

        for(auto& val : vals | hsd::views::reverse | hsd::ranges::transform(square))
            hsd::io::print<"{} ">(val);

        puts("\nRandom:");

        for(auto& val : vals | hsd::views::random)
            hsd::io::print<"{} ">(val);

        puts("\nRandom, dropped 2 elements:");

        for(auto& val : vals | hsd::views::random | hsd::views::drop(2))
            hsd::io::print<"{} ">(val);

        puts("\nRandom, dropped until 5 elements:");

        for(auto& val : vals | hsd::views::random | hsd::views::drop_while(less))
            hsd::io::print<"{} ">(val);

        puts("\nRandom, took 2 elements:");

        for(auto& val : vals | hsd::views::random | hsd::views::take(2))
            hsd::io::print<"{} ">(val);

        puts("\nRandom, took until 5 elements:");

        for(auto& val : vals | hsd::views::random | hsd::views::take_while(less))
            hsd::io::print<"{} ">(val);

        puts("\nRandom, filtred:");

        for(auto& val : vals | hsd::views::random | hsd::ranges::filter(is_even))
            hsd::io::print<"{} ">(val);

        puts("\nRandom, transformed:");

        for(auto& val : vals | hsd::views::random | hsd::ranges::transform(square))
            hsd::io::print<"{} ">(val);

        puts("");
    }

    {
        puts("hsd::unordered_map:");
        using namespace hsd::string_literals;
        constexpr auto less = [](const auto& val){ return val.second < 5; };
        constexpr auto is_even = [](const auto& val){ return (val.second & 1) == 0; };
        constexpr auto square = [](const auto& val){ return hsd::pair{val.first, val.second * val.second}; };

        hsd::unordered_map<hsd::string_view, hsd::i32> vals = {{
            hsd::pair{"first"_sv, 1}, 
            hsd::pair{"second"_sv, 2}, 
            hsd::pair{"third"_sv, 3}, 
            hsd::pair{"forth"_sv, 4}, 
            hsd::pair{"fifth"_sv, 5}, 
            hsd::pair{"sixth"_sv, 6}, 
            hsd::pair{"sevnth"_sv, 7}, 
            hsd::pair{"eighth"_sv, 8}, 
            hsd::pair{"ninth"_sv, 9}, 
            hsd::pair{"tenth"_sv, 10}
        }};

        puts("Forward:");

        for(auto& val : vals)
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nForward, dropped 3 elements:");

        for(auto& val : vals | hsd::views::drop(3))
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nForward, dropped until 5 elements:");

        for(auto& val : vals | hsd::views::drop_while(less))
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nForward, took 3 elements:");

        for(auto& val : vals | hsd::views::take(3))
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nForward, took until 5 elements:");

        for(auto& val : vals | hsd::views::take_while(less))
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nForward, filtred:");

        for(auto& val : vals | hsd::ranges::filter(is_even))
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nForward, transformed:");

        for(auto& val : vals | hsd::ranges::transform(square))
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nReversed:");

        for(auto& val : vals | hsd::views::reverse)
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nReversed, dropped 5 elements:");

        for(auto& val : vals | hsd::views::reverse | hsd::views::drop(5))
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nReversed, dropped until 5 elements:");

        for(auto& val : vals | hsd::views::reverse | hsd::views::drop_while(less))
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nReversed, took 5 elements:");

        for(auto& val : vals | hsd::views::reverse | hsd::views::take(5))
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nReversed, took until 5 elements:");

        for(auto& val : vals | hsd::views::reverse | hsd::views::take_while(less))
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nReversed, filtred:");

        for(auto& val : vals | hsd::views::reverse | hsd::ranges::filter(is_even))
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nReversed, transformed:");

        for(auto& val : vals | hsd::views::reverse | hsd::ranges::transform(square))
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nRandom:");

        for(auto& val : vals | hsd::views::random)
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nRandom, dropped 2 elements:");

        for(auto& val : vals | hsd::views::random | hsd::views::drop(2))
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nRandom, dropped until 5 elements:");

        for(auto& val : vals | hsd::views::random | hsd::views::drop_while(less))
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nRandom, took 2 elements:");

        for(auto& val : vals | hsd::views::random | hsd::views::take(2))
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nRandom, took until 5 elements:");

        for(auto& val : vals | hsd::views::random | hsd::views::take_while(less))
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nRandom, filtred:");

        for(auto& val : vals | hsd::views::random | hsd::ranges::filter(is_even))
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("\nRandom, transformed:");

        for(auto& val : vals | hsd::views::random | hsd::ranges::transform(square))
            hsd::io::print<"({}, {}) ">(val.first, val.second);

        puts("");
    }
}