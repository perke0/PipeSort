#pragma once
#include <cstdint>
#include <cstddef>

void pipe_sort(uint64_t* arr, size_t n, size_t limbs, int bit);
int msb_pos(uint64_t x);
int get_max_bit(uint64_t* arr, size_t n, size_t limbs);

