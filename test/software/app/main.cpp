// component
#include "stdint.h"
#include "string.h"
#include "stdlib/printf.hpp"
#include "stdlib/malloc.hpp"
#include "common/xcvnn.hpp"

// ----- testcase
// #include "cpu_benchmark.hpp"
// #include "veu_benchmark.hpp"
#include "benchmark/sau_benchmark_func8b.hpp"
// #include "sau_benchmark_8x8.hpp"
// #include "sau_benchmark_8x8_16b.hpp"


#define Printf printf_


long insn()
{
	int insns;
	asm volatile ("rdinstret %0" : "=r"(insns));
	return insns;
}

long time()
{
	int cycles;
	asm volatile ("rdcycle %0" : "=r"(cycles));
	return cycles;
}

void test_builtin_norun(){
	int a,b,c,s;
	a = 0x1234;
	b = 0x5678;
	c = 0xFFFFFFFF;

	__kuiloong_ace_vsetcsr(a, VEUWADDR);
	__kuiloong_ace_vsetcsr(b, VEUCFG);
	__kuiloong_ace_vsetcsr(c, VEUMASK);
	s = __kuiloong_ace_vgetcsr(VEUWADDR);
	if (s != a) {
		ACENN_DEBUG("Read %x", s);
		ACENN_DEBUG("VEUWADDR failed to set %x", a);
		asm("ebreak;");
	}
	s = __kuiloong_ace_vgetcsr(VEUCFG);
	if (s != b) {
		ACENN_DEBUG("Read %x", s);
		ACENN_DEBUG("VEUCFG failed to set %x", b);
		asm("ebreak;");
	}
	s = __kuiloong_ace_vgetcsr(VEUMASK);
	if (s != c) {
		ACENN_DEBUG("Read %x", s);
		ACENN_DEBUG("VEUMASK failed to set %x", c);
		asm("ebreak;");
	}

	int ins1,ins2,ins3,ins4,ins5,ins6,ins7,ins8;
	ins1 = 0x12345678;
	ins2 = 0x23456789;
	ins3 = 0x34567890;
	ins4 = 0x45678901;
	ins5 = 0x56789012;
	ins6 = 0x67890123;
	ins7 = 0x78901234;
	ins8 = 0x1;
	__kuiloong_ace_msetins1(ins1, ins2);
	__kuiloong_ace_msetins2(ins3, ins4);
	__kuiloong_ace_msetins3(ins5, ins6);
	__kuiloong_ace_msetins4(ins7, ins8);
	s = __kuiloong_ace_mgetins4lsb();
	if (s != ins8) {
		ACENN_DEBUG("Read %x", s);
		ACENN_DEBUG("sau start failed to set %x", ins8);
		asm("ebreak;");
	}
}

void test_operator(){
	ACENN_DEBUG("-------- test operator! --------");
	int8_t  *matrix1 = (int8_t  *)malloc(256 * sizeof(int8_t));
	int8_t  *matrix2 = (int8_t  *)malloc(256 * sizeof(int8_t));
	int8_t  *matrix3 = (int8_t  *)malloc(256 * sizeof(int8_t));


	// int8_t  *matrix1 = (int8_t  *)malloc(16384 * sizeof(int8_t));
	// int8_t  *matrix2 = (int8_t  *)malloc(16384 * sizeof(int8_t));
	// int8_t  *matrix3 = (int8_t  *)malloc(16384 * sizeof(int8_t));
	// int8_t  *matrix4 = (int8_t  *)malloc(16384 * sizeof(int8_t));
	// int16_t *matrix1 = (int16_t *)malloc(1024 * sizeof(int16_t));
	// int16_t *matrix3 = (int16_t *)malloc(1024 * sizeof(int16_t));
	// int16_t  *matrix4 = (int16_t  *)malloc(1024 * sizeof(int16_t));

	// veu_benchmark();
	matmul_multi8(matrix1, matrix2, matrix3,16,16,16);
	// bool pass = matmul_multi8_test();
	// matmul_multi8_single_test();
	// matmul_multi8_single_test();
	// fc_multi8_single_test_128_128_128();
	// vector_softmax_single_test();
	// minimum_matmul_multi_8x8_16b(matrix1, matrix2, matrix3);
	// matmul_multi16_single_test();
	// matmul_multi16_single_test();
	// fc_multi8_single_test();
	// ACENN_DEBUG("ENTERN FC MULTI TEST");
	// fc_multi8_single_test_128_128_128();
	// vector_gather_single_test();
	// vector_softmax_single_test();
	// matmul_multi8_test();
	// bool pass = matmul_multi8_single_test();
	// bool pass = matmul_multi8_test();
	// minimum_matmul_multi_8x8(matrix1, matrix2, matrix3);
	// minimum_matmul_multi_8x8(matrix1, matrix2, matrix3);

	free(matrix1);
	free(matrix2);
	free(matrix3);
	ACENN_DEBUG("-------- test operator over! --------");
}

extern bool sau_test();
int main()
{
	long BeginCycles, EndCycles;
	BeginCycles = time();
	malloc_initial();
	ACENN_DEBUG("-------- test benchmark start! --------");

	// printf测试
	// test_printf_();

	// malloc测试
	// test_malloc();

	// built-in测试
	// test_builtin_norun();

	// 算子测试
	// test_operator();
	sau_test();
	// malloc(64);
	// malloc(64);
	// malloc(64);

	// benchmark测试
	// veu_benchmark();
	// sau_benchmark(matrix1, matrix2, matrix3, matrix4);
	// sau_benchmark_8x8(matrix1, matrix2, matrix3, matrix4);
	// sau_benchmark_8x8_16b(matrix1, matrix2, matrix3, matrix4);


	ACENN_DEBUG("-------- test benchmark over! --------");
	EndCycles = time();
	printf_("my running cycles: %ld", EndCycles - BeginCycles);
	asm("ebreak;");
}

