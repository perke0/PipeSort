#include "pipesort/pipesort.h"
#include "pipe_sort_u512_idx_radix8.h"
#include "u512.h"

/* Layout compatibility:
 *   psort_u512 == u512  (w7..w0)
 */
void psort_u512_index(uint32_t* idx, uint32_t* tmp,
                      const psort_u512_t* keys, int n)
{
    pipe_sort_u512_index_radix8_fixed(idx, tmp, (const u512*)keys, n);
}
