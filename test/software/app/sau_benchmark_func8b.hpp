#ifndef __SAU_BENCHMARK_8X8_HPP__
#define __SAU_BENCHMARK_8X8_HPP__

#include "stdint.h"
#include "stdlib/printf.hpp"
#include "common/xcvnn.hpp"
#include "stdlib/malloc.hpp"

void matmul_multi8(int8_t *input_blockA, int8_t *input_blockB,
                   int8_t *output_blockD, int M, int K, int N);

#endif