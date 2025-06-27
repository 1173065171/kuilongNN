//////////////////////////////////////////////////////////////////////////////////
// Author:          yintao liu - berialest@shu.edu.cn                           //
//                                                                              //
// Additional contributions by:                                                 //
//                                                                              //
// File Name:       xcvnn.hpp                                                   //
// Project Name:    Spirit                                                      //
// Language:        cpp                                                         //
//                                                                              //
// Description:     Renaming of the xcvnn instruction subset's built-in         //
// function                                                                     //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef __XCVNN_H__
#define __XCVNN_H__

#define SELECTVEUNUM    0x000
#define SELECTSAUNUM    0x001
#define VEUSTATUS       0x100
#define VEURADDR1       0x101
#define VEURADDR2       0x102
#define VEUWADDR        0x103
#define VEUCFG          0x104
#define VEUVLEN         0x105
#define VEUMASK         0x106
#define VEUDEBUG        0x107

#define SAUINS1LSB      0x200
#define SAUINS1MSB      0x201
#define SAUINS2LSB      0x202
#define SAUINS2MSB      0x203
#define SAUINS3LSB      0x204
#define SAUINS3MSB      0x205

//  00: uint8, 01: uint16, 10: int8, 11: int16
#define CFG_UI8             0b00
#define CFG_UI16            0b01
#define CFG_I8              0b10
#define CFG_I16             0b11

//  VEUCFG SETTING
#define RESET(value)        (value & 0b1)
#define SHIFT(value)        ((value & 0x3F) << 1)
#define MODE(value)         ((value & 0b11) << 7)
#define CLEAR_OB(value)     ((value & 0b1) << 9)
#define WRITE_BACK(value)   ((value & 0b1) << 10)
#define SCALAR_EN(value)    ((value & 0b1) << 11)
#define RADDR3_EN(value)    ((value & 0b1) << 12)

#define __kuiloong_ace_vorcsr(rs1, csr)         __builtin_riscv_ace_vorcsr(rs1, csr)        
#define __kuiloong_ace_vandcsr(rs1, csr)        __builtin_riscv_ace_vandcsr(rs1, csr)       
#define __kuiloong_ace_vsetcsr(rs1, csr)        __builtin_riscv_ace_vsetcsr(rs1, csr)       
#define __kuiloong_ace_vgetcsr(csr)             __builtin_riscv_ace_vgetcsr(0, csr)         
#define __kuiloong_ace_vadd(rs1, rs2)           __builtin_riscv_ace_vadd(rs1, rs2)          
#define __kuiloong_ace_vsub(rs1, rs2)           __builtin_riscv_ace_vsub(rs1, rs2)          
#define __kuiloong_ace_vmin(rs1, rs2)           __builtin_riscv_ace_vmin(rs1, rs2)          
#define __kuiloong_ace_vmax(rs1, rs2)           __builtin_riscv_ace_vmax(rs1, rs2)          
#define __kuiloong_ace_vredmin(rs1, rs2)        __builtin_riscv_ace_vredmin(rs1, rs2)       
#define __kuiloong_ace_vredmax(rs1, rs2)        __builtin_riscv_ace_vredmax(rs1, rs2)       
#define __kuiloong_ace_vand(rs1, rs2)           __builtin_riscv_ace_vand(rs1, rs2)          
#define __kuiloong_ace_vor(rs1, rs2)            __builtin_riscv_ace_vor(rs1, rs2)           
#define __kuiloong_ace_vxor(rs1, rs2)           __builtin_riscv_ace_vxor(rs1, rs2)          
#define __kuiloong_ace_vslideup(rs1, rs2)       __builtin_riscv_ace_vslideup(rs1, rs2)      
#define __kuiloong_ace_vslidedown(rs1, rs2)     __builtin_riscv_ace_vslidedown(rs1, rs2)    
#define __kuiloong_ace_vmv(rs1, rs2)            __builtin_riscv_ace_vmv(rs1, rs2)           
#define __kuiloong_ace_vssrl(rs1, rs2)          __builtin_riscv_ace_vssrl(rs1, rs2)         
#define __kuiloong_ace_vssra(rs1, rs2)          __builtin_riscv_ace_vssra(rs1, rs2)         
#define __kuiloong_ace_vnclip(rs1, rs2)         __builtin_riscv_ace_vnclip(rs1, rs2)        
#define __kuiloong_ace_vwredsum(rs1, rs2)       __builtin_riscv_ace_vwredsum(rs1, rs2)      
#define __kuiloong_ace_vredsum(rs1, rs2)        __builtin_riscv_ace_vredsum(rs1, rs2)       
#define __kuiloong_ace_vcompress(rs1, rs2)      __builtin_riscv_ace_vcompress(rs1, rs2)     
#define __kuiloong_ace_vmac(rs1, rs2)           __builtin_riscv_ace_vmac(rs1, rs2)          
#define __kuiloong_ace_vmadd(rs1, rs2)          __builtin_riscv_ace_vmadd(rs1, rs2)         
#define __kuiloong_ace_vmul(rs1, rs2)           __builtin_riscv_ace_vmul(rs1, rs2)          
#define __kuiloong_ace_vmulhsu(rs1, rs2)        __builtin_riscv_ace_vmulhsu(rs1, rs2)       
#define __kuiloong_ace_vmulh(rs1, rs2)          __builtin_riscv_ace_vmulh(rs1, rs2)         

#define __kuiloong_ace_msetins1(rs1, rs2)       __builtin_riscv_ace_msetins1(rs1, rs2)      
#define __kuiloong_ace_mgetins1lsb()            __builtin_riscv_ace_mgetins1lsb(0, 0)       
#define __kuiloong_ace_mgetins1msb()            __builtin_riscv_ace_mgetins1msb(0, 0)       
#define __kuiloong_ace_msetins2(rs1, rs2)       __builtin_riscv_ace_msetins2(rs1, rs2)      
#define __kuiloong_ace_mgetins2lsb()            __builtin_riscv_ace_mgetins2lsb(0, 0)       
#define __kuiloong_ace_mgetins2msb()            __builtin_riscv_ace_mgetins2msb(0, 0)       
#define __kuiloong_ace_msetins3(rs1, rs2)       __builtin_riscv_ace_msetins3(rs1, rs2)      
#define __kuiloong_ace_mgetins3lsb()            __builtin_riscv_ace_mgetins3lsb(0, 0)       
#define __kuiloong_ace_mgetins3msb()            __builtin_riscv_ace_mgetins3msb(0, 0)       

#endif