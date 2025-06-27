//////////////////////////////////////////////////////////////////////////////////
// Author:          yintao liu - berialest@shu.edu.cn                           //
//                                                                              //
// Additional contributions by:                                                 //
//                                                                              //
// File Name:       UART.cpp                                                    //
// Project Name:    Spirit                                                      //
// Language:        cpp                                                         //
//                                                                              //
// Description:     The source file of UART model                               //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#include "UART.hpp"
#include <fstream>
#include <iostream>

UART::UART() : Device()
{
    this->reg0  = 0;
    this->reg1  = 0;
    this->reg2  = 0;
    this->reg3  = 0;
}

UART::~UART()
{

}

void UART::set_file_name(char* file_name)
{
    //  P.S. deep copy must be used here, because file_name is a temporary variable
    this->uart_file_name = (char*)malloc((strlen(file_name)+1) * sizeof(char));
    strcpy(this->uart_file_name, file_name);
}

void UART::init()
{
}

void UART::clean()
{
    FILE  *fp;
    fp=fopen(this->uart_file_name,"w"); //  parameter "a" represents additional writing
    fclose(fp);
}

void UART::run()
{
    if (this->get_peripherals_bus_enable()) {
        if (this->get_peripherals_bus_wstrb() == 0) {
            if ((this->get_peripherals_bus_addr() & 0x0000000F) == 0x0)
                this->set_peripherals_bus_rdata(this->reg0);
            if ((this->get_peripherals_bus_addr() & 0x0000000F) == 0x4)
                this->set_peripherals_bus_rdata(this->reg1);
            if ((this->get_peripherals_bus_addr() & 0x0000000F) == 0x8)
                this->set_peripherals_bus_rdata(this->reg2);
            if ((this->get_peripherals_bus_addr() & 0x0000000F) == 0xC)
                this->set_peripherals_bus_rdata(this->reg3);
        } else {
            FILE  *fp;
            fp=fopen(this->uart_file_name,"a"); //  parameter "a" represents additional writing
            // fprintf(fp,"%c(%x)", this->get_peripherals_bus_wdata(), this->get_peripherals_bus_wdata());
            fprintf(fp,"%c", this->get_peripherals_bus_wdata());
            fclose(fp);
        }
    }
    this->set_peripherals_bus_enable(0);
    this->set_peripherals_bus_wstrb(0);
    this->set_peripherals_bus_addr(0);
    this->set_peripherals_bus_wdata(0);
    this->set_peripherals_bus_rdata(0);
}
