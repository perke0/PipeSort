# Pipe Sort (Radix-Branch)

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
git clone https://github.com/yourusername/PipeSort-Radix-Branch.git
cd PipeSort-Radix-Branch

