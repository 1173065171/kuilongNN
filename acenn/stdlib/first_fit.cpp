
#include "stdlib/first_fit.hpp"

static void* ff_memset(void* ptr, int c, unsigned int size) {
  char* p = (char*)ptr;
  for (unsigned int i = 0; i < size; i++) {
    p[i] = (char)c;
  }
  return ptr;
}

static void* ff_memmove(void* dest, const void* src, unsigned int size) {
  char* d = (char*)dest;
  const char* s = (const char*)src;
  
  if (d < s) {
    for (unsigned int i = 0; i < size; i++) {
      d[i] = s[i];
    }
  } else if (d > s) {
    for (unsigned int i = size; i > 0; i--) {
      d[i-1] = s[i-1];
    }
  }
  return dest;
}

void init_heap() {
  if (heap_initialized) return;
  
  // 验证堆起始地址是否16字节对齐
  if ((unsigned long)heap % ALIGNMENT != 0) {
    return;  // 起始地址未对齐，初始化失败
  }
  
  union header *initial_block = (union header*)heap;
  // 计算堆区域能容纳的header数量（减去自身头部）
  initial_block->meta.len = (HEAP_SIZE / sizeof(union header)) - 1;
  initial_block->meta.next = &list;  // 形成环形链表
  
  // 初始化哨兵节点
  list.meta.next = initial_block;
  list.meta.len = 0;
  list.meta.pad = 0;
  
  first = &list;
  heap_initialized = 1;
}

void ff_free(void* ptr) {
  if (!heap_initialized || ptr == NULL) return;
  ACENN_DEBUG("ff_free addr: %x", ptr);

  
  // 从用户指针反向计算实际块头部（考虑对齐偏移）
  unsigned char *user_ptr = (unsigned char*)ptr;
  unsigned int offset = *(unsigned int*)(user_ptr - sizeof(unsigned int));
  union header *block = (union header*)(user_ptr - offset - sizeof(union header));
  union header *iter = first;

  /* 找到按地址排序的插入位置 */
  while (block <= iter || block >= iter->meta.next) {
    if ((block > iter || block < iter->meta.next) && iter >= iter->meta.next) {
      break;
    }
    iter = iter->meta.next;
  }

  // 合并后向块
  if (block + block->meta.len == iter->meta.next) {
    block->meta.len += iter->meta.next->meta.len;
    block->meta.next = iter->meta.next->meta.next;
  } else {
    block->meta.next = iter->meta.next;
  }
  // 合并前向块
  if (iter + iter->meta.len == block) {
    iter->meta.len += block->meta.len;
    iter->meta.next = block->meta.next;
  } else {
    iter->meta.next = block;
  }

  first = iter;
}

void* ff_malloc(size_t size) {
  if (!heap_initialized || size == 0) return NULL;

  union header *p, *prev;
  prev = first;
  
  // 计算实际需要的总空间：用户数据 + 对齐偏移 + 偏移量存储
  unsigned int header_size = sizeof(union header);
  unsigned int extra = sizeof(unsigned int);  // 用于存储偏移量
  unsigned int total_size = size + header_size + extra + ALIGNMENT;
  
  // 转换为header单位
  unsigned true_size = (total_size + header_size - 1) / header_size + 1;

  p = prev->meta.next;
  while (1) {
    if (p->meta.len >= true_size) {  // 找到足够大的块
      if (p->meta.len == true_size) {
        prev->meta.next = p->meta.next;
      } else {
        p->meta.len -= true_size;
        p += p->meta.len;
        p->meta.len = true_size;
      }
      
      unsigned char *base = (unsigned char*)(p + 1);  // 跳过header
      unsigned int offset = align_offset(base + extra);  // 预留偏移量存储位置
      unsigned char *user_ptr = base + extra + offset;
      
      *(unsigned int*)base = offset;
      
      first = prev;
      ACENN_DEBUG("ff_malloc addr: %x", user_ptr);
      return user_ptr;
    }

    if (p == first) {  // 内存耗尽
      return NULL;
    }

    prev = p;
    p = p->meta.next;
  }
}

void* calloc(size_t num, size_t len) {
  size_t total = num * len;
  void* ptr = ff_malloc(total);
  if (ptr != NULL) {
    ff_memset(ptr, 0, total);
  }
  return ptr;
}

void* realloc(void* ptr, size_t new_size) {
  if (ptr == NULL) return ff_malloc(new_size);
  if (new_size == 0) {
    ff_free(ptr);
    return NULL;
  }

  unsigned char *user_ptr = (unsigned char*)ptr;
  unsigned int offset = *(unsigned int*)(user_ptr - sizeof(unsigned int));
  union header *old_block = (union header*)(user_ptr - offset - sizeof(union header));
  size_t old_size = (old_block->meta.len * sizeof(union header)) - 
                   sizeof(union header) - sizeof(unsigned int) - offset;

  void* new_ptr = ff_malloc(new_size);
  if (new_ptr == NULL) return NULL;

  size_t copy_size = (new_size < old_size) ? new_size : old_size;
  ff_memmove(new_ptr, ptr, copy_size);

  ff_free(ptr);
  return new_ptr;
}
