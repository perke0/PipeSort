# Pipe Sort

**Pipe Sort (Radix-Branch)** is a high-performance sorting algorithm optimized for **wide integers** (128-bit, 256-bit, or larger).  
It sorts numbers **bit by bit**, starting from the most significant bit, and **skips identical high bits** for faster performance.  

This algorithm is especially useful for **large datasets with multi-limb integers**, skewed datasets, or cryptographic keys where standard comparison-based sorting becomes expensive.

---

## Features

- Adaptive **bitwise partitioning** (MSB first)  
- Supports **128-bit and 256-bit integers** (multi-limb arrays)  
- Skips identical high bits for **faster recursion**  
- Works with **random or skewed datasets**  
- Can be extended to **parallel execution** for large arrays  
- Fully **correct and benchmark-tested**  

---

## Time Complexity

| Case                  | Time Complexity       | Notes |
|-----------------------|--------------------|-------|
| Worst (all bits differ) | O(N × B)           | B = total bits per number |
| Best (many identical MSBs) | O(N × L_actual) | L_actual = number of differing bits examined |
| Space                 | O(B) recursion + small insertion buffers | Efficient |

> Linear in the number of bits examined — faster than `std::sort` for wide integers.

---

## Use Cases

- Sorting **128-bit or 256-bit integers**  
- Sorting **cryptographic hashes or blockchain addresses**  
- Large **multi-limb integer datasets**  
- Datasets with **repeated high bits** (timestamps, IDs, IP addresses)  
- Scientific computing and data analytics requiring **wide number sorting**

---

## Installation

1. Clone the repository:

```bash
git clone https://github.com/perke0/PipeSort.git
cd PipeSort
```

2. Compile:

```bash
g++ -O3 -march=native main.cpp pipe_sort.cpp -o pipe_sort
```

3. Usage:

```bash
./pipe_sort
```
## The benchmark supports:
- Random and skewed datasets
- N = 10,000 → 1,000,000 elements
- 128-bit (2 limbs) and 256-bit (4 limbs) numbers

Example Benchmark Output:

```yaml
=== Benchmark N=500000 LIMBS=2 (random) ===
Pipe Sort time: 0.056 s
std::sort time: 0.109 s
Speedup (std/pipe): 1.93x
Correct: YES

=== Benchmark N=500000 LIMBS=2 (skewed) ===
Pipe Sort time: 0.088 s
std::sort time: 0.125 s
Speedup (std/pipe): 1.42x
Correct: YES
```
- Pipe Sort is faster than std::sort on large datasets and maintains full correctness.

## How Pipe Sort Works

1. Partition by MSB: numbers are separated into two “pipes” depending on the current bit.
2. Recursive sorting: each pipe is sorted on the next lower bit.
3. Adaptive bit-skipping: if all numbers in a pipe share the same high bits, those bits are skipped to save work.
4. Leaf nodes: once a small bucket is reached, a simple insertion sort finalizes ordering.
5. Merge-free: partitioning preserves order, so no additional merging is required.

## Visualization

For 128-bit integers (2 limbs), imagine a recursion tree:

```arduino
[All numbers]
 ├─ MSB=0 → [pipe0]
 │      ├─ next differing bit → [subpipes]
 │      └─ ...
 └─ MSB=1 → [pipe1]
        ├─ next differing bit → [subpipes]
        └─ ...
```
- Only differing bits trigger recursion, which is the key to Pipe Sort’s speed.
- For 256-bit numbers, this scales naturally: high bits often identical → large portions of the dataset are processed with minimal bit checks.

## Notes and Tips

- Threshold for small buckets can be tuned (16–64 elements) for better performance.
- Can be parallelized, sorting each pipe independently for huge arrays.
- Extremely useful for sorting numbers that are expensive to compare fully (multi-limb, 128–256 bits).
- For small N or narrow integers, std::sort may remain faster due to low overhead.

## License

MIT License @ Matija Perovic

## Acknowledgements

Inspired by bitwise radix sorting techniques, optimized for multi-limb integers.
Pipe Sort combines the efficiency of radix-style partitioning with adaptive skipping to achieve high performance on modern datasets.

## Contributing
Contributions are welcome!
Please submit changes via Pull Requests — direct pushes to `main` are disabled.


