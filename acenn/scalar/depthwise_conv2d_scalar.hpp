//////////////////////////////////////////////////////////////////////////////////
// Author:          beining zhao - zbn@shu.edu.cn                               //
//                                                                              //
// Additional contributions by:                                                 //
//                  yintao liu - berialest@shu.edu.cn                           //
// File Name:       depthwise_conv2d_scalar.hpp                                 //
// Project Name:    ACENN                                                       //
// Language:        cpp                                                         //
//                                                                              //
// Description:     Depthwise convolution                                       //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#ifndef __DEPTHWISE_CONV2D_SCALAR_HPP__
#define __DEPTHWISE_CONV2D_SCALAR_HPP__

#include "stdint.h"
#include "common/common.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : group_conv2d_3x3
// Description:
//      This is a template function designed to perform a 3x3 group convolution operation. Group convolution is a variant of
//      standard convolution, which divides the input feature map and the convolution kernel into several groups and
//      performs convolution operations within each group separately. This technique is useful in deep learning
//      architectures, especially in convolutional neural networks, as it can reduce computational cost and
//      improve model efficiency while maintaining similar performance. The template parameter 'T' allows for
//      flexibility in handling different data types of the elements in the input feature map, and convolution kernel.
//      It takes several parameters that define the configuration of the convolution operation.
//
// Parameters:
//      uint8_t stride:        The stride value determines the step size at which the convolution kernel moves across
//                             the input feature map. A smaller stride implies more overlapping computations
//                             and results in a larger output feature map size compared to a larger stride. It
//                             controls the spatial downsampling of the feature map.
//      uint32_t padding:       The number of zero values to be padded around the edges of the input feature map before
//                             applying the convolution. Padding helps to control the size of the output feature
//                             map and handle boundary cases, ensuring that the spatial dimensions of the
//                             output are appropriate for subsequent layers.
//      acenn_matrix<T> *fm:    Pointer to the struct representing the input feature map. The 'acenn_matrix' is a
//                             template-defined data structure with multiple dimensions, and the data type of
//                             its elements is determined by the template parameter 'T'. It contains the
//                             input data on which the group convolution operation will be performed.
//      acenn_matrix<T> *kernel: Pointer to the struct representing the convolution kernel. The kernel is a 3x3 matrix
//                             in this case, and its elements' data type follows the template parameter 'T'. The
//                             kernel is used to extract features from the input feature map through
//                             convolution.
//      uint8_t shift_fig:      A shift figure used for some specific bitwise or numerical adjustments in the
//                             calculation. Only the lower 6 bits are valid, indicating that only these bits
//                             carry meaningful information for the internal operations related to this function.
//
// Return:
//      acenn_matrix<T> *:      Pointer to the resulting output feature map after the group convolution operation.
//                             The returned pointer points to the new feature map that has been processed by
//                             the group convolution with the given kernel, stride, and padding. This
//                             output feature map can be used as input for further operations in the neural
//                             network pipeline, such as passing it to another layer or applying activation
//                             functions.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
acenn_matrix<T> *depthwise_conv2d_3x3(uint8_t stride, uint32_t padding, acenn_matrix<T> *fm, acenn_matrix<T> *kernel, uint8_t shift_fig);

template<typename T>
acenn_matrix<T> *depthwise_conv2d_3x3(uint8_t stride, uint32_t padding, acenn_matrix<T> *fm, acenn_matrix<T> *kernel, uint8_t shift_fig) {
    //  check stride
    if (stride >= 3) {
        printf_("ERROR: stride >= 3!\n");
        return nullptr;
    }

    // get the size of output fm 
    uint32_t out_rows = (fm->rows - 3 + 2 * padding) / stride + 1;
    uint32_t out_cols = (fm->cols - 3 + 2 * padding) / stride + 1;

    // allocate memory space of output fm
    acenn_matrix<T> *output = static_cast<acenn_matrix<T>*>(malloc(sizeof(acenn_matrix<T>)));
    if (output == nullptr) {
        printf_("ERROR: failed to allocate output!\n");
        return nullptr;
    }

    output->dims = 1;
    output->channels = kernel->dims;
    output->rows = out_rows;
    output->cols = out_cols;
    output->matrix = create_tensor<T>(output->dims, output->channels, output->rows, output->cols);
    if (output->matrix == nullptr) {
        free(output);
        printf_("ERROR: failed to allocate output->matrix!\n");
        asm("ebreak;"); //  仿真停止
        return nullptr;
    }

    //  padding
    acenn_matrix<T> fm_padding;
    if (padding >= 1) {
        fm_padding.dims = 1;
        fm_padding.channels = fm->channels;
        fm_padding.rows = fm->rows + 2 * padding;
        fm_padding.cols = fm->cols + 2 * padding;
        fm_padding.matrix = create_tensor<T>(fm_padding.dims, fm_padding.channels, fm_padding.rows, fm_padding.cols);
        for (int k = 0; k < fm_padding.channels; ++k) {
            for (int i = 0; i < fm_padding.rows; ++i) {
                for (int j = 0; j < fm_padding.cols; ++j) {
                    //  boundary
                    if ((i < padding) || (i >= fm->rows + padding) || (j < padding) || (j >= fm->cols + padding)) {
                        (fm_padding.matrix)[(k * fm_padding.rows * fm_padding.cols) + (i * fm_padding.cols) + j] = 0;
                    }
                    else {
                        (fm_padding.matrix)[(k * fm_padding.rows * fm_padding.cols) + (i * fm_padding.cols) + j] = (fm->matrix)[(k * fm->rows * fm->cols) + ((i - padding) * fm->cols) + (j - padding)];
                    }
                }
            }
        }
    }
    else {
        fm_padding = *fm;
    }

    //  convolution
    for (uint32_t dim = 0; dim < output->dims; ++dim) {
        for (uint32_t channel = 0; channel < output->channels; ++channel) {
            for (uint32_t row = 0; row < output->rows; ++row) {
                for (uint32_t col = 0; col < output->cols; ++col) {
                    uint32_t sum = 0;
                    for (uint32_t ki = 0; ki < 3; ++ki) {
                        for (uint32_t kj = 0; kj < 3; ++kj) {
                            uint32_t input_row = row * stride + ki;
                            uint32_t input_col = col * stride + kj;
                            T input_value = fm_padding.matrix[(channel * fm_padding.rows * fm_padding.cols) + (input_row * fm_padding.cols) + input_col];
                            T kernel_value = kernel->matrix[(channel * 3 * 3) + (ki * 3) + kj];
                            sum += input_value * kernel_value;
                        }
                    }
                    (output->matrix)[(channel * out_rows * out_cols) + (row * out_cols) + col] = static_cast<T>((sum >> shift_fig) & 0xFF);
                }
            }
        }
    }
    if (padding >= 1) {
        free_tensor(fm_padding.matrix);
    }
    return output;
}

#endif