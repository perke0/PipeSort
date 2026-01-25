#pragma once
#include <stdint.h>
#include "u256.h"

#ifdef __cplusplus
extern "C" {
#endif

// Sort indices so that keys[idx[i]] is in ascending order (w3..w0).
// tmp must be length n.
void pipe_sort_u256_index_radix8_fixed(uint32_t* idx, uint32_t* tmp, const u256* keys, int n);

#ifdef __cplusplus
}
#endif

