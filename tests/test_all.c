#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "../include/pipesort/pipesort.h"  


static inline uint64_t xorshift64(uint64_t *s) {
    uint64_t x = *s;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    *s = x;
    return x;
}

static double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
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

static int is_sorted_u128(const psort_u128_t *a, int n) {
    for (int i = 1; i < n; i++) {
        if (a[i-1].hi > a[i].hi) return 0;
        if (a[i-1].hi == a[i].hi && a[i-1].lo > a[i].lo) return 0;
    }
    return 1;
}

static int arrays_equal_u128(const psort_u128_t *a, const psort_u128_t *b, int n) {
    return memcmp(a, b, (size_t)n * sizeof(psort_u128_t)) == 0;
}

int main(int argc, char **argv) {
    int n = (argc > 1) ? atoi(argv[1]) : 200000;
    uint64_t seed = (argc > 2) ? (uint64_t)strtoull(argv[2], NULL, 10) : 123;

    if (n <= 0) {
        fprintf(stderr, "n must be > 0\n");
        return 2;
    }

    printf("test_all: n=%d seed=%llu\n", n, (unsigned long long)seed);

    psort_u128_t *base = (psort_u128_t *)malloc((size_t)n * sizeof(psort_u128_t));
    psort_u128_t *a_q  = (psort_u128_t *)malloc((size_t)n * sizeof(psort_u128_t));
    psort_u128_t *a_p  = (psort_u128_t *)malloc((size_t)n * sizeof(psort_u128_t));

    if (!base || !a_q || !a_p) {
        fprintf(stderr, "malloc failed\n");
        free(base); free(a_q); free(a_p);
        return 2;
    }

    uint64_t s = seed ? seed : 1;
    for (int i = 0; i < n; i++) {
        base[i].hi = xorshift64(&s);
        base[i].lo = xorshift64(&s);
    }

    memcpy(a_q, base, (size_t)n * sizeof(psort_u128_t));
    memcpy(a_p, base, (size_t)n * sizeof(psort_u128_t));

    double t0 = now_sec();
    qsort(a_q, (size_t)n, sizeof(psort_u128_t), cmp_u128);
    double t1 = now_sec();

    double t2 = now_sec();
    psort_u128(a_p, n);
    double t3 = now_sec();

    int ok_q = is_sorted_u128(a_q, n);
    int ok_p = is_sorted_u128(a_p, n);

    printf("qsort     time: %.6f s  sorted: %s\n", (t1 - t0), ok_q ? "YES" : "NO");
    printf("psort_u128 time: %.6f s  sorted: %s\n", (t3 - t2), ok_p ? "YES" : "NO");

    if (!ok_q || !ok_p) {
        fprintf(stderr, "ERROR: sorting failed\n");
        free(base); free(a_q); free(a_p);
        return 1;
    }

    if (!arrays_equal_u128(a_q, a_p, n)) {
        fprintf(stderr, "ERROR: outputs differ (qsort vs psort_u128)\n");
        free(base); free(a_q); free(a_p);
        return 1;
    }

    double speedup = (t1 - t0) / (t3 - t2);
    printf("speedup (qsort/psort): %.3fx\n", speedup);

    free(base);
    free(a_q);
    free(a_p);
    return 0;
}

