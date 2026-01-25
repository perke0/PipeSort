<p align="center">
  <img src="assets/pipe_sort_visual.png" alt="PipeSort visual flow" width="100%">
</p>

# PipeSort

**PipeSort** is a specialized sorting algorithm for **wide integers** (128-bit and beyond). Instead of comparing keys over and over, PipeSort partitions by **bits (MSB → LSB)** and **skips shared prefixes**, which can make it very fast on hash like / ID like data.

> PipeSort is **not** a universal replacement for `std::sort`. Use it when keys are wide and comparisons are expensive.

---

## PipeSort vs `std::sort` vs QuickSort (non-expert view)

| Algorithm | What it does (simple) | Usually best for | When it struggles |
|---|---|---|---|
| **PipeSort** | Looks at bits and splits into 0/1 groups (no compares) | **Wide keys** (u128/u256/u512), shared prefixes | Small keys (`u32/u64`), tiny arrays, adversarial bit patterns |
| **`std::sort`** | General purpose comparison sort | Most programs, small/medium keys | Wide keys where each comparison is expensive |
| **QuickSort** | Fast comparisons + partitioning | Small keys, good cache behavior | Worst case can be bad; still comparison heavy |

---

## When should you try PipeSort?

Use PipeSort if you sort:
- cryptographic hashes / content IDs / UUID-like keys
- `u128`, `u256`, `u512` (or wider)
- datasets with many shared prefixes or duplicates

Don’t use PipeSort for:
- small primitive keys (`u32`, `u64`)
- very small `n`

---

## Quick build & run (GCC)

```bash
make
gcc -O3 -Iinclude test.c libpipesort.a -o test
./test
```

---

## Performance graphs

These are generated from the benchmarks in this repo:

- **Scaling with input size:** `assets/graphs/compare_scaling_n.png`
- **Scaling with key width:** `assets/graphs/compare_scaling_w.png`
- **Auxiliary memory usage:** `assets/graphs/compare_memory.png`

<p align="center">
  <img src="assets/graphs/compare_scaling_n.png" alt="Scaling with input size" width="100%">
</p>

---

## C API

Single header:

```c
#include <pipesort/pipesort.h>
```

Main entry points:
- `psort_u()` — universal multi-limb sort
- `psort_u128()` — in place u128 sort
- `psort_u256_index()` / `psort_u512_index()` — index sorts for wide keys

---

## Learn more

- Expert write-up: **`docs/algorithm.md`**

---

## License

Apache License 2.0

