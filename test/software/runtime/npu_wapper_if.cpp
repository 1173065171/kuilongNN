#ifndef NPU_WRAPPER_HPP
#define NPU_WRAPPER_HPP

#include "common/common.hpp"

#include "matrix/gemm.hpp"
#include "matrix/fc.hpp"
#include "matrix/matmul.hpp"
#include "vector/relu.hpp"
#include "vector/vecadd.hpp"
#include "vector/vecmul.hpp"
#include "vector/vecreducesum.hpp"
#include "vector/softmax.hpp"


extern "C" {
    // 为LLVM IR提供的C接口
    int8_t* Kui_Gemm(int8_t* MA, int M1, int K1, int8_t* MB, int K2, int N2, int8_t* MC, int N3, int Div, int DivN, int clip_min, int clip_min_n, int clip_max, int clip_max_n) {
        int8_t *ptr_a = (int8_t*)MA;
        int8_t *ptr_b = (int8_t*)MB;
        int8_t *ptr_c = (int8_t*)MC;
        int cutbit = Div;

        ACENN_DEBUG("Get in Gemm");
        ACENN_DEBUG("malloc size-- M:%d, K:%d, N:%d", M1, K1, N2);
        // 调用模板函数 - 这里使用int8_t作为示例
        int8_t *result = Fc(ptr_a, ptr_b, ptr_c, M1, K1, N2, cutbit);

        return result;
    }

    int8_t* Kui_FC(int8_t* MA, int M1, int K1, int8_t* MB, int K2, int N2, int Div, int DivN, int clip_min, int clip_min_n, int clip_max, int clip_max_n) {
        int cutbit = Div;

        // 调用模板函数 - 这里使用int8_t作为示例
        ACENN_DEBUG("Get in FC");
        ACENN_DEBUG("malloc size-- M:%d, K:%d, N:%d", M1, K1, N2);
        int8_t *result = Gemm(MA, MB, M1, N2, K2, cutbit);

        return result;
    }

    int8_t* Kui_Mul(int8_t* MA, int M1, int N1, int K1, int8_t* MB, int M2, int N2, int K2, int cutbit) {

        // 调用模板函数 - 这里使用int8_t作为示例
        ACENN_DEBUG("Get in VecMul");
        ACENN_DEBUG("malloc size-- M:%d, K:%d, N:%d", M1, K1, N1);
        int8_t *result = VecMul(MA, MB, M1, K1, N1);

        return result;
    }

    int8_t* Kui_ReduceSum(int8_t* i_matrix, int M, int K, int N, int axes, int axesn) {
        ACENN_DEBUG("Get in VecMul");
        ACENN_DEBUG("malloc size-- M:%d, N:%d", M, N);
        int8_t *result = CpuReduceSum_K(i_matrix, M, K, N);

        return result;
    }

    int8_t* Kui_Relu(int8_t* i_matrix, int M, int N) {

        int8_t *result = Relu(i_matrix, M, N);
        
        return result;
    }

    int8_t* Kui_Softmax(int8_t* input, int M, int N) {

        Softmax8(input, M, N);

        return input;
    }

    int8_t* Kui_Concat(int8_t* input, int m, int n) {

        return input;
    }

    int8_t* Concat3(int8_t* input1, int8_t* input2, int8_t* input3) {

        int8_t *result = input1;
        return result;
    }

    int8_t* Kui_Add(int8_t* input, int m, int n, int8_t* bias,int n2) {

        int8_t *result = VecAdd(input, bias, m, n);
        return result;
    }
}

#endif
