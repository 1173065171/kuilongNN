//////////////////////////////////////////////////////////////////////////////////
// Author:          yintao liu - berialest@shu.edu.cn                           //
//                                                                              //
// Additional contributions by:                                                 //
//                                                                              //
// File Name:       malloc.cpp                                                  //
// Project Name:    ACENN                                                       //
// Language:        cpp                                                         //
//                                                                              //
// Description:     Memory management                                           //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef __MALLOC_CPP__
#define __MALLOC_CPP__

#include "stdlib/malloc.hpp"

typedef unsigned int uint32_t;

int buddy_addr[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int buddy_order[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int buddy_use[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#define USE_SLAB 1

int malloc_init(void)
{   
    #ifdef USE_SLAB
        return kmalloc_init();
    #else
        return kmalloc_init();
    #endif
}

void *malloc(uint32_t size)
{
    // ACENN_DEBUG("malloc size: %x", size);
#ifdef USE_SLAB
        // FIXME：此处为两种分配的兼容性代码，请根据分配需要使用
        // 阈值判断的方法可能会造成malloc卡顿
        if (size < SLAB_MAX_SIZE) {
            return kmalloc(size);
        } else if (size <= BUDDY_MAX_SIZE) {
            int order = get_buddy_order(size);
            void *addr = (void *)get_free_pages(0,order);
            for (int i=0; i<10; ++i) {
                if (buddy_use[i] == 0) {
                    buddy_use[i] = 1;
                    buddy_addr[i] = (uint32_t)addr;
                    buddy_order[i] = order;
#ifdef DEBUG_MALLOC
                    ACENN_DEBUG("buddy order: %d", order);
                    ACENN_DEBUG("buddy size: %x", (1 << order)*PAGE_SIZE);
                    ACENN_DEBUG("buddy addr: %x", (uint32_t)addr);
#endif
                    return addr;
                }
            }
#ifdef DEBUG_MALLOC
            ACENN_DEBUG("10 buddy blocks have already been allocated, please release them before applying for memory allocation!");
            asm("ebreak;"); // 中断，停止运行
#endif
            return nullptr;
        }
#ifdef DEBUG_MALLOC
        ACENN_DEBUG("The memory size you want to allocate is %d, but the maximum size that malloc can allocate is %d", size, BUDDY_MAX_SIZE);
        asm("ebreak;"); // 中断，停止运行
#endif
        return nullptr;

#else
        return kmalloc(size);
#endif
}

void free(void *addr)
{
    #ifdef USE_SLAB
#ifdef DEBUG_MALLOC
        ACENN_DEBUG("free addr: %x", (uint32_t)addr);
#endif
        for (int i=9; i>=0; i--) {
            if (buddy_use[i] && (buddy_addr[i] == (uint32_t)addr)) {
                buddy_use[i] = 0;
                put_free_pages((void *)buddy_addr[i], buddy_order[i]);
                return ;
            }
        }
        kfree(addr);
    #else
        kfree(addr);
    #endif
}

void malloc_initial()
{
    init_page_map();
	int result = malloc_init();
    if (result == -1) {
        printf_("failed to malloc_init!\n");
    }
}

void test_malloc(void)
{
    // test malloc function
    init_page_map();
	int result = malloc_init();
    if (result == -1) {
        printf_("failed to malloc_init!\n");
    }
	char *p1, *p2, *p3, *p4;
	p1 = (char *)malloc(127);
	printf_("the first alloced address is %x\n",p1);
	p2 = (char *)malloc(124);
	printf_("the second alloced address is %x\n",p2);
	free(p1);
	free(p2);
	p3 = (char *)malloc(119);
	printf_("the third alloced address is %x\n",p3);
	p4 = (char *)malloc(512);
	printf_("the forth alloced address is %x\n",p4);
    free(p3);
	free(p4);

    p1 = (char *)malloc(4096);
	printf_("the first alloced address is %x\n",p1);
	free(p1);
}


void test_malloc_boundary(void)
{
    // test slab/buddy function
    init_page_map();
	int result = malloc_init();
    if (result == -1) {
        printf_("failed to malloc_init!\n");
    }
	char *p1, *p2, *p3;

	p1 = (char *)malloc(128);
	printf_("the first alloced address is %x\n",p1);
	p2 = (char *)malloc(128);
	printf_("the second alloced address is %x\n",p2);
	p3 = (char *)malloc(128);
	printf_("the third alloced address is %x\n",p3);

	free(p1);
    free(p2);
    free(p3);

    p1 = (char *)malloc(256);
	printf_("the first alloced address is %x\n",p1);
	p2 = (char *)malloc(256);
	printf_("the second alloced address is %x\n",p2);
	p3 = (char *)malloc(256);
	printf_("the third alloced address is %x\n",p3);

	free(p1);
    free(p2);
    free(p3);

    p1 = (char *)malloc(512);
	printf_("the first alloced address is %x\n",p1);
	p2 = (char *)malloc(512);
	printf_("the second alloced address is %x\n",p2);
	p3 = (char *)malloc(512);
	printf_("the third alloced address is %x\n",p3);

	free(p1);
    free(p2);
    free(p3);
}

#endif
