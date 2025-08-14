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

#ifndef __VECTOR_VMUL_HPP__
#define __VECTOR_VMUL_HPP__

#include "stdint.h"
#include "common/common.hpp"
#include "common/xcvnn.hpp"


int8_t* VecMul(int8_t *input_block1, int8_t *input_block2, int M, int K, int N)
{

    int8_t *output_vector = (int8_t*)malloc(M*K * sizeof(int8_t));

    for(int i=0;i<M*K;i++){
        uint32_t raddr2 = (uint32_t)(uintptr_t)input_block1 + N*i;
        uint32_t waddr = (uint32_t)(uintptr_t)output_vector + N*i;

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
        __kuiloong_ace_vmul(input_block2[i], raddr2);
    }

    free(input_block1);
    free(input_block2);
    return output_vector;
}

#endif
