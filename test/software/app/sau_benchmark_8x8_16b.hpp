#ifndef __SAU_BENCHMARK_8X8_16B_HPP__
#define __SAU_BENCHMARK_8X8_16B_HPP__

#include "stdint.h"
#include "stdlib/printf.hpp"
#include "common/xcvnn.hpp"

void sau_benchmark_8x8_16b(int16_t *input_block1, int8_t *input_block2, int16_t *output_block3, int16_t *input_block4);
void minimum_matmul_multi_8x8_16b(int16_t *input_block1,
                           int8_t *input_block2,
                           int16_t *output_block3);
						   

void minimum_matmul_single_8x8_16b(int16_t *input_block1,
                           int8_t *input_block2,
                           int16_t *output_block3);

void minimum_pwconv_single_8x8_16b(int16_t *input_block1,
                           int8_t *input_block2,
                           int16_t *output_block3);

void minimum_conv_single_8x8_16b(int16_t *input_block1,
                         int8_t *input_block2,
                         int16_t *output_block3,
                         int16_t *input_block4);

void minimum_dwconv_single_8x8_16b(int16_t *input_block1,
                         int8_t *input_block2,
                         int16_t *output_block3,
                         int16_t *input_block4);
#endif