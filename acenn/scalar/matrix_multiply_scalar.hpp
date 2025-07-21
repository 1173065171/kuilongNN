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

#include "type_traits"
#include "stdint.h"
#include "common/common.hpp"
#include "common/common.cpp"

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
            
            output->matrix[i * output->cols + j] = sum;
        }
    }

    return output;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : matrix_vector_multiply
// Description:
//      This is a template function that performs matrix-vector multiplication. This operation is fundamental
//      in linear algebra and is commonly used in neural networks, particularly in fully connected layers,
//      where a weight matrix is multiplied by an input vector to produce an output vector. The function
//      implements the standard matrix-vector multiplication where the result vector y = A × x, where each
//      element y[i] is computed as the dot product of row i from matrix A and the input vector x. This
//      operation is more efficient than general matrix multiplication when one of the operands is a vector.
//      The template parameter 'T' allows for flexibility in handling different data types.
//
// Parameters:
//      acenn_matrix<T> *matrix:   Pointer to the struct representing the input matrix. The matrix should be
//                                properly initialized with valid dimensions and data. For the multiplication
//                                to be valid, the number of columns in the matrix must equal the number of
//                                elements in the input vector. The data type of the matrix elements is
//                                determined by the template parameter 'T'.
//      acenn_matrix<T> *vector:   Pointer to the struct representing the input vector. This should be a
//                                matrix with either 1 row (row vector) or 1 column (column vector). The
//                                number of elements in the vector must match the number of columns in the
//                                matrix for the operation to be mathematically valid.
//      uint8_t shift_fig:        Shift figure used for specific bitwise or numerical adjustments in the
//                                calculation. This can be useful for fixed-point arithmetic or optimization
//                                purposes. Only the lower 6 bits are considered valid.
//
// Return:
//      acenn_matrix<T> *:        Pointer to the resulting output vector after the matrix-vector multiplication.
//                                The returned vector will have the same number of elements as the number of
//                                rows in the input matrix. Memory is allocated for the result vector, which
//                                should be freed by the caller when no longer needed. Returns nullptr if the
//                                operation fails due to invalid dimensions or memory allocation errors.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
acenn_matrix<T> *matrix_vector_multiply(acenn_matrix<T> *matrix, acenn_matrix<T> *vector, uint8_t shift_fig);

template<typename T>
acenn_matrix<T> *matrix_vector_multiply(acenn_matrix<T> *matrix, acenn_matrix<T> *vector, uint8_t shift_fig) {
    // Check if input matrix and vector are valid
    if (matrix == nullptr || vector == nullptr) {
        printf_("ERROR: Input matrix or vector is null!\n");
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    // Determine vector size and orientation
    uint32_t vector_size = 0;
    bool is_column_vector = false;
    
    if (vector->rows == 1) {
        // Row vector
        vector_size = vector->cols;
        is_column_vector = false;
    } else if (vector->cols == 1) {
        // Column vector
        vector_size = vector->rows;
        is_column_vector = true;
    } else {
        printf_("ERROR: Second operand is not a vector!\n");
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    // Check if matrix and vector dimensions are compatible
    if (matrix->cols != vector_size) {
        printf_("ERROR: Matrix columns (%d) don't match vector size (%d)!\n", matrix->cols, vector_size);
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    // Allocate memory space for output vector
    acenn_matrix<T> *output = static_cast<acenn_matrix<T>*>(malloc(sizeof(acenn_matrix<T>)));
    if (output == nullptr) {
        printf_("ERROR: failed to allocate output vector!\n");
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    // Set output vector dimensions (column vector)
    output->dims = 1;
    output->channels = 1;
    output->rows = matrix->rows;
    output->cols = 1;
    output->matrix = create_tensor<T>(output->dims, output->channels, output->rows, output->cols);
    
    if (output->matrix == nullptr) {
        free(output);
        printf_("ERROR: failed to allocate output->matrix!\n");
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    // Perform matrix-vector multiplication
    for (uint32_t i = 0; i < matrix->rows; ++i) {
        T sum = 0;  // Define accumulative variable based on template type T
        
        // Compute dot product of row i from matrix and the input vector
        for (uint32_t j = 0; j < matrix->cols; ++j) {
            T matrix_val = matrix->matrix[i * matrix->cols + j];
            T vector_val;
            
            if (is_column_vector) {
                vector_val = vector->matrix[j];  // j-th element in column vector
            } else {
                vector_val = vector->matrix[j];  // j-th element in row vector
            }
            
            sum += matrix_val * vector_val;
        }
        
        // Apply shift operation if needed
        if (shift_fig > 0 && shift_fig <= 63) {
            sum = sum >> (shift_fig & 0x3F);  // Use only lower 6 bits
        }
        
        output->matrix[i] = sum;
    }

    return output;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : transpose_matrix
// Description:
//      This is a template function that computes the transpose of a matrix. Matrix transposition is a
//      fundamental operation in linear algebra where the rows and columns of the matrix are interchanged.
//      If the original matrix has dimensions m×n, the transposed matrix will have dimensions n×m. This
//      operation is commonly used in various mathematical computations, machine learning algorithms, and
//      data processing tasks. The function creates a new matrix with the transposed values, leaving the
//      original matrix unchanged. The template parameter 'T' allows for flexibility in handling different
//      data types for the matrix elements.
//
// Parameters:
//      acenn_matrix<T> *matrix:   Pointer to the struct representing the input matrix to be transposed.
//                                The matrix should be properly initialized with valid dimensions and data.
//                                The data type of the matrix elements is determined by the template
//                                parameter 'T'. The original matrix remains unchanged after the operation.
//
// Return:
//      acenn_matrix<T> *:        Pointer to the resulting transposed matrix. The returned matrix will have
//                                dimensions swapped compared to the input matrix (if input is m×n, output
//                                is n×m). Memory is allocated for the result matrix, which should be freed
//                                by the caller when no longer needed. Returns nullptr if the operation fails
//                                due to invalid input or memory allocation errors.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
acenn_matrix<T> *transpose_matrix(acenn_matrix<T> *matrix);

template<typename T>
acenn_matrix<T> *transpose_matrix(acenn_matrix<T> *matrix) {
    // Check if input matrix is valid
    if (matrix == nullptr) {
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

    // Set output matrix dimensions (swap rows and cols)
    output->dims = matrix->dims;
    output->channels = matrix->channels;
    output->rows = matrix->cols;
    output->cols = matrix->rows;
    output->matrix = create_tensor<T>(output->dims, output->channels, output->rows, output->cols);
    
    if (output->matrix == nullptr) {
        free(output);
        printf_("ERROR: failed to allocate output->matrix!\n");
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    // Perform matrix transposition
    for (uint32_t i = 0; i < matrix->rows; ++i) {
        for (uint32_t j = 0; j < matrix->cols; ++j) {
            // Element at (i,j) in original matrix goes to (j,i) in transposed matrix
            output->matrix[j * output->cols + i] = matrix->matrix[i * matrix->cols + j];
        }
    }

    return output;
}

#endif
