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

#ifndef __MATRIX_GEMM_HPP__
#define __MATRIX_GEMM_HPP__

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

int8_t* Gemm(int8_t *input_blockA, int8_t *input_blockB, int M, int K, int N, int cutbit) {
  const int us=8;
// ACENN_DEBUG("SAU matmul_multi8 START: A[%d,%d] B[%d,%d] -> D[%d,%d]", M, K, K, N, M, N);
  int8_t *output_blockD = (int8_t*)malloc(M*N * sizeof(int8_t));
  // 配置寄存器
  uint32_t ins1,ins2,ins3,ins4,ins5,ins6,ins7,ins8;
  uint32_t a_addr,b_addr,d_addr;
  uint32_t a_ch,b_ch,d_ch,a_x,b_x,d_x;
  a_ch=0,a_x=K/us,b_ch=us,b_x=N/us,d_ch=1,d_x=N/us;
  int32_t id=0,bias_address=0;
  int32_t stride_mode=0,transpose_mode=1;
  int32_t shift_mode=0;
  int32_t register_mode=0, conv_kernel=0, work_mode=0;
  for (int i = 0; i < M/us; i++) {
    for (int j = 0; j < N/us; j++) {
      int32_t flow_loop_times=K/us;
      int32_t new_flow_loop_times;
      int32_t flow_mode=0;
      if (flow_loop_times >= 64) {
        // flow_times分段 256byte / 32次
        int chunk = K/us/32;
        int rest = flow_loop_times - 32*chunk;
        if (rest == 0) {
          for (int k = 0; k < chunk; k++) {
            new_flow_loop_times = 32;
            if (k == chunk - 1)
              flow_mode = 0;
            else
              flow_mode = 2;
            a_addr=(int32_t)(uintptr_t)(input_blockA + 256*k + us*K*i);
            b_addr=(int32_t)(uintptr_t)(input_blockB + N*256*k + us*j);
            d_addr=(int32_t)(uintptr_t)(output_blockD + us*(N*i + j));
            ins1 = (transpose_mode&0x03)|((cutbit&0x1f) << 2);
            ins2 = (register_mode&0x3)|((conv_kernel&0x3)<<2)|((stride_mode&0x1)<<4)|((shift_mode&0x1)<< 5);
            ins3 = (b_ch&0xFF)|((a_ch&0xFF)<<8)|((d_ch&0xFF)<<16);
            ins4 = (b_x&0x7F)|((a_x&0x7F)<<8)|((d_x&0x7F)<<16);
            ins5 = a_addr&0xFFFFF;
            ins6 = b_addr&0xFFFFF;
            ins7 = (0&0xFFFFF)|((work_mode&0x3)<<20)|((flow_mode&0x3)<<22)|((new_flow_loop_times&0x3F)<<24);
            ins8 = (1&0x1)|((id&0xFF)<<1)|((d_addr&0xFFFFF)<<9);
            __kuiloong_ace_msetins1(ins1, ins2);
            __kuiloong_ace_msetins2(ins3, ins4);
            __kuiloong_ace_msetins3(ins5, ins6);
            __kuiloong_ace_msetins4(ins7, ins8);
          }
        } else {
          for (int k = 0; k < chunk; k++) {
            new_flow_loop_times = 32;
            flow_mode = 2;
            a_addr=(int32_t)(uintptr_t)(input_blockA + 256*k + us*K*i);
            b_addr=(int32_t)(uintptr_t)(input_blockB + N*256*k + us*j);
            d_addr=(int32_t)(uintptr_t)(output_blockD + us*(N*i + j));
            ins1 = (transpose_mode&0x03)|((cutbit&0x1f) << 2);
            ins2 = (register_mode&0x3)|((conv_kernel&0x3)<<2)|((stride_mode&0x1)<<4)|((shift_mode&0x1)<< 5);
            ins3 = (b_ch&0xFF)|((a_ch&0xFF)<<8)|((d_ch&0xFF)<<16);
            ins4 = (b_x&0x7F)|((a_x&0x7F)<<8)|((d_x&0x7F)<<16);
            ins5 = a_addr&0xFFFFF;
            ins6 = b_addr&0xFFFFF;
            ins7 = (0&0xFFFFF)|((work_mode&0x3)<<20)|((flow_mode&0x3)<<22)|((new_flow_loop_times&0x3F)<<24);
            ins8 = (1&0x1)|((id&0xFF)<<1)|((d_addr&0xFFFFF)<<9);
            __kuiloong_ace_msetins1(ins1, ins2);
            __kuiloong_ace_msetins2(ins3, ins4);
            __kuiloong_ace_msetins3(ins5, ins6);
            __kuiloong_ace_msetins4(ins7, ins8);
          }
          new_flow_loop_times = rest;
          flow_mode = 0;
          a_addr=(int32_t)(uintptr_t)(input_blockA + 256*(chunk-1) + rest*us + us*K*i);
          b_addr=(int32_t)(uintptr_t)(input_blockB + N*256*(chunk-1) + N*rest*us + us*j);
          d_addr=(int32_t)(uintptr_t)(output_blockD + us*(N*i + j));
          ins1 = (transpose_mode&0x03)|((cutbit&0x1f) << 2);
          ins2 = (register_mode&0x3)|((conv_kernel&0x3)<<2)|((stride_mode&0x1)<<4)|((shift_mode&0x1)<< 5);
          ins3 = (b_ch&0xFF)|((a_ch&0xFF)<<8)|((d_ch&0xFF)<<16);
          ins4 = (b_x&0x7F)|((a_x&0x7F)<<8)|((d_x&0x7F)<<16);
          ins5 = a_addr&0xFFFFF;
          ins6 = b_addr&0xFFFFF;
          ins7 = (0&0xFFFFF)|((work_mode&0x3)<<20)|((flow_mode&0x3)<<22)|((new_flow_loop_times&0x3F)<<24);
          ins8 = (1&0x1)|((id&0xFF)<<1)|((d_addr&0xFFFFF)<<9);
          __kuiloong_ace_msetins1(ins1, ins2);
          __kuiloong_ace_msetins2(ins3, ins4);
          __kuiloong_ace_msetins3(ins5, ins6);
          __kuiloong_ace_msetins4(ins7, ins8);
        }
      } else {
        a_addr=(int32_t)(uintptr_t)(input_blockA + us*K*i);
        b_addr=(int32_t)(uintptr_t)(input_blockB + us*j);
        d_addr=(int32_t)(uintptr_t)(output_blockD + us*(N*i + j));
        ins1 = (transpose_mode&0x03)|((cutbit&0x1f) << 2);
        ins2 = (register_mode&0x3)|((conv_kernel&0x3)<<2)|((stride_mode&0x1)<<4)|((shift_mode&0x1)<< 5);
        ins3 = (b_ch&0xFF)|((a_ch&0xFF)<<8)|((d_ch&0xFF)<<16);
        ins4 = (b_x&0x7F)|((a_x&0x7F)<<8)|((d_x&0x7F)<<16);
        ins5 = a_addr&0xFFFFF;
        ins6 = b_addr&0xFFFFF;
        ins7 = (0&0xFFFFF)|((work_mode&0x3)<<20)|((flow_mode&0x3)<<22)|((flow_loop_times&0x3F)<<24);
        ins8 = (1&0x1)|((id&0xFF)<<1)|((d_addr&0xFFFFF)<<9);
        __kuiloong_ace_msetins1(ins1, ins2);
        __kuiloong_ace_msetins2(ins3, ins4);
        __kuiloong_ace_msetins3(ins5, ins6);
        __kuiloong_ace_msetins4(ins7, ins8);
      }
    }
  }
  free(input_blockA);
  return output_blockD;
}

#endif
