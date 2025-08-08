// ----- kuiloong
#include "kuiloong.hpp"


// ----- component
#include "stdint.h"
#include "string.h"
#include "stdlib/printf.hpp"
#include "stdlib/malloc.hpp"


#define Printf printf_

int main()
{ 
	ACENN_DEBUG("kuiloongNN main function started!");
	malloc_initial();
	int8_t  *input_matrix = (int8_t *)malloc(8 * 400 * sizeof(int8_t));
	int8_t  *output_matrix = (int8_t *)malloc(64 * 128 * sizeof(int8_t));

	// main graph
	output_matrix = main_graph(
		input_matrix,
		KuiGemm_model_dense_MatMul_model_dense_Relu_model_dense_BiasAdd_Gemm__142_const_fold_opt__359_quant_1
	);

	asm("ebreak;"); // 中断，停止运行
}



