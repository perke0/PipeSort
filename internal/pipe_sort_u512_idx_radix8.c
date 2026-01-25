#include "pipe_sort_u512_idx_radix8.h"
#include <string.h>

// Compare by idx for insertion base case
static inline int u512_less_by_idx(const u512* keys, uint32_t ia, uint32_t ib) {
    const u512* a = &keys[ia];
    const u512* b = &keys[ib];
    if (a->w7 != b->w7) return a->w7 < b->w7;
    if (a->w6 != b->w6) return a->w6 < b->w6;
    if (a->w5 != b->w5) return a->w5 < b->w5;
    if (a->w4 != b->w4) return a->w4 < b->w4;
    if (a->w3 != b->w3) return a->w3 < b->w3;
    if (a->w2 != b->w2) return a->w2 < b->w2;
    if (a->w1 != b->w1) return a->w1 < b->w1;
    return a->w0 < b->w0;
}

static inline void insertion_sort_idx(uint32_t* idx, const u512* keys, int n) {
    for (int i = 1; i < n; i++) {
        uint32_t key = idx[i];
        int j = i - 1;
        while (j >= 0 && u512_less_by_idx(keys, key, idx[j])) {
            idx[j + 1] = idx[j];
            j--;
        }
        idx[j + 1] = key;
    }
}

// limb 7 = w7 ... limb 0 = w0
static inline uint64_t limb_at_0_7(const u512* k, int limb0to7) {
    switch (limb0to7) {
        case 7: return k->w7;
        case 6: return k->w6;
        case 5: return k->w5;
        case 4: return k->w4;
        case 3: return k->w3;
        case 2: return k->w2;
        case 1: return k->w1;
        default: return k->w0;
    }
}

// Extract 3-bit digit at global bit position `startbit`
// startbit is the LOWEST bit of the 3-bit group.
// Example: startbit=509 uses w7 bits 63..61 (shift=61).
static inline unsigned digit3(const u512* k, int startbit) {
    int limb = startbit / 64;      // 0..7, where 7 corresponds to w7
    int shift = startbit % 64;     // 0..63; here we ensure shift<=61
    uint64_t w = limb_at_0_7(k, limb);
    return (unsigned)((w >> shift) & 7ULL);
}

static void msd_radix8_rec(uint32_t* idx, uint32_t* tmp, const u512* keys, int n, int startbit) {
    // Tune this on your machine; you observed higher is sometimes better.
    const int INSERTION_CUTOFF = 96;

    if (n <= 1) return;

    // When we run out of bits, fallback to insertion (should be tiny by then)
    if (startbit < 0 || n <= INSERTION_CUTOFF) {
        insertion_sort_idx(idx, keys, n);
        return;
    }

    // Count 8 buckets
    int c[8] = {0,0,0,0,0,0,0,0};

    // We do TWO passes over idx (count + scatter),
    // but no diff scan and only one limb load per element per pass.
    for (int i = 0; i < n; i++) {
        const u512* k = &keys[idx[i]];
        c[digit3(k, startbit)]++;
    }

    // If all fell into one bucket, just go down to next digit (skip work)
    int nonempty = 0;
    for (int b = 0; b < 8; b++) nonempty += (c[b] != 0);
    if (nonempty <= 1) {
        msd_radix8_rec(idx, tmp, keys, n, startbit - 3);
        return;
    }

    // Prefix sums -> bucket offsets
    int off[8];
    off[0] = 0;
    for (int b = 1; b < 8; b++) off[b] = off[b - 1] + c[b - 1];

    int pos[8];
    for (int b = 0; b < 8; b++) pos[b] = off[b];

    // Scatter
    for (int i = 0; i < n; i++) {
        uint32_t id = idx[i];
        const u512* k = &keys[id];
        unsigned d = digit3(k, startbit);
        tmp[pos[d]++] = id;
    }

    // Copy back (simple + correct)
    memcpy(idx, tmp, (size_t)n * sizeof(uint32_t));

    // Recurse each bucket
    for (int b = 0; b < 8; b++) {
        int sz = c[b];
        if (sz > 1) {
            msd_radix8_rec(idx + off[b], tmp + off[b], keys, sz, startbit - 3);
        }
    }
}

void pipe_sort_u512_index_radix8_fixed(uint32_t* idx, uint32_t* tmp, const u512* keys, int n) {
    if (n <= 1) return;

    // top 3-bit group: bits 511..509 -> startbit = 509 (shift 61 in w7)
    msd_radix8_rec(idx, tmp, keys, n, 509);
}

