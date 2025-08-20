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
    // 输入0: model_concatenate_concat
    // 原始维度: 8 x 396, 原始大小: 3168
    // 对齐维度: 8 x 400, 对齐大小: 3200 (16字节对齐)
    int8_t *input_0 = (int8_t *)malloc(9600 * sizeof(int8_t));
    
    // 输出矩阵分配
    // 输出矩阵 - Relu__58_0
    // 原始维度: 8 x 64, 原始大小: 512
    // 对齐维度: 8 x 64, 对齐大小: 512 (16字节对齐)
    int8_t *output_matrix = (int8_t *)malloc(512 * sizeof(int8_t));

    ACENN_DEBUG("kuiloongNN main_graph() started!");
    // 执行主计算图
    output_matrix = main_graph(input_0);

    ACENN_DEBUG("kuiloongNN main_graph() completed!");
    
    // 释放内存
    free(input_0);
    free(output_matrix);
    
    ACENN_DEBUG("kuiloongNN main_graph() over!");

    asm("ebreak;"); // 中断，停止运行
    return 0;
}