//////////////////////////////////////////////////////////////////////////////////
// Author:          yintao liu - berialest@shu.edu.cn                           //
//                                                                              //
// Additional contributions by:                                                 //
//                                                                              //
// File Name:       buddy.cpp                                                   //
// Project Name:    ACENN                                                       //
// Language:        cpp                                                         //
//                                                                              //
// Description:     Buddy memory management algorithms                          //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#ifndef __BUDDY_CPP__
#define __BUDDY_CPP__

#include "stdlib/buddy.hpp"

struct list_head page_buddy[MAX_BUDDY_PAGE_NUM];

struct page *virt_to_page(uint32_t addr)
{
    uint32_t index;
    index = ((addr) - KERNEL_PAGING_START) >> PAGE_SHIFT;
    if (index > KERNEL_PAGE_NUM) {
        return nullptr;
    }
    return (struct page *)KERNEL_PAGE_START + index;
}

void init_page_buddy()
{
    for (int i = 0; i < MAX_BUDDY_PAGE_NUM; ++i) {
        INIT_LIST_HEAD(&page_buddy[i]);
    }
}

void init_page_map()
{
    struct page *pg = (struct page *)KERNEL_PAGE_START;
// #ifdef DEBUG_MALLOC
//     ACENN_DEBUG("pg: %x, KERNEL_PAGE_START: %x", pg, KERNEL_PAGE_START);
// #endif
    init_page_buddy();
    for (int i = 0; i < KERNEL_PAGE_NUM; ++pg, ++i) {
        pg->vaddr   = KERNEL_PAGING_START + i * PAGE_SIZE;
        pg->flags   = PAGE_AVAILABLE;
        pg->counter = 0;
        INIT_LIST_HEAD(&(pg->list));

        if (i < (KERNEL_PAGE_NUM & (~PAGE_NUM_FOR_MAX_BUDDY))) {
            if ((i & PAGE_NUM_FOR_MAX_BUDDY) == 0) {
                pg->order = MAX_BUDDY_PAGE_NUM - 1;
            } else {
                pg->order = -1;
            }
            list_add_tail(&(pg->list), &page_buddy[MAX_BUDDY_PAGE_NUM-1]);
        } else {
            pg->order = 0;
            list_add_tail(&(pg->list), &page_buddy[0]);
        }
    }
}

struct page *get_pages_from_list(int order)
{
    uint32_t vaddr;
    struct page *pg, *ret;
    struct list_head *tlist, *tlist1;
    int new_order=order;
    for (; new_order < MAX_BUDDY_PAGE_NUM; ++new_order) {
        if (list_empty(&page_buddy[new_order])) {
            continue;
        } else {
            pg = list_entry(page_buddy[new_order].next, struct page, list);
            tlist = &(BUDDY_END(pg, new_order)->list);
            tlist->next->prev = &page_buddy[new_order];
            page_buddy[new_order].next = tlist->next;
            goto OUT_OK;
        }
    }
    return nullptr;

OUT_OK:
    for (new_order--; new_order >= order; --new_order) {
        tlist1 = &(BUDDY_END(pg, new_order)->list);
        tlist = &(pg->list);

        pg = NEXT_BUDDY_START(pg, new_order);
        list_entry(tlist, struct page, list)->order = new_order;

        list_add_chain_tail(tlist, tlist1, &page_buddy[new_order]);
    }
    pg->flags |= PAGE_BUDDY_BUSY;
    pg->order = order;
    return pg;
}

void put_pages_to_list(struct page *pg, int order)
{
    struct page *tprev, *tnext;
    if (!(pg->flags & PAGE_BUDDY_BUSY)) {
        printf_("Page allocation failed!\n");
        return;
    }
    pg->flags &= ~(PAGE_BUDDY_BUSY);

    for (;order < MAX_BUDDY_PAGE_NUM; ++order) {
        tnext = NEXT_BUDDY_START(pg, order);
        tprev = PREV_BUDDY_START(pg, order);
// #ifdef DEBUG_MALLOC
//         ACENN_DEBUG("pg: %x, order: %d, tnext: %x, tprev: %x", pg, order, tnext, tprev);
// #endif
        if (((!tnext->flags & PAGE_BUDDY_BUSY)) && (tnext->order == order)) {
            pg->order++;
            tnext->order = -1;
            list_remove_chain(&(tnext->list), &(BUDDY_END(tnext, order)->list));
            BUDDY_END(pg, order)->list.next = &(tnext->list);
            tnext->list.prev = &(BUDDY_END(pg, order)->list);
            continue;
        } else if ((!(tprev->flags & PAGE_BUDDY_BUSY)) && (tprev->order == order) && ((uint32_t)tprev != KERNEL_PAGE_START)) {
            pg->order = -1;
            list_remove_chain(&(pg->list), &(BUDDY_END(pg, order)->list));
            BUDDY_END(tprev, order)->list.next = &(pg->list);
            pg->list.prev = &(BUDDY_END(tprev, order)->list);

            pg = tprev;
            pg->order++;
            continue;
        } else {
            break;
        }
    }
    list_add_chain(&(pg->list), &((tnext - 1)->list), &page_buddy[order]);
}

void *page_address(struct page *pg)
{
    return (void *)(pg->vaddr);
}

struct page *alloc_pages(uint32_t flag, int order)
{
    struct page *pg;
    pg = get_pages_from_list(order);
    if (pg == nullptr) {
        return nullptr;
    }
    for (int i = 0; i < (1 << order); ++i) {
        (pg + i)->flags |= PAGE_DIRTY;
    }
    return pg;
}

void free_pages(struct page *pg, int order)
{
// #ifdef DEBUG_MALLOC
//     ACENN_DEBUG("pg: %x, order: %d", pg, order);
// #endif
    for (int i = 0; i < (1 << order); ++i) {
        (pg + i)->flags &= ~PAGE_DIRTY;
    }
    put_pages_to_list(pg, order);
}

void *get_free_pages(uint32_t flag, int order)
{
    struct page *pg;
    pg = alloc_pages(flag, order);
    if (pg == nullptr) {
        return nullptr;
    }
    return page_address(pg);
}

void put_free_pages(void *addr, int order)
{
// #ifdef DEBUG_MALLOC
//     ACENN_DEBUG("addr: %x, order: %d", addr, order);
// #endif
    free_pages(virt_to_page((uint32_t)addr), order);
}

// __always_inline int get_buddy_order(uint32_t size) {
int get_buddy_order(uint32_t size) {
    int order = 0;
    if (size <= PAGE_SIZE) {
        return 0;
    } else if (size <= 2*PAGE_SIZE) {
        return 1;
    } else if (size <= 4*PAGE_SIZE) {
        return 2;
    } else if (size <= 8*PAGE_SIZE) {
        return 3;
    } else if (size <= 16*PAGE_SIZE) {
        return 4;
    } else if (size <= 32*PAGE_SIZE) {
        return 5;
    } else if (size <= 64*PAGE_SIZE) {
        return 6;
    } else if (size <= 128*PAGE_SIZE) {
        return 7;
    } else if (size <= 256*PAGE_SIZE) {
        return 8;
    } else {
#ifdef DEBUG_MALLOC
        ACENN_DEBUG("The size exceeds BUDDy_MAX_SIZE!");
#endif
        return -1;
    }
}

#endif
