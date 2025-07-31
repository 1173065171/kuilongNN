#ifndef __VEU_BENCHMARK_HPP__
#define __VEU_BENCHMARK_HPP__

#include "stdint.h"
#include "stdlib/printf.hpp"
#include "common/xcvnn.hpp"
#include "stdlib/malloc.hpp"

void gather_trick(int8_t *input_block, int M, int K, int col, int8_t *output_vector);

#endif