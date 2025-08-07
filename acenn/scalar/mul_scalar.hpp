#ifndef __MUL_SCALAR_HPP__
#define __MUL_SCALAR_HPP__

#include "stdint.h"
#include "common/common.hpp"

// ---------------------
// Mul
// ---------------------
template<typename T>
acenn_matrix<T>* mul_scalar(acenn_matrix<T>* a, acenn_matrix<T>* b) {
    if (a == nullptr || b == nullptr || a->matrix == nullptr || b->matrix == nullptr) {
        printf_("ERROR: input feature map is null!\n");
        asm("ebreak;");
        return nullptr;
    }

    if (a->channels != b->channels || a->rows != b->rows || a->cols != b->cols) {
        printf_("ERROR: mismatched dimensions in Mul!\n");
        asm("ebreak;");
        return nullptr;
    }

    acenn_matrix<T>* output = static_cast<acenn_matrix<T>*>(malloc(sizeof(acenn_matrix<T>)));
    if (output == nullptr) {
        printf_("ERROR: failed to allocate output!\n");
        asm("ebreak;");
        return nullptr;
    }

    output->dims = a->dims;
    output->channels = a->channels;
    output->rows = a->rows;
    output->cols = a->cols;
    output->matrix = create_tensor<T>(output->dims, output->channels, output->rows, output->cols);
    if (output->matrix == nullptr) {
        free(output);
        printf_("ERROR: failed to allocate output->matrix!\n");
        asm("ebreak;");
        return nullptr;
    }

    // process
    uint32_t total_elements = a->channels * a->rows * a->cols;
    for (uint32_t i = 0; i < total_elements; ++i) {
        output->matrix[i] = a->matrix[i] * b->matrix[i];
    }

    return output;
}

#endif
