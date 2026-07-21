#include <benchmark/benchmark.h>
#include <vector>

static void BM_VectorPushBack(benchmark::State &state)
{
    for (auto _ : state)
    {
        std::vector<int> v;
        for (int i = 0; i < state.range(0); ++i)
            v.push_back(i);
    }
}

BENCHMARK(BM_VectorPushBack)->Arg(100)->Arg(1000)->Arg(10000);

BENCHMARK_MAIN();