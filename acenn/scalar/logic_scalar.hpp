#ifndef __LOGIC_SCALAR_HPP__
#define __LOGIC_SCALAR_HPP__

#include "stdint.h"
#include "common/common.hpp"
#include <cmath>
#include <algorithm>
#include <numeric> // for accumulate
#include <cstring> // for memcpy

// ---------------------
// Add
// ---------------------
// ... [unchanged code here]

// ---------------------
// Clip
// ---------------------
// ... [unchanged code here]

// ---------------------
// ReduceSum
// ---------------------
// ... [unchanged code here]

// ---------------------
// ReduceProd
// ---------------------
// ... [unchanged code here]

// ---------------------
// Round
// ---------------------
// ... [unchanged code here]

// ---------------------
// Softmax
// ---------------------
// ... [unchanged code here]

// ---------------------
// Concat (axis = channels only)
// ---------------------
template<typename T>
acenn_matrix<T>* concat_channels(acenn_matrix<T>* a, acenn_matrix<T>* b) {
    if (a == nullptr || b == nullptr || a->matrix == nullptr || b->matrix == nullptr) {
        printf_("ERROR: input is null!\n");
        asm("ebreak;");
        return nullptr;
    }
    if (a->dims != b->dims || a->rows != b->rows || a->cols != b->cols) {
        printf_("ERROR: concat only supports matching dims/rows/cols!\n");
        asm("ebreak;");
        return nullptr;
    }
    acenn_matrix<T>* output = static_cast<acenn_matrix<T>*>(malloc(sizeof(acenn_matrix<T>)));
    output->dims = a->dims;
    output->channels = a->channels + b->channels;
    output->rows = a->rows;
    output->cols = a->cols;
    output->matrix = create_tensor<T>(output->dims, output->channels, output->rows, output->cols);
    if (output->matrix == nullptr) {
        free(output);
        printf_("ERROR: concat allocation failed!\n");
        asm("ebreak;");
        return nullptr;
    }
    uint32_t len_a = a->channels * a->rows * a->cols;
    uint32_t len_b = b->channels * b->rows * b->cols;
    std::memcpy(output->matrix, a->matrix, sizeof(T) * len_a);
    std::memcpy(output->matrix + len_a, b->matrix, sizeof(T) * len_b);
    return output;
}

// ---------------------
// Gather (axis = 0 only, flatten index)
// ---------------------
template<typename T>
acenn_matrix<T>* gather_scalar(acenn_matrix<T>* input, const std::vector<int>& indices) {
    if (input == nullptr || input->matrix == nullptr) {
        printf_("ERROR: input is null!\n");
        asm("ebreak;");
        return nullptr;
    }
    acenn_matrix<T>* output = static_cast<acenn_matrix<T>*>(malloc(sizeof(acenn_matrix<T>)));
    output->dims = 1;
    output->channels = indices.size();
    output->rows = 1;
    output->cols = 1;
    output->matrix = create_tensor<T>(1, output->channels, 1, 1);
    if (output->matrix == nullptr) {
        free(output);
        printf_("ERROR: gather allocation failed!\n");
        asm("ebreak;");
        return nullptr;
    }
    for (size_t i = 0; i < indices.size(); ++i) {
        output->matrix[i] = input->matrix[indices[i]];
    }
    return output;
}

// ---------------------
// Reshape (only shape meta change)
// ---------------------
template<typename T>
acenn_matrix<T>* reshape_scalar(acenn_matrix<T>* input, int new_channels, int new_rows, int new_cols) {
    if (input == nullptr || input->matrix == nullptr) {
        printf_("ERROR: input is null!\n");
        asm("ebreak;");
        return nullptr;
    }
    uint32_t total_old = input->channels * input->rows * input->cols;
    uint32_t total_new = new_channels * new_rows * new_cols;
    if (total_old != total_new) {
        printf_("ERROR: reshape sizes mismatch!\n");
        asm("ebreak;");
        return nullptr;
    }
    acenn_matrix<T>* output = static_cast<acenn_matrix<T>*>(malloc(sizeof(acenn_matrix<T>)));
    *output = *input;
    output->channels = new_channels;
    output->rows = new_rows;
    output->cols = new_cols;
    return output;
}

// ---------------------
// Shape (return shape as int tensor)
// ---------------------
template<typename T>
std::vector<int> shape_scalar(acenn_matrix<T>* input) {
    if (input == nullptr) {
        printf_("ERROR: input is null!\n");
        asm("ebreak;");
        return {};
    }
    return {input->dims, input->channels, input->rows, input->cols};
}

// ---------------------
// Unsqueeze (only dims+=1 at axis 0)
// ---------------------
template<typename T>
acenn_matrix<T>* unsqueeze_scalar(acenn_matrix<T>* input) {
    if (input == nullptr || input->matrix == nullptr) {
        printf_("ERROR: input is null!\n");
        asm("ebreak;");
        return nullptr;
    }
    acenn_matrix<T>* output = static_cast<acenn_matrix<T>*>(malloc(sizeof(acenn_matrix<T>)));
    *output = *input;
    output->dims = input->dims + 1; // simplistic unsqueeze
    return output;
}

#endif
