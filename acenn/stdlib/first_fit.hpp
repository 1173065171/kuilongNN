
#include "stdlib/config.hpp"
#include "stdlib/printf.hpp"
#include <cstddef>
#define HEAP_START  MEM_START   // 堆起始地址
#define HEAP_SIZE   (MEM_END - MEM_START)  // 1MB 堆空间
#define NALLOC      128            // 最小分配单位
#define ALIGNMENT   16             // 128 bit 对齐
#define NULL        0

union header {
  struct {
    union header *next;  // 下一个空闲块
    unsigned len;        // 块长度
    unsigned pad;        // 填充字段，确保整体大小为16字节的倍数
  } meta;
  unsigned char align[ALIGNMENT];
};

static char * const heap = (char * const)HEAP_START;
static union header list;        // 哨兵节点
static union header *first = NULL;
static int heap_initialized = 0;  // 堆初始化标志

static inline unsigned int align_offset(void *ptr) {
  unsigned long addr = (unsigned long)ptr;
  unsigned int offset = (ALIGNMENT - (addr % ALIGNMENT)) % ALIGNMENT;
  return offset;
}

static void* ff_memset(void* ptr, int c, unsigned int size);
static void* ff_memmove(void* dest, const void* src, unsigned int size);
void init_heap();
void ff_free(void* ptr);
void* ff_malloc(size_t size);
void* calloc(size_t num, size_t len);
void* realloc(void* ptr, size_t new_size);

