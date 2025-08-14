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

#ifndef __VECTOR_VADD_HPP__
#define __VECTOR_VADD_HPP__

#include "stdint.h"
#include "common/common.hpp"
#include "common/xcvnn.hpp"


int8_t* VecAdd(int8_t *input_block, int8_t *bias, int M, int K)
{
    ACENN_DEBUG("Get in VecAdd");
    ACENN_DEBUG("malloc size-- M:%d, K:%d", M, K);
    int8_t *output_vector = (int8_t*)malloc(M*K * sizeof(int8_t));
    
    uint32_t raddr1 = (uint32_t)(uintptr_t)input_block;
    uint32_t raddr2 = (uint32_t)(uintptr_t)bias;
    uint32_t waddr = (uint32_t)(uintptr_t)output_vector;

    __kuiloong_ace_vsetcsr( 
                            SCALAR_EN(0) | 
                            WRITE_BACK(1) | 
                            CLEAR_OB(1) | 
                            MODE(CFG_I8) | 
                            SHIFT(0) | 
                            RESET(0), VEUCFG);
    __kuiloong_ace_vsetcsr(8*M*K, VEUVLEN);
    __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
    __kuiloong_ace_vsetcsr(waddr, VEUWADDR);
    __kuiloong_ace_vadd(raddr1, raddr2);

    free(input_block);
    return output_vector;
}

#endif
