//////////////////////////////////////////////////////////////////////////////////
// Author:          yintao liu - berialest@shu.edu.cn                           //
//                                                                              //
// Additional contributions by:                                                 //
//                                                                              //
// File Name:       slab.hpp                                                    //
// Project Name:    ACENN                                                       //
// Language:        cpp                                                         //
//                                                                              //
// Description:     Slab memory management algorithms                           //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef __SLAB_HPP__
#define __SLAB_HPP__

#include "cstdlib"
#include "stdlib/config.hpp"
#include "stdlib/printf.hpp"
#include "stdlib/buddy.hpp"

typedef unsigned int uint32_t;

#define KMEM_CACHE_DEFAULT_ORDER    (0)
#define KMEM_CACHE_MAX_ORDER        (3)
#define KMEM_CACHE_SAVE_RATE        (0x32)
#define KMEM_CACHE_PERCENT          (0x64)
#define KMEM_CACHE_MAX_WAST         (PAGE_SIZE - KMEM_CACHE_SAVE_RATE * PAGE_SIZE / KMEM_CACHE_PERCENT)

struct kmem_cache {
    uint32_t obj_size;
    uint32_t obj_nr;
    uint32_t page_order;
    uint32_t flags;
    struct page *head_page;
    struct page *end_page;
    void *nf_block;
};

//  2^{4} = 16 Byte = 128 Bit
#define KMALLOC_BIAS_SHIFT          (4)
#define SLAB_MAX_SIZE               (256)
#define KMALLOC_MINIMAL_SIZE_BIAS   (1 << (KMALLOC_BIAS_SHIFT))
#define KMALLOC_CACHE_SIZE          (SLAB_MAX_SIZE / KMALLOC_MINIMAL_SIZE_BIAS)
#define kmalloc_cache_size_to_index(size)   ((size) >> (KMALLOC_BIAS_SHIFT))

int kmalloc_init(void);

void *kmalloc(uint32_t size);

void kfree(void *addr);

#endif
