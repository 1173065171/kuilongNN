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

#ifndef __VECTOR_SOFTMAX_HPP__
#define __VECTOR_SOFTMAX_HPP__

#include "stdint.h"
#include "common/common.hpp"
#include "common/xcvnn.hpp"


int8_t* lookup_softmax8(int8_t *input_vector, int N) {
  // ACENN_DEBUG("VEU lookup_softmax8 START: In[%d] N = %d -> Out[%d]", N, N, N);
  int8_t* max_val = (int8_t*)malloc(sizeof(int8_t));
  uint8_t *output_vector = (uint8_t*)malloc(N * sizeof(uint8_t));
  *max_val = input_vector[0];
  uint32_t raddr = (uint32_t)(uintptr_t)input_vector;
  uint32_t waddr = (uint32_t)(uintptr_t)max_val;
  if (N < 16) {
    for (int i = 1; i < N; ++i) {
      if (input_vector[i] > *max_val)
        *max_val = input_vector[i];
    }
  } else {
    __kuiloong_ace_vsetcsr(
                            SCALAR_EN(0) |
                            WRITE_BACK(1) |
                            CLEAR_OB(1) |
                            MODE(CFG_I8) |
                            SHIFT(0) |
                            RESET(0), VEUCFG);
    __kuiloong_ace_vsetcsr(8*N, VEUVLEN);
    __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
    __kuiloong_ace_vsetcsr(waddr, VEUWADDR);
    __kuiloong_ace_vredmax(raddr);
  }
  for (int i = 0; i < N; ++i) {
    int16_t shifted = (int16_t)input_vector[i] - *max_val;
    uint8_t index = (shifted < -255) ? 0 : (uint8_t)(shifted + 255);
    output_vector[i] = piecewise_exp_lut_32[index];
  }
  free(max_val);
  free(input_vector);
  return output_vector;
}


#endif
