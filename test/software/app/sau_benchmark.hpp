#ifndef __SAU_BENCHMARK_HPP__
#define __SAU_BENCHMARK_HPP__

#include "stdint.h"
#include "stdlib/printf.hpp"
#include "common/xcvnn.hpp"

void sau_benchmark(int8_t *input_block1, int8_t *input_block2, int8_t *output_block3, int8_t *input_block4);

void minimum_matmul_single_1(int8_t *input_block1,
                           int8_t *input_block2,
                           int8_t *output_block3);
						   
void minimum_matmul_single_2(int8_t *input_block1,
                           int8_t *input_block2,
                           int8_t *output_block3);

void minimum_matmul_single(int8_t *input_block1,
                           int8_t *input_block2,
                           int8_t *output_block3);

void minimum_pwconv_single(int8_t *input_block1,
                           int8_t *input_block2,
                           int8_t *output_block3);

void minimum_conv_single(int8_t *input_block1,
                         int8_t *input_block2,
                         int8_t *output_block3,
                         int8_t *input_block4);

#endif