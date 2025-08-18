//////////////////////////////////////////////////////////////////////////////////
// Author:          yintao liu - berialest@shu.edu.cn                           //
//                                                                              //
// Additional contributions by:                                                 //
//                                                                              //
// File Name:       printf.hpp                                                  //
// Project Name:    ACENN                                                       //
// Language:        cpp                                                         //
//                                                                              //
// Description:     Print characters to UART                                    //
// refer: https://github.com/mpaland/printf/tree/master                         //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef __PRINTF_HPP__
#define __PRINTF_HPP__

#include "stdlib/uart.hpp"

#include "stdbool.h"
#include "stdint.h"
#include "stddef.h"
#include "stdarg.h"

#define KUILOONG_DEBUG

// 条件编译 print功能是否执行， 不执行可以减少cpu操作数
#ifndef KUILOONG_DEBUG

#define ACENN_DEBUG(fmt,...);  do { \
} while(0);

#else

#define ACENN_DEBUG(fmt,...);  do { \
    printf_("[%s : %s : %05d] ", __FILE__ , __func__, __LINE__); \
    printf_(fmt, ## __VA_ARGS__); \
    printf_("\n"); \
} while(0);

#endif

int printf_(const char* format, ...);

void test_printf_(void);

#endif
