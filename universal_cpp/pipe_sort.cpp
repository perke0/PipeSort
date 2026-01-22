#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <vector>
#include "pipe_sort.h"
// Compute true maximum bit across all multi-limb integers
int get_max_bit(uint64_t* arr, size_t n, size_t limbs) {
    int max_bit = -1;
    for (size_t i = 0; i < n; i++) {
        // Scan from most significant limb to least
        for (size_t l = 0; l < limbs; l++) {
            int bit = msb_pos(arr[i * limbs + l]);
            if (bit >= 0) {
                max_bit = std::max(max_bit, (int)(bit + (limbs - 1 - l) * 64));
                break; // go to next integer
            }
        }
    }
    return max_bit;
}

// Compute MSB position of a 64-bit integer
int msb_pos(uint64_t x) {
    return x ? 63 - __builtin_clzll(x) : -1;
}

// Compare multi-limb integers (big-endian)
bool greater_or_equal(const uint64_t* a, const uint64_t* b, size_t limbs) {
    for (size_t i = 0; i < limbs; i++) {
        if (a[i] > b[i]) return true;
        if (a[i] < b[i]) return false;
    }
    return true;
}

// Insertion sort for small buckets
void insertion_sort(uint64_t* arr, size_t n, size_t limbs) {
    for (size_t i = 1; i < n; i++) {
        std::vector<uint64_t> key(arr + i * limbs, arr + i * limbs + limbs);
        size_t j = i;
        while (j > 0 && greater_or_equal(arr + (j - 1) * limbs, key.data(), limbs)) {
            std::copy(arr + (j - 1) * limbs, arr + (j - 1) * limbs + limbs, arr + j * limbs);
            j--;
        }
        std::copy(key.begin(), key.end(), arr + j * limbs);
    }
}

// Adaptive Pipe Sort
void pipe_sort(uint64_t* arr, size_t n, size_t limbs, int bit) {
    constexpr size_t SMALL = 16;
    if (n <= 1 || bit < 0) return;
    if (n <= SMALL) {
        insertion_sort(arr, n, limbs);
        return;
    }

    size_t limb_index = limbs - 1 - bit / 64;
    size_t bit_in_limb = bit % 64;

    // Adaptive bit skip
    uint64_t first_bit = (arr[0 * limbs + limb_index] >> bit_in_limb) & 1ULL;
    bool all_same = true;
    for (size_t i = 1; i < n; i++) {
        uint64_t b = (arr[i * limbs + limb_index] >> bit_in_limb) & 1ULL;
        if (b != first_bit) { all_same = false; break; }
    }
    if (all_same) {
        pipe_sort(arr, n, limbs, bit - 1);
        return;
    }

    size_t left = 0;
    size_t right = n - 1;

    while (left <= right) {
        uint64_t left_bit = (arr[left * limbs + limb_index] >> bit_in_limb) & 1ULL;
        if (left_bit == 0) { left++; continue; }

        uint64_t right_bit = (arr[right * limbs + limb_index] >> bit_in_limb) & 1ULL;
        if (right_bit == 1) { right--; continue; }

        for (size_t l = 0; l < limbs; l++)
            std::swap(arr[left * limbs + l], arr[right * limbs + l]);

        left++; right--;
    }

    pipe_sort(arr, left, limbs, bit - 1);
    pipe_sort(arr + left * limbs, n - left, limbs, bit - 1);
}

