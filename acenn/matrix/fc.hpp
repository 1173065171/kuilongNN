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

#ifndef __MATRIX_FULLCONNECTION_HPP__
#define __MATRIX_FULLCONNECTION_HPP__

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
acenn_matrix<T> *fullconnection(acenn_matrix<T> *matrixA, acenn_matrix<T> *matrixB, uint8_t shift_fig);

template<typename T>
acenn_matrix<T> *fullconnection(acenn_matrix<T> *matrixA, acenn_matrix<T> *matrixB, uint8_t shift_fig) {
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


void matmul_multi8(int8_t *input_blockA, int8_t *input_blockB, int8_t *output_blockD, int M, int K, int N) {
	const int us=8;
	ACENN_DEBUG("SAU matmul_multi8 START: A[%d,%d] B[%d,%d] -> D[%d,%d]", M, K, K, N, M, N);
	// 配置寄存器
	int32_t a,b,c,d,e,f;
	// 一次性声明所有变量
	int32_t h_ch = 0;
	int32_t h_x = K/us;
	int32_t v_ch = us;
	int32_t v_x = N/us;
	int32_t o_ch = 1;
	int32_t o_x = N/us;
	int32_t id = 0;
	int32_t bias_address = 0;
	int32_t flow_loop_times = K/us;
	int32_t stride_mode = 0;
	int32_t transpose_mode = 1;
	int32_t cutbit = 4;
	int32_t shift_mode = 0;
	int32_t conv_kernel = 0;
	int32_t register_mode = 0;
	int32_t flow_mode = 2;
	int32_t work_mode = 0;
	int32_t h_addr = 0;
	int32_t v_addr = 0;
	int32_t o_addr = 0;
	// once for all


	if ((K == 400) && (N < 128)) {
		int minblock = 128;
		int chunk = K/minblock;
		int rest = K - chunk*minblock;
		ACENN_DEBUG("K==%d, split!, chunk=%d, rest=%d", K, chunk, rest);
		ACENN_DEBUG("A: %d, %d, %d, %d", input_blockA[0], input_blockA[1], input_blockA[2], input_blockA[3]);
		ACENN_DEBUG("B: %d, %d, %d, %d", input_blockB[0], input_blockB[1], input_blockB[2], input_blockB[3]);
		int8_t *newA[4], *newB[4];
		for (int i = 0; i < chunk; i++) {
			newA[i] = (int8_t*)malloc(M*minblock*sizeof(int8_t));
			newB[i] = (int8_t*)malloc(minblock*N*sizeof(int8_t));
		}
		newA[3] = (int8_t*)malloc(M*rest*sizeof(int8_t));
		newB[3] = (int8_t*)malloc(rest*N*sizeof(int8_t));
		_split_bigmat_400K(input_blockA, input_blockB, M, K, N, newA, newB);
		for (int i = 0; i < M/us; i++) {
			for (int j = 0; j < N/us; j++) {
				for (int k = 0; k < chunk; k++) {
					int newK = minblock;
					int32_t h_ch=0,h_x=newK/us,v_ch=us,v_x=N/us,o_ch=1,o_x=N/us;
					int32_t id=0,bias_address=0,flow_loop_times=newK/us;
					int32_t stride_mode=0,transpose_mode=1;
					int32_t cutbit=4, shift_mode=0;
					int32_t h_addr=(int32_t)(uintptr_t)(newA[k] + us*newK*i);
					int32_t v_addr=(int32_t)(uintptr_t)(newB[k] + us*j);
					int32_t o_addr=(int32_t)(uintptr_t)(output_blockD + us*(N*i+j));
					int32_t conv_kernel=0, register_mode=0, flow_mode=2, work_mode=0;
					a=(v_ch<<25)|(v_x<<20)|(v_addr & 0xFFFFF);
					b=(h_ch<<25)|(h_x<<20)|(h_addr & 0xFFFFF);
					c=(o_ch<<25)|(o_x<<20)|(o_addr & 0xFFFFF);
					d=(cutbit<<18)|(shift_mode<<17)|(stride_mode<<16)|(transpose_mode<<14)|(conv_kernel<<12)|(register_mode<<10)|(flow_mode<<8)|(work_mode<<6)|flow_loop_times;
					e=1;
					f=(bias_address<<8)|id;
					__kuiloong_ace_msetins1(b,a);
					__kuiloong_ace_msetins2(d,c);
					__kuiloong_ace_msetins3(f,e);
				}

				int32_t newK = rest;
				int32_t h_ch=0,h_x=newK/us,v_ch=us,v_x=N/us,o_ch=1,o_x=N/us;
				int32_t id=0,bias_address=0,flow_loop_times=newK/us;
				int32_t stride_mode=0,transpose_mode=1;
				int32_t cutbit=4, shift_mode=0;
				int32_t h_addr=(int32_t)(uintptr_t)(newA[chunk] + us*newK*i);
				int32_t v_addr=(int32_t)(uintptr_t)(newB[chunk] + us*j);
				int32_t o_addr=(int32_t)(uintptr_t)(output_blockD + us*(N*i+j));
				int32_t conv_kernel=0, register_mode=0, flow_mode=0, work_mode=0;
				a=(v_ch<<25)|(v_x<<20)|(v_addr & 0xFFFFF);
				b=(h_ch<<25)|(h_x<<20)|(h_addr & 0xFFFFF);
				c=(o_ch<<25)|(o_x<<20)|(o_addr & 0xFFFFF);
				d=(cutbit<<18)|(shift_mode<<17)|(stride_mode<<16)|(transpose_mode<<14)|(conv_kernel<<12)|(register_mode<<10)|(flow_mode<<8)|(work_mode<<6)|flow_loop_times;
				e=1;
				f=(bias_address<<8)|id;
				__kuiloong_ace_msetins1(b,a);
				__kuiloong_ace_msetins2(d,c);
				__kuiloong_ace_msetins3(f,e);
			}
		}
		for (int i = 0; i <= chunk; i++) {
			free(newA[i]);
			free(newB[i]);
		}
	} else {
		int32_t h_ch=0,h_x=K/us,v_ch=us,v_x=N/us,o_ch=1,o_x=N/us;
		int32_t id=0,bias_address=0,flow_loop_times=K/us;
		int32_t stride_mode=0,transpose_mode=1;
		int32_t cutbit=4, shift_mode=0;
		for (int i = 0; i < M/us; i++) {
			for (int j = 0; j < N/us; j++) {
				int32_t h_addr=(int32_t)(uintptr_t)(input_blockA + us*K*i);
				int32_t v_addr=(int32_t)(uintptr_t)(input_blockB + us*j);
				int32_t o_addr=(int32_t)(uintptr_t)(output_blockD + us*(N*i + j));
				a=(v_ch<<25)|(v_x<<20)|(v_addr & 0xFFFFF);
				b=(h_ch<<25)|(h_x<<20)|(h_addr & 0xFFFFF);
				c=(o_ch<<25)|(o_x<<20)|(o_addr & 0xFFFFF);
				d=(cutbit<<18)|(shift_mode<<17)|(stride_mode<<16)|(transpose_mode<<14)|(0<<12)|(0<<10)|(0<<8)|(0<<6)|flow_loop_times;
				e=1;
				f=(bias_address<<8)|id;
				__kuiloong_ace_msetins1(b,a);
				__kuiloong_ace_msetins2(d,c);
				__kuiloong_ace_msetins3(f,e);
			}
		}
	}
}

#endif
