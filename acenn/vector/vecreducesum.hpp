//////////////////////////////////////////////////////////////////////////////////
// Author:          yintao liu - berialest@shu.edu.cn                           //
//                                                                              //
// Additional contributions by:                                                 //
//                                                                              //
// File Name:       standard_conv2d_scalar.hpp                                  //
// Project Name:    ACENN                                                       //
// Language:        cpp                                                         //
//                                                                              //
// Description:     Standard convolution                                        //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#ifndef __VECTOR_REDUCESUM_HPP__
#define __VECTOR_REDUCESUM_HPP__

#include "stdint.h"
#include "common/common.hpp"
#include "common/xcvnn.hpp"


int8_t* VecReduceSum(int8_t *input_block, int M, int K, int N)
{
    ACENN_DEBUG("Get in VecAdd");
    ACENN_DEBUG("malloc size-- M:%d, K:%d", M, K);
    int8_t *output_vector = (int8_t*)malloc(M*K * sizeof(int8_t));
    
    uint32_t raddr1 = (uint32_t)(uintptr_t)input_block;
    uint32_t waddr = (uint32_t)(uintptr_t)output_vector;

    __kuiloong_ace_vsetcsr( 
                            SCALAR_EN(0) | 
                            WRITE_BACK(1) | 
                            CLEAR_OB(1) | 
                            MODE(CFG_I8) | 
                            SHIFT(0) | 
                            RESET(0), VEUCFG);
    __kuiloong_ace_vsetcsr(8*M*K, VEUVLEN);
    __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
    __kuiloong_ace_vsetcsr(waddr, VEUWADDR);
    __kuiloong_ace_vredsum(raddr1);

    free(input_block);
    return output_vector;
}


// 对M维求和，输出K×N
int8_t* CpuReduceSum_M(int8_t *input, int M, int K, int N) {
    int8_t *output = (int8_t*)malloc(K * N * sizeof(int8_t));
    if (!output) return nullptr;
    for (int j = 0; j < K; ++j) {
        for (int k = 0; k < N; ++k) {
            int8_t sum = 0;
            for (int i = 0; i < M; ++i) {
                sum += input[i * K * N + j * N + k];
            }
            output[j * N + k] = sum;
        }
    }
    free(input);
    return output;
}

// 对K维求和，输出M×N
int8_t* CpuReduceSum_K(int8_t *input, int M, int K, int N) {
    int8_t *output = (int8_t*)malloc(M * N * sizeof(int8_t));
    if (!output) return nullptr;
    for (int i = 0; i < M; ++i) {
        for (int k = 0; k < N; ++k) {
            int8_t sum = 0;
            for (int j = 0; j < K; ++j) {
                sum += input[i * K * N + j * N + k];
            }
            output[i * N + k] = sum;
        }
    }
    free(input);
    return output;
}

// 对N维求和，输出M×K
int8_t* CpuReduceSum_N(int8_t *input, int M, int K, int N) {
    int8_t *output = (int8_t*)malloc(M * K * sizeof(int8_t));
    if (!output) return nullptr;
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < K; ++j) {
            int8_t sum = 0;
            for (int k = 0; k < N; ++k) {
                sum += input[i * K * N + j * N + k];
            }
            output[i * K + j] = sum;
        }
    }
    free(input);
    return output;
}


#endif
