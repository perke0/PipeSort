#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 *  PipeSort (psort_) — public C API
 *
 *  Key conventions:
 *   - unsigned integers
 *   - big endian limb order (most significant limb first)
 * ============================================================ */

/* ---------------- Universal: N × 64-bit limbs ----------------
 *
 * Sorts N keys stored in a flat uint64_t array:
 *   keys length = n * limbs
 *   key i stored at keys[i*limbs + 0 .. i*limbs + limbs-1]
 */
void psort_u(uint64_t* keys, size_t n, size_t limbs);

/* ---------------- Fixed width key types ---------------- */

typedef struct { uint64_t hi, lo; } psort_u128_t;

/* 256-bit key as 4×64-bit limbs (w3 MS .. w0 LS) */
typedef struct { uint64_t w3, w2, w1, w0; } psort_u256_t;

/* 512-bit key as 8×64-bit limbs (w7 MS .. w0 LS) */
typedef struct { uint64_t w7,w6,w5,w4,w3,w2,w1,w0; } psort_u512_t;

/* ---------------- Specialized variants ---------------- */

/* In place sort for psort_u128 */
void psort_u128(psort_u128_t* keys, int n);
int  psort_u128_is_sorted(const psort_u128_t* keys, int n);

/* Index sort (does not move keys). tmp must be length n. */
void psort_u256_index(uint32_t* idx, uint32_t* tmp,
                      const psort_u256_t* keys, int n);

void psort_u512_index(uint32_t* idx, uint32_t* tmp,
                      const psort_u512_t* keys, int n);

#ifdef __cplusplus
}
#endif
