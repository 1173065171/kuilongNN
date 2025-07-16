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

#include "type_traits"
#include "stdint.h"
#include "common/common.hpp"

////////////////////////////////////////////////////////////////////////////////
// Function : relu
// Description:
//      This is a template function that applies the ReLU (Rectified Linear Unit)
//      activation function to a feature map. ReLU is one of the most commonly
//      used activation functions in deep learning, particularly in convolutional
//      neural networks. It introduces non-linearity to the model by setting
//      all negative values to zero while keeping positive values unchanged.
//      The function is mathematically defined as: f(x) = max(0, x).
//      It is applicable for different data types specified by the template
//      parameter 'T'. The ReLU activation is applied element-wise to the
//      input feature map, which is a common operation in neural network
//      layers for tasks like image recognition, object detection, etc.
//
// Parameters:
//      acenn_matrix<T> *fm:    Pointer to the struct representing the input
//                             feature map. The feature map is a multi-
//                             dimensional data structure (usually with
//                             dimensions like channels, height, and width)
//                             that contains the input data on which the
//                             ReLU activation function will be applied.
//                             The specific data type of the elements in the
//                             feature map is determined by the template
//                             parameter 'T'.
//      uint8_t shift_fig:      Shift figure which is used for some specific
//                             bitwise or numerical adjustments in the
//                             calculation. Only the lower 6 bits are valid,
//                             indicating that only these bits carry meaningful
//                             information for the internal operations related
//                             to this function. This parameter can be used
//                             for quantization or fixed-point arithmetic
//                             optimizations.
//
// Return:
//      acenn_matrix<T> *:      Pointer to the resulting output feature map after
//                             the ReLU activation has been applied. The returned
//                             struct represents the new feature map that has been
//                             processed by the ReLU function, with all negative
//                             values set to zero and positive values preserved.
//                             The output feature map has the same dimensions as
//                             the input feature map. The data type of the
//                             elements in the returned feature map follows the
//                             template parameter 'T'.
////////////////////////////////////////////////////////////////////////////////
template<typename T>
acenn_matrix<T> *relu(acenn_matrix<T> *fm, uint8_t shift_fig);

template<typename T>
acenn_matrix<T> *relu(acenn_matrix<T> *fm, uint8_t shift_fig) {
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