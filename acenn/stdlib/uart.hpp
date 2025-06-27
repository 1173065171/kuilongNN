//////////////////////////////////////////////////////////////////////////////////
// Author:          yintao liu - berialest@shu.edu.cn                           //
//                                                                              //
// Additional contributions by:                                                 //
//                                                                              //
// File Name:       uart.hpp                                                    //
// Project Name:    ACENN                                                       //
// Language:        cpp                                                         //
//                                                                              //
// Description:     Output and read characters through UART                     //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef __UART_HPP__
#define __UART_HPP__

#define USE_XILINX_UARTLITE 1

#ifdef USE_XILINX_UARTLITE
    void uart_putc(int c);

    void uart_puts(char *p);
#endif

#endif
