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


int8_t* Relu(int8_t *input_block, int M, int N)
{
    // 参考 Softmax8 的内存分配模式
    ACENN_DEBUG("Get in Relu")
    int8_t *output_vector = (int8_t*)malloc(M * N * sizeof(int8_t));
    
    uint32_t raddr = (uint32_t)(uintptr_t)input_block;
    uint32_t waddr = (uint32_t)(uintptr_t)output_vector;
    
    int total_elements = M * N;

    __kuiloong_ace_vsetcsr( 
                            SCALAR_EN(0) |      // ReLU不需要标量使能
                            WRITE_BACK(1) | 
                            CLEAR_OB(1) | 
                            MODE(CFG_I8) | 
                            SHIFT(0) | 
                            RESET(0), VEUCFG);
    __kuiloong_ace_vsetcsr(8 * total_elements, VEUVLEN);
    __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
    __kuiloong_ace_vsetcsr(waddr, VEUWADDR);
    __kuiloong_ace_vmax(0, raddr);  // 与0比较取最大值实现ReLU
    
    // 参考 Softmax8 的内存管理模式
    free(input_block);
    
    return output_vector;
}

#endif
