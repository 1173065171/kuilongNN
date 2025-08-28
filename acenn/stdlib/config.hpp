//////////////////////////////////////////////////////////////////////////////////
// Author:          yintao liu - berialest@shu.edu.cn                           //
//                                                                              //
// Additional contributions by:                                                 //
//                                                                              //
// File Name:       config.hpp                                                  //
// Project Name:    ACENN                                                       //
// Language:        cpp                                                         //
//                                                                              //
// Description:     Some configs                                                //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

/****************************************************************************************************/
/*                                      debug flag                                                  */
/****************************************************************************************************/
// switch of ACENN_DEBUG() print func
#define KUILOONG_DEBUG

// switch of memory allocation debug
#define DEBUG_MALLOC 1

/****************************************************************************************************/
/*                                      buddy algorithms                                            */
/****************************************************************************************************/
//  frac{4*(1-2^{9})}{1-2} = 2044 Byte
//  sram 最大 0x1001_0000 = 64KB
#define MEM_END    0x1003E000
#define MEM_START  0x10030000

// 一个页的大小
#define PAGE_SHIFT_ 10
// buddy算法最大分配的阶
#define MAX_PAGE_NUM_EXP 4

#endif

// select malloc algo
// #define USE_SLAB 1
#define USE_FIRST_FIT 1