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

#ifndef __VECTOR_RELU_HPP__
#define __VECTOR_RELU_HPP__

#include "stdint.h"
#include "common/common.hpp"
#include "common/xcvnn.hpp"


void relu(int8_t *input_block, int M, int K, int col, int8_t *output_vector)
{
    ACENN_DEBUG("VEU gather_trick START: In[%d,%d] col = %d -> Out[%d]", M, K, col, K);
    uint32_t raddr = (uint32_t)(uintptr_t)(input_block + col*K);
    uint32_t waddr = (uint32_t)(uintptr_t)output_vector;

  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(1) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(8*K, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vmax(0, waddr);
}

#endif
