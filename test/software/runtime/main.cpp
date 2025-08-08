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
	int8_t  *input_matrix = (int8_t *)malloc(32 * 64 * sizeof(int8_t));
	int8_t  *output_matrix = (int8_t *)malloc(64 * 128 * sizeof(int8_t));

	// main graph
	int result = main_graph(m1);

	asm("ebreak;"); // 中断，停止运行
}



