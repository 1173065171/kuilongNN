//////////////////////////////////////////////////////////////////////////////////
// Author:          yintao liu - berialest@shu.edu.cn                           //
//                                                                              //
// Additional contributions by:                                                 //
//                                                                              //
// File Name:       uart.cpp                                                    //
// Project Name:    ACENN                                                       //
// Language:        cpp                                                         //
//                                                                              //
// Description:     Output and read characters through UART                     //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef __UART_CPP__
#define __UART_CPP__

#include "stdlib/uart.hpp"

#pragma once
#define REG_WRITE(address, value)   (*(volatile uint32_t *)(address)) = (value)

#pragma once
#define REG_READ(address)           (*(volatile uint32_t *)(address))

typedef unsigned int uint32_t;

#ifdef USE_XILINX_UARTLITE
    #define UART_ADDRESS_RX     0x20000000
    #define UART_ADDRESS_TX     0x20000004
    #define UART_ADDRESS_STAT   0x20000008
    #define UART_ADDRESS_CTRL   0x2000000C

    void uart_putc(int c)
    {
        // while ((REG_READ(UART_ADDRESS_STAT) & 0b1000) == 0b1000);
        REG_WRITE(UART_ADDRESS_TX, c);
    }

    void uart_puts(char *p)
    {
        while (*p) {
            // while ((REG_READ(UART_ADDRESS_STAT) & 0b1000) == 0b1000);
            REG_WRITE(UART_ADDRESS_TX, *(p++));
        }
    }
#endif

#endif
