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

#ifndef __STANDARD_CONV2D_SCALAR_HPP__
#define __STANDARD_CONV2D_SCALAR_HPP__

#include "type_traits"
#include "stdint.h"
#include "common/common.hpp"

////////////////////////////////////////////////////////////////////////////////
// Function : standard_conv2d_3x3
// Description:
//      This is a template function that serves a purpose similar to performing
//      a two-dimensional convolution operation with a fixed kernel size of 3x3.
//      It is applicable for different data types specified by the template
//      parameter 'T'. The convolution is applied to an input feature map to
//      generate an output feature map, which is a common operation in deep
//      learning, especially in convolutional neural networks for tasks like
//      image recognition, object detection, etc. It allows for flexibility in
//      handling various data representations depending on the specific needs
//      of the application or the data characteristics.
//
// Parameters:
//      uint8_t stride:         The stride value determines the step size at which
//                             the convolution kernel moves across the input
//                             feature map. A smaller stride implies more
//                             overlapping computations and results in a larger
//                             output feature map size compared to a larger
//                             stride. It must be less than 3 in this function.
//      uint32_t padding:       The number of zero values to be filled around the
//                             edges of the input feature map before applying the
//                             convolution. Padding can be used to control the
//                             size of the output feature map and to handle the
//                             boundary cases properly. For example, 'same'
//                             padding tries to keep the output size similar to
//                             the input size when possible.
//      acenn_matrix<T> *fm:    P ointer to the struct representing the input
//                             feature map. The feature map is a multi-
//                             dimensional data structure (usually with
//                             dimensions like channels, height, and width)
//                             that contains the input data on which the
//                             convolution operation will be performed. The
//                             specific data type of the elements in the
//                             feature map is determined by the template
//                             parameter 'T'.
//      acenn_matrix<T> *kernel:  Pointer to the struct representing the convolution
//                             kernel. The kernel is a small matrix (in this case,
//                             3x3) with weights that are multiplied with the
//                             corresponding elements of the input feature map
//                             during the convolution process to extract features.
//                             The data type of its elements also follows the
//                             template parameter 'T'.
//      uint8_t shift_fig:      Shift figure which is used for some specific
//                             bitwise or numerical adjustments in the
//                             calculation. Only the lower 6 bits are valid,
//                             indicating that only these bits carry meaningful
//                             information for the internal operations related
//                             to this function.
//
// Return:
//      acenn_matrix<T> *:      Pointer to the resulting output feature map after
//                             the convolution operation. The returned struct
//                             represents the new feature map that has been
//                             processed by the convolution with the given
//                             kernel, stride, and padding, and is ready
//                             to be used as input for further operations in
//                             the neural network pipeline, such as passing it
//                             to another layer or applying activation functions.
//                             The data type of the elements in the returned
//                             feature map follows the template parameter 'T'.
////////////////////////////////////////////////////////////////////////////////
template<typename T>
acenn_matrix<T> *standard_conv2d_3x3(uint8_t stride, uint32_t padding, acenn_matrix<T> *fm, acenn_matrix<T> *kernel, uint8_t shift_fig);

template<typename T>
acenn_matrix<T> *standard_conv2d_3x3(uint8_t stride, uint32_t padding, acenn_matrix<T> *fm, acenn_matrix<T> *kernel, uint8_t shift_fig) {
    //  check stride
    if (stride > 3) {
        printf_("ERROR: stride > 3!\n");
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    // get the size of output fm 
    uint32_t out_rows = (fm->rows - 3 + 2 * padding) / stride + 1;
    uint32_t out_cols = (fm->cols - 3 + 2 * padding) / stride + 1;

    // allocate memory space of output fm
    acenn_matrix<T> *output = static_cast<acenn_matrix<T>*>(malloc(sizeof(acenn_matrix<T>)));
    if (output == nullptr) {
        printf_("ERROR: failed to allocate output!\n");
        asm("ebreak;"); // 中断，停止运行
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
        asm("ebreak;"); // 中断，停止运行
        return nullptr;
    }

    acenn_matrix<T> fm_padding;
    fm_padding.dims = 1;
    fm_padding.channels = fm->channels;
    fm_padding.rows = fm->rows + 2 * padding;
    fm_padding.cols = fm->cols + 2 * padding;
    fm_padding.matrix = create_tensor<T>(fm_padding.dims, fm_padding.channels, fm_padding.rows, fm_padding.cols);
    //  padding
    if (padding >= 1) {
        for (int k = 0; k < fm_padding.channels; ++k) {
            for (int i = 0; i < fm_padding.rows; ++i) {
                for (int j = 0; j < fm_padding.cols; ++j) {
                    //  boundary
                    if ((i <= padding) || (i >= fm->rows) || (j <= padding) || (j >= fm->cols)) {
                        fm_padding.matrix[(k * fm_padding.rows * fm_padding.cols) + (i * fm_padding.cols) + j] = 0;
                    } else {
                        fm_padding.matrix[(k * fm_padding.rows * fm_padding.cols) + (i * fm_padding.cols) + j] = fm->matrix[(k * fm->rows * fm->cols) + (i * fm->cols) + j - padding];
                    }
                }
            }
        }
    }

    //  convolution
    for (uint32_t k = 0; k < output->channels; ++k) {
        for (uint32_t i = 0; i < output->rows; ++i) {
            for (uint32_t j = 0; j < output->cols; ++j) {
                T sum = 0;  //  define cumulative variable types based on template type T
                for (uint32_t f = 0; f < fm_padding.channels; ++f) {
                    for (int n = 0; n < kernel->rows; ++n) {
                        for (int m = 0; m < kernel->cols; ++m) {
                            sum += fm_padding.matrix[(f * fm_padding.rows * fm_padding.cols) + ((i + n) * fm_padding.cols) + j + m] * 
                                   kernel->matrix[(k * fm_padding.channels * fm_padding.rows * fm_padding.cols) + (f * fm_padding.rows * fm_padding.cols) + (n * fm_padding.cols) + m];
                        }
                    }
                }
                // printf_("output->matrix[%d]\n", (k * output->rows * output->cols) + (i * output->cols) + j);
                output->matrix[(k * output->rows * output->cols) + (i * output->cols) + j] = sum;
            }
        }
    }
    free_tensor(fm_padding.matrix);
    return output;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : batchnorm2d
// Description:
//      This is a template function that performs a form of two-dimensional batch normalization operation on an
//      input feature map. Batch normalization is commonly used in neural networks to normalize the input data
//      distribution within a mini-batch during training. In this implementation, it iterates over each element
//      in the multi-dimensional 'acenn_matrix' representing the feature map and adds a corresponding bias value
//      to each element. The nested loops are used to traverse through all the dimensions (including dimensions,
//      channels, rows, and columns) of the feature map. The template parameter 'T' allows for flexibility in
//      handling different data types for the elements of the feature map and the bias values, enabling it to be
//      used in various neural network scenarios with different data representations. The function is declared
//      as 'inline' to suggest to the compiler that it should expand the function's code in-place when called,
//      which can potentially reduce the overhead of a function call and improve performance in some cases.
//
// Parameters:
//      acenn_matrix<T> *fm:    Pointer to the struct representing the input feature map. The 'acenn_matrix' is a
//                             template-defined data structure with multiple dimensions, where the properties
//                             'dims', 'channels', 'rows', and 'cols' define its structure. The data type of the
//                             elements within this matrix is determined by the template parameter 'T'. This is
//                             the data on which the batch normalization operation will be performed.
//      T *bias:                Pointer to an array of bias values. The data type of these bias values is the
//                             same as the template parameter 'T'. The bias values are added to the corresponding
//                             elements of the feature map during the batch normalization process. Each element
//                             in the feature map within a particular channel will have the corresponding bias
//                             value added to it.
//
// Return:
//      acenn_matrix<T> *:      Pointer to the resulting output feature map after performing the batch normalization
//                             operation. The returned pointer points to the same 'acenn_matrix' structure that was
//                             passed in as input, but with its elements modified by adding the appropriate bias
//                             values. This modified feature map can then be used as input for further operations in
//                             the neural network pipeline, such as being passed to the next layer for additional
//                             processing or having other normalization or activation functions applied to it.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
inline acenn_matrix<T> *batchnorm2d(acenn_matrix<T> *fm, T *bias);

template<typename T>
inline acenn_matrix<T> *batchnorm2d(acenn_matrix<T> *fm, T *bias) {
    for (int d = 0; d < fm->dims; ++d) {
        for (int c = 0; c < fm->channels; ++c) {
            for (int i = 0; i < fm->rows; ++i) {
                for (int j = 0; j < fm->cols; ++j) {
                    fm->matrix[(d * fm->channels * fm->rows * fm->cols) + (c * fm->rows * fm->cols) + (i * fm->cols) + j] += *bias;
                }
            }
            bias++;
        }
    }
    return fm;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : relu
// Description:
//      This is a template function that implements the Rectified Linear Unit (ReLU) activation function.
//      ReLU is a widely used activation function in the field of neural networks. Its purpose is to introduce
//      non-linearity into the network's computations. For each element in the input feature map, it applies
//      a simple rule: if the element's value is greater than 0, it remains unchanged; otherwise, it is set to 0.
//      This transformation helps the neural network to learn more complex patterns and relationships in the data,
//      as it allows the network to have a non-linear response to different input values. The template parameter
//      'T' makes the function generic, enabling it to work with different data types when the 'acenn_matrix' template
//      is instantiated with specific types. This provides flexibility for handling various data representations in
//      different neural network architectures or applications.
//
// Parameters:
//      acenn_matrix<T> *fm:    Pointer to the struct representing the input feature map. The 'acenn_matrix' is a
//                             template-defined data structure that likely encapsulates multi-dimensional data,
//                             where the specific dimensions are represented by members like 'dims' (number of
//                             dimensions), 'channels' (number of channels, which could be analogous to color
//                             channels in image data), 'rows', and 'cols' (representing the spatial dimensions in
//                             a 2D or higher-dimensional context). The data type of the elements within this
//                             matrix is determined by the template parameter 'T'. This input feature map contains
//                             the data on which the ReLU operation will be applied.
//
// Return:
//      acenn_matrix<T> *:      Pointer to the resulting output feature map after applying the ReLU activation function.
//                             The returned pointer points to the same 'acenn_matrix' structure that was passed in as
//                             input, but with its elements modified according to the ReLU rule. This modified feature
//                             map can then be used as input for further operations in the neural network pipeline,
//                             such as being passed to the next layer for further processing or having additional
//                             activation functions applied to it if required by the network architecture.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
inline acenn_matrix<T> *relu(acenn_matrix<T> *fm);

template<typename T>
inline acenn_matrix<T> *relu(acenn_matrix<T> *fm) {
    for (int i = 0; i < (fm->dims * fm->channels * fm->rows * fm->cols); ++i) {
        fm->matrix[i] = (fm->matrix[i] > 0)? fm->matrix[i] : 0;
    }
    return fm;
}

#endif