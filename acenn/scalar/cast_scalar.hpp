#ifndef __CAST_SCALAR_HPP__
#define __CAST_SCALAR_HPP__

#include "stdint.h"
#include "common/common.hpp"
#include <cmath>
#include <algorithm>

// ---------------------
// Cast (float -> int)
// ---------------------
template<typename T, typename U>
acenn_matrix<U>* cast_scalar(acenn_matrix<T>* input) {
    if (input == nullptr || input->matrix == nullptr) {
        printf_("ERROR: input feature map is null!\n");
        asm("ebreak;");
        return nullptr;
    }
    acenn_matrix<U>* output = static_cast<acenn_matrix<U>*>(malloc(sizeof(acenn_matrix<U>)));
    if (output == nullptr) {
        printf_("ERROR: failed to allocate output!\n");
        asm("ebreak;");
        return nullptr;
    }
    output->dims = input->dims;
    output->channels = input->channels;
    output->rows = input->rows;
    output->cols = input->cols;
    output->matrix = create_tensor<U>(output->dims, output->channels, output->rows, output->cols);
    if (output->matrix == nullptr) {
        free(output);
        printf_("ERROR: failed to allocate output->matrix!\n");
        asm("ebreak;");
        return nullptr;
    }

    // process
    uint32_t total_elements = input->channels * input->rows * input->cols;
    for (uint32_t i = 0; i < total_elements; ++i) {
        output->matrix[i] = static_cast<U>(input->matrix[i]);
    }
    return output;
}

// ---------------------
// Clip
// ---------------------
template<typename T>
acenn_matrix<T>* clip_scalar(acenn_matrix<T>* input, T min_val, T max_val) {
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

    // process
    uint32_t total_elements = input->channels * input->rows * input->cols;
    for (uint32_t i = 0; i < total_elements; ++i) {
        T val = input->matrix[i];
        output->matrix[i] = std::min(std::max(val, min_val), max_val);
    }
    return output;
}

#endif
