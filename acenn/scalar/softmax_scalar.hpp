//////////////////////////////////////////////////////////////////////////////////
// Author:          yintao liu - berialest@shu.edu.cn                           //
//                                                                              //
// Additional contributions by:                                                 //
//                                                                              //
// File Name:       softmax_scalar.hpp                                         //
// Project Name:    ACENN                                                       //
// Language:        cpp                                                         //
//                                                                              //
// Description:     Softmax operations                                         //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#ifndef __SOFTMAX_SCALAR_HPP__
#define __SOFTMAX_SCALAR_HPP__

#include "stdint.h"
#include "common/common.hpp"
#include <cmath>

////////////////////////////////////////////////////////////////////////////////
// Function : softmax
// Description:
//      This is a template function that computes the softmax of a matrix.
////////////////////////////////////////////////////////////////////////////////

template<typename T>
acenn_matrix<T> *softmax(acenn_matrix<T> *input_matrix, uint8_t shift_fig = 0);

template<typename T>
acenn_matrix<T> *softmax(acenn_matrix<T> *input_matrix, uint8_t shift_fig) {
    // Check if input matrix is valid
    if (input_matrix == nullptr) {
        printf_("ERROR: Input matrix is null!\n");
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    // Allocate memory space for output matrix
    acenn_matrix<T> *output = static_cast<acenn_matrix<T>*>(malloc(sizeof(acenn_matrix<T>)));
    if (output == nullptr) {
        printf_("ERROR: failed to allocate output matrix!\n");
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    // Set output matrix dimensions (same as input)
    output->dims = input_matrix->dims;
    output->channels = input_matrix->channels;
    output->rows = input_matrix->rows;
    output->cols = input_matrix->cols;
    output->matrix = create_tensor<T>(output->dims, output->channels, output->rows, output->cols);
    
    if (output->matrix == nullptr) {
        free(output);
        printf_("ERROR: failed to allocate output->matrix!\n");
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    uint32_t total_elements = output->rows * output->cols;
    
    // Find maximum value for numerical stability
    T max_val = input_matrix->matrix[0];
    for (uint32_t i = 1; i < total_elements; ++i) {
        if (input_matrix->matrix[i] > max_val) {
            max_val = input_matrix->matrix[i];
        }
    }

    // Compute exponentials and sum
    T sum_exp = 0;
    for (uint32_t i = 0; i < total_elements; ++i) {
        // TODO : 这里用了标准库的 exp 函数，如何通过近似计算实现
        T exp_val = static_cast<T>(exp(static_cast<double>(input_matrix->matrix[i] - max_val)));
        output->matrix[i] = exp_val;
        sum_exp += exp_val;
    }

    // Normalize by dividing by sum
    for (uint32_t i = 0; i < total_elements; ++i) {
        output->matrix[i] = output->matrix[i] / sum_exp;
        
        // Apply shift operation if needed (for fixed-point arithmetic)
        if (shift_fig > 0 && shift_fig <= 63) {
            output->matrix[i] = output->matrix[i] >> (shift_fig & 0x3F);
        }
    }

    return output;
}



// ---------------------
// Softmax1
// ---------------------
template<typename T>
acenn_matrix<T>* softmax1_scalar(acenn_matrix<T>* input) {
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
    T max_val = *std::max_element(input->matrix, input->matrix + total_elements);
    T sum = 0;
    for (uint32_t i = 0; i < total_elements; ++i)
        sum += std::exp(input->matrix[i] - max_val);
    for (uint32_t i = 0; i < total_elements; ++i)
        output->matrix[i] = std::exp(input->matrix[i] - max_val) / sum;
    return output;
}

#endif
