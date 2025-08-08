#include "matrix/gemm.hpp"
#include "matrix/fc.hpp"
#include "vector/relu.hpp"
#include "vector/vecadd.hpp"
#include "vector/vecmul.hpp"
#include "vector/softmax.hpp"

extern "C" {
    // 为LLVM IR提供的C接口
    int Kui_Gemm(int ma, int mb, int M, int N, int K, int cutbit) {
        int8_t *ptr_a = (int8_t*)ma;
        int8_t *ptr_b = (int8_t*)mb;

        // 调用模板函数 - 这里使用int8_t作为示例
        int8_t *result = Gemm(ptr_a, ptr_b, M, N, K, cutbit);

        return (result != nullptr) ? 0 : -1;  // 返回成功/失败状态
    }

    int Kui_Fc(int ma, int mb, int mc, int M, int N, int K, int cutbit) {
        int8_t *ptr_a = (int8_t*)ma;
        int8_t *ptr_b = (int8_t*)mb;
        int8_t *ptr_c = (int8_t*)mc;

        // 调用模板函数 - 这里使用int8_t作为示例
        int8_t *result = Fc(ptr_a, ptr_b, ptr_c, M, N, K, cutbit);

        return (result != nullptr) ? 0 : -1;  // 返回成功/失败状态
    }

    int Kui_Relu(int i_matrix, int M, int N) {
        int8_t *ptr_a = (int8_t*)i_matrix;

        int8_t *output = Relu(ptr_a, M, N);
        return (output != nullptr) ? 0 : -1;
    }

    int Kui_Softmax(int input, int M, int N) {
        int8_t *ptr_a = (int8_t*)input;

        Softmax8(ptr_a, M);
        return input;
    }

    int Kui_Concat(int input, int m, int n) {


        return input;
    }
}