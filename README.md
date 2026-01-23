# PiedPiperSort
### Pipe Sort (Radix-Branch)

**PiedPiperSort** is a high-performance sorting algorithm optimized for **wide integers**
(128-bit, 256-bit, or larger).

At its core is **Pipe Sort (Radix-Branch)** — an MSB-first, bitwise sorting strategy that
guides numbers through “pipes” based on their bits, **skipping identical high bits**
to avoid unnecessary work.

This approach is especially effective for **large datasets**, **multi-limb integers**,
and **skewed distributions**, where traditional comparison-based sorting becomes costly.

---

## Features

- Adaptive **bitwise partitioning** (most-significant-bit first)
- Designed for **128-bit and 256-bit integers** (multi-limb arrays)
- Skips identical high bits for **reduced recursion**
- Performs well on **random and skewed datasets**
- Naturally **parallelizable** (independent pipes)
- Fully **correct and benchmark-tested**

---

## Time Complexity

| Case | Complexity | Notes |
|-----|------------|-------|
| Worst case | **O(N × B)** | B = total number of bits |
| Adaptive / best case | **O(N × L_actual)** | Only differing bits are examined |
| Space | **O(B)** | Recursion depth + small buffers |

> Pipe Sort is linear in the number of bits examined, making it highly competitive
> against `std::sort` for wide integers.

---

## Use Cases

- Sorting **128-bit / 256-bit integers**
- Cryptographic hashes and blockchain addresses
- Large **multi-limb numeric datasets**
- Data with **shared high bits** (IDs, timestamps, IP ranges)
- Scientific computing and data analytics

---

## Installation

```bash
git clone https://github.com/perke0/PiedPiperSort.git
cd PiedPiperSort
g++ -O3 -march=native main.cpp pipe_sort.cpp -o piedpipersort
./piedpipersort
```

---

## License

Apache 2.0 License @ Matija Perovic

