// Build: gcc -O3 -march=native -std=c11 main_bench_c.c pipe_sort_u128.c -o bench_c
#define _POSIX_C_SOURCE 200809L
#include "pipe_sort_u128.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

static inline uint64_t xorshift64(uint64_t* s) {
    uint64_t x = *s;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    *s = x;
    return x;
}

static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

static int qsort_cmp_u128(const void* pa, const void* pb) {
    const u128* a = (const u128*)pa;
    const u128* b = (const u128*)pb;
    return u128_cmp(a, b);
}

typedef enum { DATA_RANDOM, DATA_SKEWED_PREFIX, DATA_DUPLICATES } dataset_kind;

static void fill_data(u128* a, int n, dataset_kind kind, uint64_t seed) {
    uint64_t s = seed;

    if (kind == DATA_RANDOM) {
        for (int i = 0; i < n; i++) {
            a[i].hi = xorshift64(&s);
            a[i].lo = xorshift64(&s);
        }
        return;
    }

    if (kind == DATA_SKEWED_PREFIX) {
        uint64_t shared_hi = (xorshift64(&s) & 0xFFFF000000000000ULL); // share top bits
        for (int i = 0; i < n; i++) {
            uint64_t noise_hi = xorshift64(&s) & 0x0000FFFFFFFFFFFFULL;
            a[i].hi = shared_hi | noise_hi;
            a[i].lo = xorshift64(&s);
        }
        return;
    }

    // DATA_DUPLICATES
    u128 pool[256];
    for (int i = 0; i < 256; i++) {
        pool[i].hi = xorshift64(&s);
        pool[i].lo = xorshift64(&s);
    }
    for (int i = 0; i < n; i++) {
        uint64_t r = xorshift64(&s);
        a[i] = pool[(int)(r & 255ULL)];
    }
}

static void run_one(const char* name, dataset_kind kind, int n, int iters) {
    u128* base = (u128*)malloc((size_t)n * sizeof(u128));
    u128* work = (u128*)malloc((size_t)n * sizeof(u128));
    if (!base || !work) { fprintf(stderr, "alloc failed\n"); exit(1); }

    // Prepare one base dataset
    fill_data(base, n, kind, 123456789ULL);

    // Warm up (Pipe)
    memcpy(work, base, (size_t)n * sizeof(u128));
    pipe_sort_u128(work, n);
    assert(u128_is_sorted(work, n));

    // Pipe Sort timing
    double t0 = now_seconds();
    for (int i = 0; i < iters; i++) {
        memcpy(work, base, (size_t)n * sizeof(u128));
        pipe_sort_u128(work, n);
    }
    double t1 = now_seconds();
    assert(u128_is_sorted(work, n));

    // qsort warm up
    memcpy(work, base, (size_t)n * sizeof(u128));
    qsort(work, (size_t)n, sizeof(u128), qsort_cmp_u128);
    assert(u128_is_sorted(work, n));

    // qsort timing
    double t2 = now_seconds();
    for (int i = 0; i < iters; i++) {
        memcpy(work, base, (size_t)n * sizeof(u128));
        qsort(work, (size_t)n, sizeof(u128), qsort_cmp_u128);
    }
    double t3 = now_seconds();
    assert(u128_is_sorted(work, n));

    double pipe_s = (t1 - t0);
    double qsort_s = (t3 - t2);

    double items = (double)n * (double)iters;
    double pipe_mips = (items / pipe_s) / 1e6;
    double qsort_mips = (items / qsort_s) / 1e6;

    printf("\n[%s] n=%d iters=%d\n", name, n, iters);
    printf("  PipeSort: %8.2f ms  (%.2f M items/s)\n", pipe_s * 1000.0, pipe_mips);
    printf("  qsort   : %8.2f ms  (%.2f M items/s)\n", qsort_s * 1000.0, qsort_mips);
    printf("  Speedup : %8.2fx (qsort / PipeSort)\n", qsort_s / pipe_s);

    free(base);
    free(work);
}

int main(int argc, char** argv) {
    int n = 200000;
    int iters = 6;

    if (argc >= 2) n = atoi(argv[1]);
    if (argc >= 3) iters = atoi(argv[2]);

    // Quick correctness smoke tests
    {
        u128 small[10];
        fill_data(small, 10, DATA_RANDOM, 42);
        pipe_sort_u128(small, 10);
        assert(u128_is_sorted(small, 10));
    }

    run_one("Random", DATA_RANDOM, n, iters);
    run_one("SkewedPrefix", DATA_SKEWED_PREFIX, n, iters);
    run_one("Duplicates(256)", DATA_DUPLICATES, n, iters);

    puts("\nC benchmark done.");
    return 0;
}

