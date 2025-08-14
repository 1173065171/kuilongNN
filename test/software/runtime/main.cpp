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
	ACENN_DEBUG("kuiloongNN main() started!");
	malloc_initial();

	ACENN_DEBUG("kuiloongNN input/output malloc!");
	int8_t  *input_matrix = (int8_t *)malloc(8 * 64 * sizeof(int8_t));
	int8_t  *output_matrix = (int8_t *)malloc(8 * 12 * sizeof(int8_t));

	ACENN_DEBUG("kuiloongNN main_graph() started!");
	// main graph
	output_matrix = main_graph(
		input_matrix
	);
	free(input_matrix);
    free(output_matrix);
	ACENN_DEBUG("kuiloongNN main_graph() over!");

	asm("ebreak;"); // 中断，停止运行
	return 0;
}



