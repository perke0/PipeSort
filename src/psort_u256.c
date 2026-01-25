#include "pipesort/pipesort.h"
#include "pipe_sort_u256_idx_radix8.h"
#include "u256.h"

/* Layout compatibility:
 *   psort_u256 == u256  (w3,w2,w1,w0)
 */
void psort_u256_index(uint32_t* idx, uint32_t* tmp,
                      const psort_u256_t* keys, int n)
{
    pipe_sort_u256_index_radix8_fixed(idx, tmp, (const u256*)keys, n);
}
