//////////////////////////////////////////////////////////////////////////////////
// Author:          yintao liu - berialest@shu.edu.cn                           //
//                                                                              //
// Additional contributions by:                                                 //
//                                                                              //
// File Name:       buddy.hpp                                                   //
// Project Name:    ACENN                                                       //
// Language:        cpp                                                         //
//                                                                              //
// Description:     Buddy memory management algorithms                          //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#ifndef __BUDDY_HPP__
#define __BUDDY_HPP__

#include "cstdlib"
#include "stdlib/config.hpp"
#include "stdlib/printf.hpp"

typedef unsigned int uint32_t;

/****************************************************************************************************/
/*                                      double linked list                                          */
/****************************************************************************************************/
struct list_head {
    struct list_head *next, *prev;
};

static inline void INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}

static inline void __list_add(  struct list_head *new_list,
                                struct list_head *prev,
                                struct list_head *next)
{
    //  prev <-> new_list <-> next
    next->prev = new_list;
    new_list->next = next;
    new_list->prev = prev;
    prev->next = new_list;
}

static inline void list_add(struct list_head *new_list, struct list_head *head)
{
    __list_add(new_list, head, head->next);
}

static inline void list_add_tail(struct list_head *new_list, struct list_head *head)
{
    __list_add(new_list, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
}

static inline void list_remove_chain(struct list_head *ch, struct list_head *ct)
{
    ch->prev->next = ct->next;
    ct->next->prev = ch->prev;
}

static inline void list_add_chain(struct list_head *ch, struct list_head *ct, struct list_head *head)
{
    ch->prev = head;
    ct->next = head->next;
    head->next->prev = ct;
    head->next = ch;
}

static inline void list_add_chain_tail(struct list_head *ch,struct list_head *ct,struct list_head *head){
    ch->prev=head->prev;
    head->prev->next=ch;
    head->prev=ct;
    ct->next=head;
}

static inline int list_empty(const struct list_head *head)
{
    return head->next == head;
}

#pragma once
#define offsetof(TYPE, MEMBER) ((unsigned int) & ((TYPE *)0)->MEMBER)

#define containerof(ptr, type, member) ({ \
    const typeof(((type *)0) -> member) *__mptr = (ptr); \
    (type *)((char *)__mptr - offsetof(type, member)); \
})

#define list_entry(ptr, type, member) containerof(ptr, type, member)

#define list_for_each(pos, head) for(pos = (head)->next; pos != (head); pos = pos->next)

/****************************************************************************************************/
/*                                      buddy algorithms                                            */
/****************************************************************************************************/
//  frac{4*(1-2^{9})}{1-2} = 2044 Byte
#define MEM_END    0x50020000
#define MEM_START  0x50000000

//  1 page = 4096 Byte
// #define PAGE_SHIFT  (12)
//  1 page = 1024 Byte
#define PAGE_SHIFT  (10)
#define PAGE_SIZE   (1 << PAGE_SHIFT)
#define PAGE_MASK   (~(PAGE_SIZE - 1))

#define KERNEL_MEM_END          (MEM_END)
#define KERNEL_PAGING_START     ((MEM_START + (~PAGE_MASK)) & (PAGE_MASK))
#define KERNEL_PAGING_END       (((KERNEL_MEM_END - KERNEL_PAGING_START) / (PAGE_SIZE + sizeof(struct page))) * (PAGE_SIZE) + KERNEL_PAGING_START)

#define KERNEL_PAGE_NUM         ((KERNEL_PAGING_END - KERNEL_PAGING_START) / PAGE_SIZE)
#define KERNEL_PAGE_END         MEM_END
#define KERNEL_PAGE_START       (KERNEL_PAGE_END - KERNEL_PAGE_NUM * sizeof(struct page))

#define PAGE_AVAILABLE          0x00
#define PAGE_DIRTY              0x01
#define PAGE_PROTECT            0x02
#define PAGE_BUDDY_BUSY         0x04
#define PAGE_IN_CACHE           0x08

struct page {
    uint32_t vaddr;
    uint32_t flags;
    int order;
    uint32_t counter;
    struct kmem_cache *cachep;
    struct list_head list;
};

//  max size = 1024 K Byte = 1 M Byte
//  #define MAX_BUDDY_PAGE_NUM          (8)
//  64 KB
#define MAX_BUDDY_PAGE_NUM          (6)
#define AVERAGE_PAGE_NUM_PER_BUDDY  (KERNEL_PAGE_NUM / MAX_BUDDY_PAGE_NUM)
#define PAGE_NUM_FOR_MAX_BUDDY      ((1 << MAX_BUDDY_PAGE_NUM) - 1)
#define BUDDY_MAX_SIZE              ((PAGE_SIZE) * (1 << MAX_BUDDY_PAGE_NUM))

#define BUDDY_END(x, order)         ((x) + (1 << (order)) - 1)
#define NEXT_BUDDY_START(x, order)  ((x) + (1 << (order)))
#define PREV_BUDDY_START(x, order)  ((x) - (1 << (order)))

void init_page_map();

struct page *virt_to_page(uint32_t addr);

void *page_address(struct page *pg);

struct page *alloc_pages(uint32_t flag, int order);

void free_pages(struct page *pg, int order);

void *get_free_pages(uint32_t flag, int order);

void *get_free_pages(uint32_t flag, int order);

void put_free_pages(void *addr, int order);

int get_buddy_order(uint32_t size);

#endif
