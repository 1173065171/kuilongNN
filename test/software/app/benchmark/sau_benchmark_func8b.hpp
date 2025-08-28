#ifndef __SAU_BENCHMARK_8X8_HPP__
#define __SAU_BENCHMARK_8X8_HPP__

#include "stdint.h"
#include "stdlib/printf.hpp"
#include "common/xcvnn.hpp"
#include "stdlib/malloc.hpp"

void matmul_multi8(int8_t *input_blockA, int8_t *input_blockB,
                   int8_t *output_blockD, int M, int K, int N);

void fc_multi8(int8_t *input_blockX, int8_t *input_blockW,
               int8_t *intput_bias, int8_t *output_blockY,
               int B, int Di, int Do);

#endif