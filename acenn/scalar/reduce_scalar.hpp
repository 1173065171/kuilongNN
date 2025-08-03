#ifndef __REDUCE_SCALAR_HPP__
#define __REDUCE_SCALAR_HPP__

#include "stdint.h"
#include "common/common.hpp"


template<typename T>
T reduce_sum_scalar(acenn_matrix<T>* input) {
    if (input == nullptr || input->matrix == nullptr) {
        printf_("ERROR: input feature map is null!\n");
        asm("ebreak;");
        return T(0);
    }
    uint32_t total_elements = input->channels * input->rows * input->cols;
    T sum = std::accumulate(input->matrix, input->matrix + total_elements, T(0));
    return sum;
}

// ---------------------
// ReduceProd
// ---------------------
template<typename T>
T reduce_prod_scalar(acenn_matrix<T>* input) {
    if (input == nullptr || input->matrix == nullptr) {
        printf_("ERROR: input feature map is null!\n");
        asm("ebreak;");
        return T(1);
    }
    uint32_t total_elements = input->channels * input->rows * input->cols;
    T prod = std::accumulate(input->matrix, input->matrix + total_elements, T(1), std::multiplies<T>());
    return prod;
}

// ---------------------
// Round
// ---------------------
template<typename T>
acenn_matrix<T>* round_scalar(acenn_matrix<T>* input) {
    if (input == nullptr || input->matrix == nullptr) {
        printf_("ERROR: input feature map is null!\n");
        asm("ebreak;");
        return nullptr;
    }
    acenn_matrix<T>* output = static_cast<acenn_matrix<T>*>(malloc(sizeof(acenn_matrix<T>)));
    if (output == nullptr) {
        printf_("ERROR: failed to allocate output!\n");
        asm("ebreak;");
        return nullptr;
    }
    output->dims = input->dims;
    output->channels = input->channels;
    output->rows = input->rows;
    output->cols = input->cols;
    output->matrix = create_tensor<T>(output->dims, output->channels, output->rows, output->cols);
    if (output->matrix == nullptr) {
        free(output);
        printf_("ERROR: failed to allocate output->matrix!\n");
        asm("ebreak;");
        return nullptr;
    }
    uint32_t total_elements = input->channels * input->rows * input->cols;
    for (uint32_t i = 0; i < total_elements; ++i) {
        output->matrix[i] = std::round(input->matrix[i]);
    }
    return output;
}

#endif
