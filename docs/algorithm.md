# PipeSort — Algorithm Details

This document explains **PipeSort** for readers who want the mechanics, cost model, and design tradeoffs.

---

## Motivation

For very wide keys (128-bit and beyond), comparison cost grows with key width:

- Each comparison is **O(w)** where `w` is the key width in bits
- Comparison based sorts typically do **O(n log n)** comparisons
- So total work often behaves like **O(n log n · w)**

PipeSort avoids element to element comparisons and instead orders elements by their **bit structure**.

---

## Core idea

PipeSort is an **MSB first bit partitioning** algorithm:

1. Choose a bit position `b` (starting near the highest set bit across all keys)
2. Partition the array into `bit(b)=0` and `bit(b)=1`
3. Recurse into each partition with `b-1`

### Prefix skipping

If all keys in the current range share the same bit value at `b`, that level is **skipped** and the algorithm continues with `b-1`.

This is what makes PipeSort **data adaptive**: it only explores the decision tree levels that actually matter for the given input.

---

## Conceptual model

PipeSort can be viewed as constructing a **partial binary trie** over the key bits, but implemented using in-place partitioning rather than explicit node allocation.

Compared to a pointer-based trie, this keeps:
- memory predictable
- cache behavior better
- implementation simpler in C

---

## Small range cutoff

Real implementations often switch to a simple sort (commonly insertion sort) for small partitions to reduce overhead and improve cache locality.

This changes constants, not asymptotic behavior.

---

## Time complexity

Let:
- `n` = number of elements
- `w` = key width in bits
- `k` = number of effective bit levels examined (`k ≤ w`)

| Case | Complexity |
|---|---|
| Best | **O(n)** when large prefixes are shared |
| Average | **O(n · k)** (often `k ≪ w` on structured data) |
| Worst | **O(n · w)** when bits are maximally distinguishing |

---

## Space complexity

| Component | Cost |
|---|---|
| In-place partitioning | **O(1)** auxiliary |
| Buffered variant (optional) | **O(n)** auxiliary |
| Recursion depth | **O(w)** (can be made iterative) |

---

## Comparisons to other families

### vs `std::sort`

- `std::sort` (introsort) is an excellent general purpose baseline
- For wide keys, comparison cost dominates, and the number of comparisons stays ~`n log n`
- PipeSort does **no key comparisons** and can benefit when the data shares prefixes

### vs QuickSort

- QuickSort is fast for small keys and typical inputs
- Still relies on comparisons (expensive for wide keys)
- Worst-case `O(n^2)` exists (mitigated in many practical implementations)

### vs Radix sort

- Radix sort does a fixed number of passes (depending on digit size)
- PipeSort effectively does only the passes it needs (adaptive)

---

## Practical use cases

- Cryptographic hashes (u256 / u512)
- Content-addressed identifiers
- Databases with wide primary keys
- Blockchain / Merkle-tree workloads

---

## Figures and benchmarks

If this repository contains benchmark plots, they typically live in:

- `assets/graphs/compare_scaling_n.png`
- `assets/graphs/compare_scaling_w.png`
- `assets/graphs/compare_memory.png`

They are referenced from the landing README.

---

## License

Apache License 2.0

