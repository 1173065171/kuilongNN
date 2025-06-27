//////////////////////////////////////////////////////////////////////////////////
// Author:          yintao liu - berialest@shu.edu.cn                           //
//                                                                              //
// Additional contributions by:                                                 //
//                                                                              //
// File Name:       UART.hpp                                                    //
// Project Name:    Spirit                                                      //
// Language:        cpp                                                         //
//                                                                              //
// Description:     The header file of UART model                               //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#ifndef __UART_H__
#define __UART_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <climits>
#include "../Device/Device.hpp"

class UART : public Device
{
    public:
        UART();
        ~UART();
        
        void init() override;
        void run() override;
        void set_file_name(char* file_name);
        void clean();

    private:
        char* uart_file_name;

        uint32_t reg0;
        uint32_t reg1;
        uint32_t reg2;
        uint32_t reg3;
};

#endif