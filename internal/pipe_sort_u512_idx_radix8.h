#pragma once
#include <stdint.h>
#include "u512.h"

#ifdef __cplusplus
extern "C" {
#endif

// Sort indices by keys[idx[i]] ascending (w7..w0), assuming keys are random like.
// tmp must be length n.
void pipe_sort_u512_index_radix8_fixed(uint32_t* idx, uint32_t* tmp, const u512* keys, int n);

#ifdef __cplusplus
}
#endif

