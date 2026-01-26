#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "../include/pipesort/pipesort.h"   // psort_u128_t, psort_u128, psort_u128_is_sorted

static inline uint64_t xorshift64(uint64_t *s) {
    uint64_t x = *s;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    *s = x;
    return x;
}

static int cmp_u128(const void *a, const void *b) {
    const psort_u128_t *x = (const psort_u128_t *)a;
    const psort_u128_t *y = (const psort_u128_t *)b;
    if (x->hi < y->hi) return -1;
    if (x->hi > y->hi) return  1;
    if (x->lo < y->lo) return -1;
    if (x->lo > y->lo) return  1;
    return 0;
}

static double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

int main(int argc, char **argv) {
    int n = (argc > 1) ? atoi(argv[1]) : 5000000;
    uint64_t seed = (argc > 2) ? (uint64_t)strtoull(argv[2], NULL, 10) : 123;

    if (n <= 0) {
        fprintf(stderr, "n must be > 0\n");
        return 2;
    }

    printf("bench_u128_qsort: n=%d seed=%llu\n", n, (unsigned long long)seed);

    psort_u128_t *a = (psort_u128_t *)malloc((size_t)n * sizeof(psort_u128_t));
    psort_u128_t *b = (psort_u128_t *)malloc((size_t)n * sizeof(psort_u128_t));
    if (!a || !b) {
        fprintf(stderr, "malloc failed (n=%d)\n", n);
        free(a); free(b);
        return 2;
    }

    // Generate data
    uint64_t s = seed ? seed : 1;
    for (int i = 0; i < n; i++) {
        a[i].hi = xorshift64(&s);
        a[i].lo = xorshift64(&s);
    }
    memcpy(b, a, (size_t)n * sizeof(psort_u128_t));

    // qsort
    double t0 = now_sec();
    qsort(a, (size_t)n, sizeof(psort_u128_t), cmp_u128);
    double t1 = now_sec();

    // psort_u128
    double t2 = now_sec();
    psort_u128(b, n);
    double t3 = now_sec();

    int ok_q = psort_u128_is_sorted(a, n);
    int ok_p = psort_u128_is_sorted(b, n);

    printf("qsort     time: %.6f s  sorted: %s\n", (t1 - t0), ok_q ? "YES" : "NO");
    printf("psort_u128 time: %.6f s  sorted: %s\n", (t3 - t2), ok_p ? "YES" : "NO");

    if (!ok_q || !ok_p) {
        fprintf(stderr, "ERROR: sort failed\n");
        free(a); free(b);
        return 1;
    }

    double speedup = (t1 - t0) / (t3 - t2);
    printf("speedup (qsort/psort): %.3fx\n", speedup);

    free(a);
    free(b);
    return 0;
}

