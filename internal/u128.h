#pragma once
#include <stdint.h>

typedef struct {
    uint64_t hi;
    uint64_t lo;
} u128;

static inline int u128_cmp(const u128* a, const u128* b) {
    if (a->hi < b->hi) return -1;
    if (a->hi > b->hi) return  1;
    if (a->lo < b->lo) return -1;
    if (a->lo > b->lo) return  1;
    return 0;
}

