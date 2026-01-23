# Pied Piper Sort or Pipe Sort

**Pipe Sort** is a high-performance, non-comparison sorting algorithm designed for **wide integers** (such as 128-bit, 256-bit, or multi-limb numbers).

Instead of comparing numbers against each other, Pipe Sort works by **examining bits from the most significant side**, grouping numbers step by step until the correct order naturally emerges.

This makes it especially effective for:

* Large integers (`u128`, cryptographic keys, hashes)
* Datasets with shared prefixes or skewed distributions
* Situations where comparison-based sorting becomes expensive

---

## How It Works (Intuition)

Think of Pipe Sort as guiding numbers through a system of pipes:

1. Look at the **current most significant bit** of all numbers
2. Split them into two groups:

   * Bit `0` → left pipe
   * Bit `1` → right pipe
3. If all numbers share the same bit, **skip it** and move on
4. Repeat for the next bit, recursively
5. Stop when groups are fully determined

No direct comparisons are ever made — numbers are ordered purely by their binary structure.

---

## Key Features

* **MSB-first bitwise partitioning**
* **Adaptive bit skipping** for identical prefixes
* **No comparisons** (comparison-free sorting)
* Optimized for **multi-limb integers**
* Cache-friendly memory access patterns
* Works well on **random and skewed datasets**
* Fully benchmarked and correctness-tested

---

## Time Complexity

Let:

* `n` = number of elements
* `w` = number of bits per element (e.g. `128` for `u128`)
* `k` = number of *effective* bits actually examined

| Case         | Time Complexity                  |
| ------------ | -------------------------------- |
| Best case    | **O(n)** (large shared prefixes) |
| Average case | **O(n · k)** with `k ≪ w`        |
| Worst case   | **O(n · w)**                     |

In practice, Pipe Sort often behaves close to **linear time** for real-world wide-integer data.

---

## Space Complexity

* Recursion depth: **O(w)** (or **O(1)** if iterative)
* Auxiliary memory:

  * In-place partitioning: **O(1)**
  * Out-of-place partitioning: **O(n)**

---

## Comparison with Other Algorithms

| Algorithm     | Time Complexity | Comparisons | Notes               |
| ------------- | --------------- | ----------- | ------------------- |
| `std::sort`   | O(n log n)      | Yes         | General-purpose     |
| Radix (LSD)   | O(n · w)        | No          | Fixed passes        |
| **Pipe Sort** | **O(n · k)**    | No          | Adaptive, MSB-first |
| QuickSort     | O(n²) worst     | Yes         | Branch-heavy        |

Pipe Sort excels when dealing with **wide integers** and datasets with meaningful binary structure.

---

## When to Use Pipe Sort

Pipe Sort is a strong choice when:

* Sorting `u128` or larger integer types
* Working with cryptographic or hash-based keys
* Data has shared prefixes or skewed distributions
* Comparison cost dominates performance

It is **not** intended as a universal replacement for `std::sort` on small primitive types.

---

## Status

* Correctness verified
* Benchmarked against `std::sort`
* Implementations available in **C and C++**
* Designed for extensibility (parallel versions possible)

---

## License

See `LICENSE` file for details.

