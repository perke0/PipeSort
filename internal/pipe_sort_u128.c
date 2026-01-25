#include "pipe_sort_u128.h"
#include <stddef.h>
#include <assert.h>

static inline int clz64_nonzero(uint64_t x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_clzll(x);
#else
    int n = 0;
    while ((x & (1ULL << 63)) == 0) { n++; x <<= 1; }
    return n;
#endif
}

static inline int highest_set_bit_index_u128(uint64_t diff_hi, uint64_t diff_lo) {
    // diff is guaranteed non zero by caller
    if (diff_hi) return 127 - clz64_nonzero(diff_hi);
    return 63 - clz64_nonzero(diff_lo);
}

static inline void insertion_sort_u128(u128* a, int n) {
    for (int i = 1; i < n; i++) {
        u128 key = a[i];
        int j = i - 1;
        while (j >= 0 && u128_cmp(&a[j], &key) > 0) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

// Faster partition: choose limb once, no per element (bit>=64) branch
static inline int partition_by_bit_hi(u128* a, int n, int shift) {
    const uint64_t mask = 1ULL << shift;
    int i = 0, j = n - 1;

    while (i <= j) {
        while (i <= j && (a[i].hi & mask) == 0) i++;
        while (i <= j && (a[j].hi & mask) != 0) j--;
        if (i < j) {
            u128 tmp = a[i];
            a[i] = a[j];
            a[j] = tmp;
            i++; j--;
        }
    }
    return i;
}

static inline int partition_by_bit_lo(u128* a, int n, int shift) {
    const uint64_t mask = 1ULL << shift;
    int i = 0, j = n - 1;

    while (i <= j) {
        while (i <= j && (a[i].lo & mask) == 0) i++;
        while (i <= j && (a[j].lo & mask) != 0) j--;
        if (i < j) {
            u128 tmp = a[i];
            a[i] = a[j];
            a[j] = tmp;
            i++; j--;
        }
    }
    return i;
}

void pipe_sort_u128(u128* restrict a, int n) {
    const int INSERTION_CUTOFF = 48;

    while (n > INSERTION_CUTOFF) {
        const uint64_t bh = a[0].hi;
        const uint64_t bl = a[0].lo;

        uint64_t diff_hi = 0, diff_lo = 0;

        // Unrolled diff scan (hot loop)
        int i = 1;
        for (; i + 3 < n; i += 4) {
            diff_hi |= (a[i+0].hi ^ bh) | (a[i+1].hi ^ bh) | (a[i+2].hi ^ bh) | (a[i+3].hi ^ bh);
            diff_lo |= (a[i+0].lo ^ bl) | (a[i+1].lo ^ bl) | (a[i+2].lo ^ bl) | (a[i+3].lo ^ bl);
        }
        for (; i < n; i++) {
            diff_hi |= (a[i].hi ^ bh);
            diff_lo |= (a[i].lo ^ bl);
        }

        if ((diff_hi | diff_lo) == 0) return; // all equal in this range

        const int bit_index = highest_set_bit_index_u128(diff_hi, diff_lo);

        int split;
        if (bit_index >= 64) split = partition_by_bit_hi(a, n, bit_index - 64);
        else                split = partition_by_bit_lo(a, n, bit_index);

        // If diff had this bit set, split must be non degenerate.
        // If this ever fires, there's a bug in diff computation or partitioning.
        assert(split > 0 && split < n);

        const int left_n  = split;
        const int right_n = n - split;

        // Tail recursion elimination: recurse smaller side
        if (left_n < right_n) {
            pipe_sort_u128(a, left_n);
            a += split;
            n = right_n;
        } else {
            pipe_sort_u128(a + split, right_n);
            n = left_n;
        }
    }

    insertion_sort_u128(a, n);
}

int u128_is_sorted(const u128* a, int n) {
    for (int i = 1; i < n; i++) {
        if (u128_cmp(&a[i - 1], &a[i]) > 0) return 0;
    }
    return 1;
}

