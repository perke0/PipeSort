#pragma once
#include <cstdint>

struct u128 {
    std::uint64_t hi;
    std::uint64_t lo;
};

static inline int u128_cmp(const u128& a, const u128& b) {
    if (a.hi < b.hi) return -1;
    if (a.hi > b.hi) return  1;
    if (a.lo < b.lo) return -1;
    if (a.lo > b.lo) return  1;
    return 0;
}

