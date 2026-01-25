#include "pipe_sort_u256_idx_radix8.h"
#include <string.h>

// limb 3 = w3 ... limb 0 = w0
static inline uint64_t limb_at_0_3(const u256* k, int limb0to3) {
    switch (limb0to3) {
        case 3: return k->w3;
        case 2: return k->w2;
        case 1: return k->w1;
        default: return k->w0;
    }
}

static inline int u256_less_by_idx(const u256* keys, uint32_t ia, uint32_t ib) {
    const u256* a = &keys[ia];
    const u256* b = &keys[ib];
    if (a->w3 != b->w3) return a->w3 < b->w3;
    if (a->w2 != b->w2) return a->w2 < b->w2;
    if (a->w1 != b->w1) return a->w1 < b->w1;
    return a->w0 < b->w0;
}

static inline void insertion_sort_idx(uint32_t* idx, const u256* keys, int n) {
    for (int i = 1; i < n; i++) {
        uint32_t key = idx[i];
        int j = i - 1;
        while (j >= 0 && u256_less_by_idx(keys, key, idx[j])) {
            idx[j + 1] = idx[j];
            j--;
        }
        idx[j + 1] = key;
    }
}

// Extract 3-bit digit at global bit position startbit (lowest bit of the 3-bit group).
// Bits are numbered [255..0] where 255 is MSB of w3 and 0 is LSB of w0.
// For top group bits 255..253 => startbit = 253.
static inline unsigned digit3(const u256* k, int startbit) {
    int limb = startbit / 64;      // 0..3 where 3 maps to w3
    int shift = startbit % 64;     // ensure shift<=61 by choosing startbits 253,250,...,1,0? (we stop at <0)
    uint64_t w = limb_at_0_3(k, limb);
    return (unsigned)((w >> shift) & 7ULL);
}

static void msd_radix8_rec(uint32_t* idx, uint32_t* tmp, const u256* keys, int n, int startbit) {
    // For hash like keys, higher cutoffs can be faster (less pass/recursion overhead).
    const int INSERTION_CUTOFF = 96;

    if (n <= 1) return;
    if (startbit < 0 || n <= INSERTION_CUTOFF) {
        insertion_sort_idx(idx, keys, n);
        return;
    }

    int c[8] = {0,0,0,0,0,0,0,0};

    // Count
    for (int i = 0; i < n; i++) {
        const u256* k = &keys[idx[i]];
        c[digit3(k, startbit)]++;
    }

    // If all in one bucket, skip to next digit
    int nonempty = 0;
    for (int b = 0; b < 8; b++) nonempty += (c[b] != 0);
    if (nonempty <= 1) {
        msd_radix8_rec(idx, tmp, keys, n, startbit - 3);
        return;
    }

    // Prefix sums -> offsets
    int off[8];
    off[0] = 0;
    for (int b = 1; b < 8; b++) off[b] = off[b - 1] + c[b - 1];

    int pos[8];
    for (int b = 0; b < 8; b++) pos[b] = off[b];

    // Scatter
    for (int i = 0; i < n; i++) {
        uint32_t id = idx[i];
        const u256* k = &keys[id];
        unsigned d = digit3(k, startbit);
        tmp[pos[d]++] = id;
    }

    // Copy back
    memcpy(idx, tmp, (size_t)n * sizeof(uint32_t));

    // Recurse buckets
    for (int b = 0; b < 8; b++) {
        int sz = c[b];
        if (sz > 1) {
            msd_radix8_rec(idx + off[b], tmp + off[b], keys, sz, startbit - 3);
        }
    }
}

void pipe_sort_u256_index_radix8_fixed(uint32_t* idx, uint32_t* tmp, const u256* keys, int n) {
    if (n <= 1) return;
    // Top 3-bit group is bits 255..253 => startbit = 253
    msd_radix8_rec(idx, tmp, keys, n, 253);
}

