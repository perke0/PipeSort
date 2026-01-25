#include "pipesort/pipesort.h"
#include "pipe_sort_u128.h"
#include "u128.h"

/* Layout compatibility:
 *   psort_u128 == u128  (hi, lo)
 */
void psort_u128(psort_u128_t* keys, int n) {
    pipe_sort_u128((u128*)keys, n);
}

int psort_u128_is_sorted(const psort_u128_t* keys, int n) {
    return u128_is_sorted((const u128*)keys, n);
}
