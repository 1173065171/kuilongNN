//////////////////////////////////////////////////////////////////////////////////
// Author:          beining zhao - zbn@shu.edu.cn                               //
//                                                                              //
// Additional contributions by:                                                 //
//                  yintao liu - berialest@shu.edu.cn                           //
// File Name:       pointwise_conv2d_scalar.hpp                                 //
// Project Name:    ACENN                                                       //
// Language:        cpp                                                         //
//                                                                              //
// Description:     Pointwise convolution                                       //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#ifndef __POINTWISE_CONV2D_SCALAR_HPP__
#define __POINTWISE_CONV2D_SCALAR_HPP__

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
//                             the group convolution with the given kernel. This
//                             output feature map can be used as input for further operations in the neural
//                             network pipeline, such as passing it to another layer or applying activation
//                             functions.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
acenn_matrix<T> *pointwise_conv2d_1x1(acenn_matrix<T> *fm, acenn_matrix<T> *kernel, T *bias, uint8_t shift_fig);

template<typename T>
acenn_matrix<T> *pointwise_conv2d_1x1(acenn_matrix<T> *fm, acenn_matrix<T> *kernel, T *bias, uint8_t shift_fig) {
	// get the size of output fm 
	uint32_t out_rows = fm->rows;
	uint32_t out_cols = fm->cols;

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
	fm_padding = *fm;

	//  convolution
	for (uint32_t dim = 0; dim < output->dims; ++dim) {
		for (uint32_t oc = 0; oc < kernel->dims; ++oc){
			for (uint32_t row = 0; row < output->rows; ++row) {
				for (uint32_t col = 0; col < output->cols; ++col) {
					int32_t sum = 0;
					uint32_t input_row = row;
					uint32_t input_col = col;
					for (uint32_t ic = 0; ic < kernel->channels; ++ic) {
						// pw conv kernel
						T input_value = fm_padding.matrix[(ic * fm_padding.rows * fm_padding.cols) + (input_row * fm_padding.cols) + input_col];
						T kernel_value = kernel->matrix[(oc*kernel->channels+ic)];
						sum += input_value * kernel_value;
						if ((oc == 1) && (row == 0) && (col == 0)) {
							ACENN_DEBUG("input_value: %x, kernel_value: %x, sum: %x, bias: %x", input_value, kernel_value, sum, bias[oc]);
						}
					}
					sum += bias[oc];
					(output->matrix)[(oc * out_rows * out_cols) + (row * out_cols) + col] = static_cast<T>((sum >> shift_fig) & 0xFF);
				}
			}
		}
	}
	free_tensor(fm_padding.matrix);
	return output;
}


#endif