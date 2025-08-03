#ifndef __CPU_BENCHMARK_HPP__
#define __CPU_BENCHMARK_HPP__

#include "stdint.h"
#include "stdlib/printf.hpp"
#include "common/xcvnn.hpp"

#include "matrix/gemm.hpp"
#include "scalar/matrix_multiply_scalar.hpp"


void test_component();

void test_gemm_proc();

void test_fc_proc();

#endif
