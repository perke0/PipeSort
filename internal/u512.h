#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint64_t w7; // most significant
    uint64_t w6;
    uint64_t w5;
    uint64_t w4;
    uint64_t w3;
    uint64_t w2;
    uint64_t w1;
    uint64_t w0; // least significant
} u512;

int  u512_cmp(const u512* a, const u512* b);
void pipe_sort_u512(u512* a, int n);
int  u512_is_sorted(const u512* a, int n);

#ifdef __cplusplus
}
#endif

