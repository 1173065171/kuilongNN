//////////////////////////////////////////////////////////////////////////////////
// Author:          yintao liu - berialest@shu.edu.cn                           //
//                                                                              //
// Additional contributions by:                                                 //
//                                                                              //
// File Name:       relu.hpp                                                    //
// Project Name:    ACENN                                                       //
// Language:        cpp                                                         //
//                                                                              //
// Description:     ReLU activation function                                    //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#ifndef __RELU_HPP__
#define __RELU_HPP__

#include "stdint.h"
#include "common/common.hpp"


// Function : relu

template<typename T>
acenn_matrix<T> *relu_scalar(acenn_matrix<T> *fm);

template<typename T>
acenn_matrix<T> *relu_scalar(acenn_matrix<T> *fm) {
    // check input validity
    if (fm == nullptr || fm->matrix == nullptr) {
        printf_("ERROR: input feature map is null!\n");
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    // allocate memory space for output feature map
    acenn_matrix<T> *output = static_cast<acenn_matrix<T>*>(malloc(sizeof(acenn_matrix<T>)));
    if (output == nullptr) {
        printf_("ERROR: failed to allocate output!\n");
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    // copy dimensions from input feature map
    output->dims = fm->dims;
    output->channels = fm->channels;
    output->rows = fm->rows;
    output->cols = fm->cols;
    
    // allocate memory for output matrix
    output->matrix = create_tensor<T>(output->dims, output->channels, output->rows, output->cols);
    if (output->matrix == nullptr) {
        free(output);
        printf_("ERROR: failed to allocate output->matrix!\n");
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    // apply ReLU activation function element-wise
    uint32_t total_elements = output->channels * output->rows * output->cols;
    for (uint32_t i = 0; i < total_elements; ++i) {
        // ReLU function: f(x) = max(0, x)
        T input_val = fm->matrix[i];
        if (input_val > 0) {
            output->matrix[i] = input_val;
        } else {
            output->matrix[i] = 0;
        }
    }

    return output;
}

#endif