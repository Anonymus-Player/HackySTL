#include <benchmark/benchmark.h>
#include "../../cpp/UnorderedMap.hpp"

static void MapBufAlloc(benchmark::State& state)
{
    hsd::uchar buf[4096]{};

    for(auto _ : state)
    {
        hsd::buffered_allocator<hsd::uchar> alloc{buf, 4096};
        hsd::unordered_map<size_t, size_t, hsd::hash<size_t, size_t>, hsd::buffered_allocator> map{alloc};

        [&]<size_t... Ints>(hsd::index_sequence<Ints...>)
        {
            (map.emplace(Ints, Ints), ...);
        }(hsd::make_index_sequence<14>{});

        benchmark::DoNotOptimize(map.begin());
    }
}

static void MapHeapAlloc(benchmark::State& state)
{
    for(auto _ : state)
    {
        hsd::unordered_map<size_t, size_t, hsd::hash<size_t, size_t>> map;

        [&]<size_t... Ints>(hsd::index_sequence<Ints...>)
        {
            (map.emplace(Ints, Ints), ...);
        }(hsd::make_index_sequence<14>{});

        benchmark::DoNotOptimize(map.begin());
    }
}

BENCHMARK(MapBufAlloc);

BENCHMARK(MapHeapAlloc);

BENCHMARK_MAIN();