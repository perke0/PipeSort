#include "pipesort/pipesort.h"

#include <string.h>   // memcpy, memmove
#include <stdlib.h>   // malloc, free

// ---------------- helpers ----------------

static inline int psort_msb_pos_u64(uint64_t x) {
#if defined(__GNUC__) || defined(__clang__)
    return x ? (int)(63 - __builtin_clzll(x)) : -1;
#else
    if (!x) return -1;
    int p = 0;
    while (x >>= 1) p++;
    return p;
#endif
}

static int psort_get_max_bit_u(const uint64_t* keys, size_t n, size_t limbs) {
    int max_bit = -1;
    for (size_t i = 0; i < n; i++) {
        const uint64_t* k = keys + i * limbs;
        for (size_t l = 0; l < limbs; l++) {
            int bit = psort_msb_pos_u64(k[l]);
            if (bit >= 0) {
                int abs_bit = bit + (int)((limbs - 1 - l) * 64);
                if (abs_bit > max_bit) max_bit = abs_bit;
                break;
            }
        }
    }
    return max_bit;
}

// returns 1 if a >= b (big endian limbs), else 0
static int psort_ge_key(const uint64_t* a, const uint64_t* b, size_t limbs) {
    for (size_t i = 0; i < limbs; i++) {
        if (a[i] > b[i]) return 1;
        if (a[i] < b[i]) return 0;
    }
    return 1;
}

static void psort_swap_key(uint64_t* a, uint64_t* b, size_t limbs) {
    for (size_t i = 0; i < limbs; i++) {
        uint64_t t = a[i];
        a[i] = b[i];
        b[i] = t;
    }
}

static void psort_insertion_sort_u(uint64_t* keys, size_t n, size_t limbs) {
    uint64_t* tmp = (uint64_t*)malloc(limbs * sizeof(uint64_t));
    if (!tmp) return;

    for (size_t i = 1; i < n; i++) {
        memcpy(tmp, keys + i * limbs, limbs * sizeof(uint64_t));

        size_t j = i;
        while (j > 0 && psort_ge_key(keys + (j - 1) * limbs, tmp, limbs)) {
            memmove(keys + j * limbs,
                    keys + (j - 1) * limbs,
                    limbs * sizeof(uint64_t));
            j--;
        }
        memcpy(keys + j * limbs, tmp, limbs * sizeof(uint64_t));
    }

    free(tmp);
}

static void psort_pipe_sort_u(uint64_t* keys, size_t n, size_t limbs, int bit) {
    const size_t SMALL = 16;
    if (n <= 1 || bit < 0) return;

    if (n <= SMALL) {
        psort_insertion_sort_u(keys, n, limbs);
        return;
    }

    size_t limb_index = (size_t)(limbs - 1 - (size_t)(bit / 64));
    size_t bit_in_limb = (size_t)(bit % 64);

    // Adaptive bit skip: check if all keys share this bit
    uint64_t first_bit = (keys[0 * limbs + limb_index] >> bit_in_limb) & 1ULL;
    int all_same = 1;
    for (size_t i = 1; i < n; i++) {
        uint64_t b = (keys[i * limbs + limb_index] >> bit_in_limb) & 1ULL;
        if (b != first_bit) { all_same = 0; break; }
    }
    if (all_same) {
        psort_pipe_sort_u(keys, n, limbs, bit - 1);
        return;
    }

    // Partition by current bit
    size_t left = 0;
    size_t right = n - 1;

    while (left <= right) {
        uint64_t lb = (keys[left * limbs + limb_index] >> bit_in_limb) & 1ULL;
        if (lb == 0) { left++; continue; }

        uint64_t rb = (keys[right * limbs + limb_index] >> bit_in_limb) & 1ULL;
        if (rb == 1) { 
            if (right == 0) break;
            right--; 
            continue; 
        }

        psort_swap_key(keys + left * limbs, keys + right * limbs, limbs);
        left++;
        if (right == 0) break;
        right--;
    }

    psort_pipe_sort_u(keys, left, limbs, bit - 1);
    psort_pipe_sort_u(keys + left * limbs, n - left, limbs, bit - 1);
}

// ---------------- public entry ----------------
void psort_u(uint64_t* keys, size_t n, size_t limbs) {
    if (!keys || n <= 1 || limbs == 0) return;
    int max_bit = psort_get_max_bit_u(keys, n, limbs);
    if (max_bit < 0) return;
    psort_pipe_sort_u(keys, n, limbs, max_bit);
}
