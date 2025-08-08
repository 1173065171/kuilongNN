#include "matrix/gemm.hpp"
#include "matrix/fc.hpp"
#include "vector/relu.hpp"
#include "vector/vecadd.hpp"
#include "vector/vecmul.hpp"

extern "C" {
    // 为LLVM IR提供的C接口
    int Kui_Gemm(int m1, int n1, int m2, int n2, int dims, int channels) {
        printf_("C Interface Kui_Gemm called: %d %d %d %d %d %d\n", m1, n1, m2, n2, dims, channels);
        int32_t *output_block = (int32_t *)malloc(m1 * n1 * sizeof(int32_t));
        int8_t *ptr_a = (int8_t*)n1;

        for (int i = 0; i < 400 * n1; i++) {
            ACENN_DEBUG("weight[%d] = %hhd", i, ptr_a[i]);
        }
        
        // 调用模板函数 - 这里使用int8_t作为示例
        // acenn_matrix<int8_t> *result = Gemm<int8_t>((uint32_t)m1, (uint32_t)n1, (uint32_t)m2, (uint32_t)n2, (uint32_t)dims, (uint32_t)channels);
        
        // return (result != nullptr) ? 0 : -1;  // 返回成功/失败状态
        return n1;
    }

    // int Kui_FC(int m1, int n1, int m2, int n2, int dims, int channels) {
    //     printf_("C Interface Kui_FC called: %d %d %d %d %d %d\n", m1, n1, m2, n2, dims, channels);
    //     int32_t *output_block = (int32_t *)malloc(m1 * n1 * sizeof(int32_t));

        
    //     // 调用模板函数 - 这里使用int8_t作为示例
    //     acenn_matrix<int8_t> *result = FC<int8_t>((uint32_t)m1, (uint32_t)n1, (uint32_t)m2, (uint32_t)n2, (uint32_t)dims, (uint32_t)channels);

    //     return (result != nullptr) ? 0 : -1;  // 返回成功/失败状态
    // }
    
    int Kui_Relu(int input, int m, int n) {
        printf_("C Interface Kui_Relu called: %d\n", input);
        int32_t *output_block = (int32_t *)malloc(m * n * sizeof(int32_t));

        // Relu(input, output_block);
        return input;
    }

    
}