//////////////////////////////////////////////////////////////////////////////////
// Author:          yintao liu - berialest@shu.edu.cn                           //
//                                                                              //
// Additional contributions by:                                                 //
//                                                                              //
// File Name:       matrix_multiply_scalar.hpp                                  //
// Project Name:    ACENN                                                       //
// Language:        cpp                                                         //
//                                                                              //
// Description:     Matrix multiplication operations                            //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#ifndef __MATRIX_MULTIPLY_SCALAR_HPP__
#define __MATRIX_MULTIPLY_SCALAR_HPP__

#include "stdint.h"
#include "common/common.hpp"

////////////////////////////////////////////////////////////////////////////////
// Function : matrix_multiply
// Description:
//      This is a template function that performs matrix multiplication between
//      two matrices. Matrix multiplication is a fundamental operation in linear
//      algebra and is widely used in deep learning, computer graphics, scientific
//      computing, and many other fields. The function implements the standard
//      matrix multiplication algorithm where the result matrix C = A × B, where
//      element C[i][j] is computed as the dot product of row i from matrix A and
//      column j from matrix B. The template parameter 'T' allows for flexibility
//      in handling different data types, making it suitable for various numerical
//      representations depending on the specific requirements of the application.
//
// Parameters:
//      acenn_matrix<T> *matrix_a:  Pointer to the struct representing the first input
//                                 matrix (left operand). The matrix should be properly
//                                 initialized with valid dimensions and data. For
//                                 multiplication to be valid, the number of columns
//                                 in matrix_a must equal the number of rows in matrix_b.
//                                 The data type of the elements is determined by the
//                                 template parameter 'T'.
//      acenn_matrix<T> *matrix_b:  Pointer to the struct representing the second input
//                                 matrix (right operand). This matrix will be multiplied
//                                 with matrix_a to produce the result. The number of
//                                 rows in matrix_b must match the number of columns in
//                                 matrix_a for the operation to be mathematically valid.
//                                 The data type follows the template parameter 'T'.
//      uint8_t shift_fig:         Shift figure which is used for specific bitwise or
//                                numerical adjustments in the calculation. This parameter
//                                can be used for optimization purposes or fixed-point
//                                arithmetic adjustments. Only the lower 6 bits are valid,
//                                indicating that only these bits carry meaningful
//                                information for the internal operations.
//
// Return:
//      acenn_matrix<T> *:         Pointer to the resulting output matrix after the
//                                multiplication operation. The returned matrix will have
//                                dimensions of (matrix_a->rows × matrix_b->cols). The
//                                function allocates memory for the result matrix, which
//                                should be freed by the caller when no longer needed.
//                                Returns nullptr if the operation fails due to invalid
//                                dimensions, memory allocation errors, or other issues.
////////////////////////////////////////////////////////////////////////////////
template<typename T>
acenn_matrix<T> *matrix_multiply(acenn_matrix<T> *matrix_a, acenn_matrix<T> *matrix_b, uint8_t shift_fig);

template<typename T>
acenn_matrix<T> *matrix_multiply(acenn_matrix<T> *matrix_a, acenn_matrix<T> *matrix_b, uint8_t shift_fig) {
    // Check if input matrices are valid
    if (matrix_a == nullptr || matrix_b == nullptr) {
        printf_("ERROR: Input matrices are null!\n");
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    // Check if matrix dimensions are compatible for multiplication
    if (matrix_a->cols != matrix_b->rows) {
        printf_("ERROR: Matrix dimensions incompatible for multiplication!\n");
        printf_("Matrix A cols: %d, Matrix B rows: %d\n", matrix_a->cols, matrix_b->rows);
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

    // Set output matrix dimensions
    output->dims = 1;
    output->channels = 1;
    output->rows = matrix_a->rows;
    output->cols = matrix_b->cols;
    output->matrix = create_tensor<T>(output->dims, output->channels, output->rows, output->cols);
    
    if (output->matrix == nullptr) {
        free(output);
        printf_("ERROR: failed to allocate output->matrix!\n");
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    // Perform matrix multiplication
    for (uint32_t i = 0; i < output->rows; ++i) {
        for (uint32_t j = 0; j < output->cols; ++j) {
            T sum = 0;  // Define accumulative variable based on template type T

            // Compute dot product of row i from matrix_a and column j from matrix_b
            for (uint32_t k = 0; k < matrix_a->cols; ++k) {
                T a_val = matrix_a->matrix[i * matrix_a->cols + k];
                T b_val = matrix_b->matrix[k * matrix_b->cols + j];
                sum += a_val * b_val;
            }
            
            // Apply shift operation if needed (for fixed-point arithmetic or optimization)
            if (shift_fig > 0 && shift_fig <= 63) {
                sum = sum >> (shift_fig & 0x3F);  // Use only lower 6 bits
            }
            
            output->matrix[i * output->cols + j] = sum >> shift_fig;
        }
    }

    return output;
}

#endif
