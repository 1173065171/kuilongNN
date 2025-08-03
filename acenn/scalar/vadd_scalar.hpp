//////////////////////////////////////////////////////////////////////////////////
// Author:          yintao liu - berialest@shu.edu.cn                           //
//                                                                              //
// Additional contributions by:                                                 //
//                                                                              //
// File Name:       vadd_scalar.hpp                                            //
// Project Name:    ACENN                                                       //
// Language:        cpp                                                         //
//                                                                              //
// Description:     Vector addition operations                                  //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#ifndef __VADD_SCALAR_HPP__
#define __VADD_SCALAR_HPP__

#include "type_traits"
#include "stdint.h"
#include "common/common.hpp"


// Function : vadd_scalar

template<typename T>
acenn_matrix<T> *vadd_scalar(acenn_matrix<T> *matrix_a, acenn_matrix<T> *matrix_b);

template<typename T>
acenn_matrix<T> *vadd_scalar(acenn_matrix<T> *matrix_a, acenn_matrix<T> *matrix_b) {
    // check input validity
    if (matrix_a == nullptr || matrix_a->matrix == nullptr || 
        matrix_b == nullptr || matrix_b->matrix == nullptr) {
        printf_("ERROR: input matrices are null!\n");
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    // check if dimensions match
    if (matrix_a->dims != matrix_b->dims || matrix_a->channels != matrix_b->channels ||
        matrix_a->rows != matrix_b->rows || matrix_a->cols != matrix_b->cols) {
        printf_("ERROR: matrix dimensions do not match!\n");
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    // allocate memory space for output matrix
    acenn_matrix<T> *output = static_cast<acenn_matrix<T>*>(malloc(sizeof(acenn_matrix<T>)));
    if (output == nullptr) {
        printf_("ERROR: failed to allocate output!\n");
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    // copy dimensions from input matrices
    output->dims = matrix_a->dims;
    output->channels = matrix_a->channels;
    output->rows = matrix_a->rows;
    output->cols = matrix_a->cols;
    
    // allocate memory for output matrix
    output->matrix = create_tensor<T>(output->dims, output->channels, output->rows, output->cols);
    if (output->matrix == nullptr) {
        free(output);
        printf_("ERROR: failed to allocate output->matrix!\n");
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    // apply vector addition element-wise
    uint32_t total_elements = output->channels * output->rows * output->cols;
    for (uint32_t i = 0; i < total_elements; ++i) {
        // Vector addition: result[i] = a[i] + b[i]
        output->matrix[i] = matrix_a->matrix[i] + matrix_b->matrix[i];
    }

    return output;
}



#endif