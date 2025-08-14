#ifndef __REDUCESUM_SCALAR_HPP__
#define __REDUCESUM_SCALAR_HPP__

#include "stdint.h"
#include "common/common.hpp"

// 对M维求和，输出K×N
int8_t* ReduceSum_Scalar_M(int8_t *input, int M, int K, int N) {
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
    return output;
}

// 对K维求和，输出M×N
int8_t* ReduceSum_Scalar_K(int8_t *input, int M, int K, int N) {
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
    return output;
}

// 对N维求和，输出M×K
int8_t* ReduceSum_Scalar_N(int8_t *input, int M, int K, int N) {
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
    return output;
}

#endif