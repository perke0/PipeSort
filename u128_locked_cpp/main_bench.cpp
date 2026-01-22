// Build:
// g++ -O3 -march=native -flto -std=c++20 main_bench.cpp pipe_sort_u128.cpp -o bench_cpp

#include "pipe_sort_u128.hpp"
#include <vector>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstdint>

static inline std::uint64_t xorshift64(std::uint64_t& s) {
    std::uint64_t x = s;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    s = x;
    return x;
}

static double now_seconds() {
    using clock = std::chrono::steady_clock;
    return std::chrono::duration<double>(clock::now().time_since_epoch()).count();
}

enum dataset_kind { DATA_RANDOM, DATA_SKEWED_PREFIX, DATA_DUPLICATES };

static void fill_data(u128* a, int n, dataset_kind kind, std::uint64_t seed) {
    std::uint64_t s = seed;

    if (kind == DATA_RANDOM) {
        for (int i = 0; i < n; i++) {
            a[i].hi = xorshift64(s);
            a[i].lo = xorshift64(s);
        }
        return;
    }

    if (kind == DATA_SKEWED_PREFIX) {
        std::uint64_t shared_hi = (xorshift64(s) & 0xFFFF000000000000ULL);
        for (int i = 0; i < n; i++) {
            std::uint64_t noise_hi = xorshift64(s) & 0x0000FFFFFFFFFFFFULL;
            a[i].hi = shared_hi | noise_hi;
            a[i].lo = xorshift64(s);
        }
        return;
    }

    // DATA_DUPLICATES (256 keys repeated)
    u128 pool[256];
    for (int i = 0; i < 256; i++) {
        pool[i].hi = xorshift64(s);
        pool[i].lo = xorshift64(s);
    }
    for (int i = 0; i < n; i++) {
        std::uint64_t r = xorshift64(s);
        a[i] = pool[int(r & 255ULL)];
    }
}

static void run_one(const char* name, dataset_kind kind, int n, int iters) {
    std::vector<u128> base((size_t)n);
    std::vector<u128> work((size_t)n);

    fill_data(base.data(), n, kind, 123456789ULL);

    // Warmup PipeSort
    std::memcpy(work.data(), base.data(), (size_t)n * sizeof(u128));
    pipe_sort_u128(work.data(), n);
    assert(u128_is_sorted(work.data(), n));

    // Time PipeSort
    double t0 = now_seconds();
    for (int i = 0; i < iters; i++) {
        std::memcpy(work.data(), base.data(), (size_t)n * sizeof(u128));
        pipe_sort_u128(work.data(), n);
    }
    double t1 = now_seconds();
    assert(u128_is_sorted(work.data(), n));

    // Warmup std::sort
    std::memcpy(work.data(), base.data(), (size_t)n * sizeof(u128));
    std::sort(work.begin(), work.end(), [](const u128& a, const u128& b) {
        return (a.hi < b.hi) || (a.hi == b.hi && a.lo < b.lo);
    });
    assert(u128_is_sorted(work.data(), n));

    // Time std::sort
    double t2 = now_seconds();
    for (int i = 0; i < iters; i++) {
        std::memcpy(work.data(), base.data(), (size_t)n * sizeof(u128));
        std::sort(work.begin(), work.end(), [](const u128& a, const u128& b) {
            return (a.hi < b.hi) || (a.hi == b.hi && a.lo < b.lo);
        });
    }
    double t3 = now_seconds();
    assert(u128_is_sorted(work.data(), n));

    double pipe_s = (t1 - t0);
    double sort_s = (t3 - t2);

    double items = (double)n * (double)iters;
    double pipe_mips = (items / pipe_s) / 1e6;
    double sort_mips = (items / sort_s) / 1e6;

    std::printf("\n[%s] n=%d iters=%d\n", name, n, iters);
    std::printf("  PipeSort : %8.2f ms  (%.2f M items/s)\n", pipe_s * 1000.0, pipe_mips);
    std::printf("  std::sort: %8.2f ms  (%.2f M items/s)\n", sort_s * 1000.0, sort_mips);
    std::printf("  Speedup  : %8.2fx (std::sort / PipeSort)\n", sort_s / pipe_s);
}

int main(int argc, char** argv) {
    int n = 200000;
    int iters = 6;

    if (argc >= 2) n = std::atoi(argv[1]);
    if (argc >= 3) iters = std::atoi(argv[2]);

    // Smoke test
    {
        u128 small[10];
        fill_data(small, 10, DATA_RANDOM, 42);
        pipe_sort_u128(small, 10);
        assert(u128_is_sorted(small, 10));
    }

    run_one("Random", DATA_RANDOM, n, iters);
    run_one("SkewedPrefix", DATA_SKEWED_PREFIX, n, iters);
    run_one("Duplicates(256)", DATA_DUPLICATES, n, iters);

    std::puts("\nC++ benchmark done.");
    return 0;
}

