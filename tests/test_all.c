#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "../include/pipesort/pipesort.h"   // adjust if your include path differs

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

/* If your header uses different names, change these accordingly. */
static int is_sorted_u128(const psort_u128_t *a, int n) {
    for (int i = 1; i < n; i++) {
        if (a[i-1].hi > a[i].hi) return 0;
        if (a[i-1].hi == a[i].hi && a[i-1].lo > a[i].lo) return 0;
    }
    return 1;
}

int main(int argc, char **argv) {
    int n = (argc > 1) ? atoi(argv[1]) : 200000;
    uint64_t seed = (argc > 2) ? (uint64_t)strtoull(argv[2], NULL, 10) : 123;

    if (n <= 0) {
        fprintf(stderr, "n must be > 0\n");
        return 2;
    }

    psort_u128_t *x = (psort_u128_t *)malloc((size_t)n * sizeof(psort_u128_t));
    if (!x) {
        fprintf(stderr, "malloc failed\n");
        return 2;
    }

    uint64_t s = seed ? seed : 1;
    for (int i = 0; i < n; i++) {
        x[i].hi = xorshift64(&s);
        x[i].lo = xorshift64(&s);
    }

    double t0 = now_sec();
    psort_u128(x, n);
    double t1 = now_sec();

    int ok = is_sorted_u128(x, n);
    printf("psort_u128 time: %.6f s  sorted: %s\n", (t1 - t0), ok ? "YES" : "NO");

    free(x);
    return ok ? 0 : 1;
}

