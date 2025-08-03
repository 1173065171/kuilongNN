// ----- component
#include "stdint.h"
#include "string.h"
#include "stdlib/printf.hpp"
#include "stdlib/malloc.hpp"

// ----- testcase
#include "veu_benchmark.hpp"
// #include "sau_benchmark.hpp"
// #include "sau_benchmark_8x8.hpp"
// #include "sau_benchmark_8x8_16b.hpp"
#include "cpu_benchmark.hpp"


#define Printf printf_


long insn()
{
	int insns;
	asm volatile ("rdinstret %0" : "=r"(insns));
	return insns;
}


void test_benchmark(){
	ACENN_DEBUG("-------- hello kuiloong! ----------");
	int8_t  *matrix1 = (int8_t  *)malloc(1024 * sizeof(int8_t));
	int8_t  *matrix2 = (int8_t  *)malloc(1024 * sizeof(int8_t));
	int8_t  *matrix3 = (int8_t  *)malloc(1024 * sizeof(int8_t));
	int8_t  *matrix4 = (int8_t  *)malloc(1024 * sizeof(int8_t));

	// int16_t *matrix1 = (int16_t *)malloc(1024 * sizeof(int16_t));
	// int8_t  *matrix2 = (int8_t  *)malloc(1024 * sizeof(int8_t));
	// int16_t *matrix3 = (int16_t *)malloc(1024 * sizeof(int16_t));
	// int16_t *matrix4 = (int16_t  *)malloc(1024 * sizeof(int16_t));


	veu_benchmark();
	// sau_benchmark(matrix1, matrix2, matrix3, matrix4);
	// sau_benchmark_8x8(matrix1, matrix2, matrix3, matrix4);
	// sau_benchmark_8x8_16b(matrix1, matrix2, matrix3, matrix4);
	ACENN_DEBUG("-------- test benchmark over! --------");
}

void test_operator(){
	ACENN_DEBUG("-------- test operator! --------");
	int8_t  *matrix1 = (int8_t  *)malloc(1024 * sizeof(int8_t));
	int8_t  *matrix2 = (int8_t  *)malloc(1024 * sizeof(int8_t));
	int8_t  *matrix3 = (int8_t  *)malloc(1024 * sizeof(int8_t));
	int8_t  *matrix4 = (int8_t  *)malloc(1024 * sizeof(int8_t));




	free(matrix1);
	free(matrix2);
	free(matrix3);
	free(matrix4);
	ACENN_DEBUG("-------- test operator over! --------");
}



int main()
{ 
	ACENN_DEBUG("kuiloongNN main function started!");
	long BeginInsn, EndInsn;
    // start time
	BeginInsn = insn();
	malloc_initial();

	// test function
    test_benchmark();

    // end time
	EndInsn = insn();
	ACENN_DEBUG("test insn: %ld", EndInsn - BeginInsn);

	asm("ebreak;"); // 中断，停止运行
}



