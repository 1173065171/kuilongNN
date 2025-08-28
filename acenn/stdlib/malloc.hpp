//////////////////////////////////////////////////////////////////////////////////
// Author:          yintao liu - berialest@shu.edu.cn                           //
//                                                                              //
// Additional contributions by:                                                 //
//                                                                              //
// File Name:       malloc.hpp                                                  //
// Project Name:    ACENN                                                       //
// Language:        cpp                                                         //
//                                                                              //
// Description:     Memory management                                           //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef __MALLOC_HPP__
#define __MALLOC_HPP__

#include "stdlib/config.hpp"

#ifdef USE_SLAB
#include "stdlib/slab.hpp"
#include "stdlib/slab.hpp"
#elif USE_FIRST_FIT
#include "stdlib/printf.hpp"
#include "stdlib/first_fit.hpp"
#endif

int malloc_init(void);

void *malloc(uint32_t size);

void free(void *addr);

void malloc_initial();

#ifdef USE_SLAB
void test_malloc(void);

void test_malloc_boundary(void);
#endif

#endif
