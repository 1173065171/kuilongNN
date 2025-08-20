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
// #define KUILOONG_DEBUG

// switch of memory allocation debug
#define DEBUG_MALLOC 1

/****************************************************************************************************/
/*                                      buddy algorithms                                            */
/****************************************************************************************************/
//  frac{4*(1-2^{9})}{1-2} = 2044 Byte
//  sram 最大 0x1010_0000 = 1MB
#define MEM_END    0x10090000
#define MEM_START  0x10070000

#endif