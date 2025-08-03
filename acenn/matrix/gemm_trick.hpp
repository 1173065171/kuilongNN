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

#ifndef __STANDARD_HPP__
#define __STANDARD_HPP__

#include "stdint.h"
#include "common/common.hpp"
#include "common/xcvnn.hpp"

////////////////////////////////////////////////////////////////////////////////
// Function : standard_gemm
// Description:
//      This function performs a standard General Matrix Multiplication (GEMM)
//      operation on two matrices (matrixA and matrixB) and returns the result
//      in a new matrix (output). The GEMM operation is a fundamental operation
//      in many machine learning and neural network applications, where it is
//      used to compute the product of two matrices, often as part of a layer in
//      a neural network.
//
// Parameters:
//      acenn_matrix<T> *matrixA: Pointer to the first input matrix.
//      acenn_matrix<T> *matrixB: Pointer to the second input matrix.
//      acenn_matrix<T> *output: Pointer to the output matrix where the result
//                               will be stored.
//
// Return:
//      acenn_matrix<T> *:     Pointer to the output matrix containing the result of the GEMM operation.
//                           If the operation fails, it returns nullptr.
////////////////////////////////////////////////////////////////////////////////
template<typename T>
acenn_matrix<T> *standard_gemm(acenn_matrix<T> *matrixA, acenn_matrix<T> *matrixB, uint8_t shift_fig);

template<typename T>
acenn_matrix<T> *standard_gemm(acenn_matrix<T> *matrixA, acenn_matrix<T> *matrixB, uint8_t shift_fig) {
    // check matrix size
    if (matrixA->cols != matrixB->rows) {
        printf_("ERROR: matrixA cols (%d) != matrixB rows (%d)!\n", matrixA->cols, matrixB->rows);
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    // allocate memory space of output fm
    acenn_matrix<T> *output = static_cast<acenn_matrix<T>*>(malloc(sizeof(acenn_matrix<T>)));

    output->dims = 1;
    output->channels = matrixA->channels;
    output->rows = matrixA->rows;
    output->cols = matrixB->cols;
    output->matrix = create_tensor<T>(output->dims, output->channels, output->rows, output->cols);
    if (output->matrix == nullptr) {
        free(output);
        printf_("ERROR: failed to allocate output->matrix!\n");
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    return output;
}

#endif
