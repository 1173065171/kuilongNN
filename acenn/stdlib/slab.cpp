//////////////////////////////////////////////////////////////////////////////////
// Author:          yintao liu - berialest@shu.edu.cn                           //
//                                                                              //
// Additional contributions by:                                                 //
//                                                                              //
// File Name:       slab.cpp                                                    //
// Project Name:    ACENN                                                       //
// Language:        cpp                                                         //
//                                                                              //
// Description:     Slab memory management algorithms                           //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef __SLAB_CPP__
#define __SLAB_CPP__

#include "stdlib/slab.hpp"

struct kmem_cache kmalloc_cache[KMALLOC_CACHE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};

int find_right_order(uint32_t size)
{
    int order = 0;
    for (; order <= KMEM_CACHE_MAX_ORDER; ++order) {
        if (size <= (KMEM_CACHE_MAX_WAST) * (1 << order)) {
// #ifdef DEBUG_MALLOC
//             ACENN_DEBUG("size: %x", size);
// #endif
            return order;
        }
    }
    if (size > (1 << order)) {
// #ifdef DEBUG_MALLOC
//         ACENN_DEBUG("size: %x", size);
// #endif
        return -1;
    }
// #ifdef DEBUG_MALLOC
//     ACENN_DEBUG("size: %x", size);
// #endif
    return order;
}

int kmem_cache_line_object(void *head, uint32_t size, int order)
{
    void **pl;
    char *p;
    pl = (void **)head;
    p = (char *)head + size;
    int i = 0;
    int s = PAGE_SIZE * (1 << order);
    for (; s > size; ++i, s -= size) {
        *pl = (void *)p;
        pl = (void **)p;
        p = p + size;
    }
    if (s == size) {
        ++i;
    }
    return i;
}

struct kmem_cache *kmem_cache_create(struct kmem_cache *cache, uint32_t size, uint32_t flags)
{
    void **nf_block = &(cache->nf_block);

    int order = find_right_order(size);
// #ifdef DEBUG_MALLOC
//     ACENN_DEBUG("order: %d", order);
// #endif
    if (order == -1) {
        return nullptr;
    }
    if ((cache->head_page = alloc_pages(0, order)) == nullptr) {
        return nullptr;
    }
    *nf_block = page_address(cache->head_page);
    cache->obj_nr = kmem_cache_line_object(*nf_block, size, order);
    cache->obj_size = size;
    cache->page_order = order;
    cache->flags = flags;
    cache->end_page = BUDDY_END(cache->head_page, order);
    cache->end_page->list.next = nullptr;
    return cache;
}

void kmem_cache_destroy(struct kmem_cache *cache)
{
    int order = cache->page_order;
    struct page *pg = cache->head_page;
    struct list_head *list;
    while(1) {
        list = BUDDY_END(pg, order)->list.next;
        free_pages(pg, order);
        if (list) {
            pg = list_entry(list, struct page, list);
        } else {
            return ;
        }
    }
}

void kmem_cache_free(struct kmem_cache *cache, void *objp)
{
    *(void **)objp = cache->nf_block;
    cache->nf_block = objp;
    cache->obj_nr++;
}

void *kmem_cache_alloc(struct kmem_cache *cache, uint32_t flag)
{
    void *p;
    struct page *pg;
    if (cache == nullptr) {
        return nullptr;
    }
    void **nf_block = &(cache->nf_block);
    uint32_t *nr = &(cache->obj_nr);
    int order = cache->page_order;

    if (!*nr) {
        if ((pg = alloc_pages(0, order)) == nullptr) {
            return nullptr;
        }
        *nf_block = page_address(pg);
        cache->end_page->list.next = &pg->list;
        cache->end_page = BUDDY_END(pg, order);
        cache->end_page->list.next = nullptr;
        *nr += kmem_cache_line_object(*nf_block, cache->obj_size, order);
    }
    (*nr)--;
    p = *nf_block;
    *nf_block = *(void **)p;
    pg = virt_to_page((uint32_t)p);
    pg->cachep = cache;
    return p;
}

int kmalloc_init(void)
{
    for (int i = 0; i < KMALLOC_CACHE_SIZE; ++i) {
        if (kmem_cache_create(&kmalloc_cache[i], (i+1) * KMALLOC_MINIMAL_SIZE_BIAS, 0) == nullptr) {
            return -1;
        }
#ifdef DEBUG_MALLOC
        ACENN_DEBUG("order: %d, address: %x, size: %x", kmalloc_cache[i].page_order, page_address(kmalloc_cache[i].head_page), kmalloc_cache[i].obj_size);
#endif
    }
    return 0;
}

void *kmalloc(uint32_t size)
{
    int index = kmalloc_cache_size_to_index(size);
    void *addr = (void *)kmem_cache_alloc(&kmalloc_cache[index], 0);
    if (index >= KMALLOC_CACHE_SIZE) {
        return nullptr;
    }
#ifdef DEBUG_MALLOC
    ACENN_DEBUG("slab order: %d", kmalloc_cache[index].page_order);
    ACENN_DEBUG("slab size: %x", kmalloc_cache[index].obj_size);
    ACENN_DEBUG("slab addr: %x", (uint32_t)addr);
#endif
    return addr;
}

void kfree(void *addr) {
    struct page *pg;
    pg = virt_to_page((uint32_t)addr);
    kmem_cache_free(pg->cachep, addr);
}

#endif
