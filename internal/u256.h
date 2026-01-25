#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 256-bit key as 4x64-bit limbs (w3 MS .. w0 LS)
typedef struct {
    uint64_t w3;
    uint64_t w2;
    uint64_t w1;
    uint64_t w0;
} u256;

#ifdef __cplusplus
}
#endif

