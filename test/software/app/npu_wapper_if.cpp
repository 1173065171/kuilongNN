#include "matrix/gemm.hpp"
#include "vector/relu.hpp"

extern "C" {
    // 为LLVM IR提供的C接口
    int Kui_Gemm(int m1, int n1, int m2, int n2, int dims, int channels) {
        printf_("C Interface Kui_Gemm called: %d %d %d %d %d %d\n", 
                m1, n1, m2, n2, dims, channels);
        
        // 调用模板函数 - 这里使用int8_t作为示例
        acenn_matrix<int8_t> *result = Kui_Gemm<int8_t>((uint32_t)m1, (uint32_t)n1, 
                                                        (uint32_t)m2, (uint32_t)n2, 
                                                        (uint32_t)dims, (uint32_t)channels);
        
        return (result != nullptr) ? 0 : -1;  // 返回成功/失败状态
    }
    
    int Kui_Relu(int input) {
        printf_("C Interface Kui_Relu called: %d\n", input);
        // 简单的pass through实现
        return input;
    }
}