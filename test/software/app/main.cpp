// component
#include "stdint.h"
#include "string.h"
#include "stdlib/printf.hpp"
#include "stdlib/malloc.hpp"

// testcase
// #include "veu_benchmark.hpp"
// #include "sau_benchmark.hpp"
// #include "sau_benchmark_8x8.hpp"
// #include "sau_benchmark_8x8_16b.hpp"
// #include "sau_benchmark_8x8_16b.hpp"


#define Printf printf_


long insn()
{
	int insns;
	asm volatile ("rdinstret %0" : "=r"(insns));
	return insns;
}


void test_component(){
	ACENN_DEBUG("hello kuiloong!");
	test_printf_();
	test_malloc();
	test_malloc_boundary();
	ACENN_DEBUG("test component over!");
}

extern bool matmul_multi8_test();
extern bool matmul_multi8_single_test();
extern bool matmul_multi16_single_test();
extern bool matmul_multi16_test();

int main()
{
	long BeginInsn, EndInsn;
	BeginInsn = insn();

	malloc_initial();
	// int8_t  *matrix1 = (int8_t  *)malloc(16384 * sizeof(int8_t));
	// int8_t  *matrix2 = (int8_t  *)malloc(16384 * sizeof(int8_t));
	// int8_t  *matrix3 = (int8_t  *)malloc(16384 * sizeof(int8_t));
	// int8_t  *matrix4 = (int8_t  *)malloc(16384 * sizeof(int8_t));
	// int16_t *matrix1 = (int16_t *)malloc(1024 * sizeof(int16_t));
	// int16_t *matrix3 = (int16_t *)malloc(1024 * sizeof(int16_t));
	// int16_t  *matrix4 = (int16_t  *)malloc(1024 * sizeof(int16_t));

	// test_printf_();
	// test_malloc();
	// veu_benchmark();
	// matmul_multi8(matrix1, matrix2, matrix3);
	// bool pass = matmul_multi8_test();
	// minimum_matmul_multi_8x8_16b(matrix1, matrix2, matrix3);
	// matmul_multi16_single_test();
	matmul_multi16_single_test();
	// bool pass = matmul_multi8_single_test();
	// bool pass = matmul_multi8_test();
	// minimum_matmul_multi_8x8(matrix1, matrix2, matrix3);
	// minimum_matmul_multi_8x8(matrix1, matrix2, matrix3);
	// sau_benchmark(matrix1, matrix2, matrix3, matrix4);
	// sau_benchmark_8x8(matrix1, matrix2, matrix3, matrix4);
	// sau_benchmark_8x8_16b(matrix1, matrix2, matrix3, matrix4);

	EndInsn = insn();
	ACENN_DEBUG("test insn: %ld", EndInsn - BeginInsn);

	asm("ebreak;"); // 中断，停止运行
}



