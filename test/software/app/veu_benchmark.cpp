#include "veu_benchmark.hpp"
#include "veu_testdata.hpp"




// int test_vadd_asm(int a, int b)
// {
//   int result;
//   __asm__ __volatile__ 
//   (
//   "vadd %1,%2,%0"
//   :"=r"(result)
//   :"r"(a), "r"(b)
//   :
//   );	
// };

// int test_vorcsr_asm(int a, int b)
// {
//   int result;
//   __asm__ __volatile__ 
//   (
//   "vorcsr %0,%1,%2"
//   :"=r"(result)
//   :"r"(a), "i"(0x10)
//   :
//   );
// };


void vorcsr_testcase1()
{
  int a, b, r;
  a = 0xA;
  b = 0xB;
  __kuiloong_ace_vsetcsr(a, VEURADDR1);
  r = __kuiloong_ace_vgetcsr(VEURADDR1);
  if (r != a) {
    ACENN_DEBUG("Read %x", r);
    ACENN_DEBUG("VEURADDR1 failed to set %x", a);
    asm("ebreak;");
  }
  __kuiloong_ace_vorcsr(b, VEURADDR1);
  r = __kuiloong_ace_vgetcsr(VEURADDR1);
  if (r != b) {
    ACENN_DEBUG("Read %x", r);
    ACENN_DEBUG("VEURADDR1 failed to set %x", b);
    asm("ebreak;");
  }
  ACENN_DEBUG("VORCSR PASS!");
}

void vandcsr_testcase1()
{
  int a, b, r;
  a = 0xA;
  b = 0xB;
  __kuiloong_ace_vsetcsr(b, VEURADDR1);
  r = __kuiloong_ace_vgetcsr(VEURADDR1);
  if (r != b) {
    ACENN_DEBUG("Read %x", r);
    ACENN_DEBUG("VEURADDR1 failed to set %x", b);
    asm("ebreak;");
  }
  __kuiloong_ace_vandcsr(a, VEURADDR1);
  r = __kuiloong_ace_vgetcsr(VEURADDR1);
  if (r != a) {
    ACENN_DEBUG("Read %x", r);
    ACENN_DEBUG("VEURADDR1 failed to set %x", a);
    asm("ebreak;");
  }
  ACENN_DEBUG("VANDCSR PASS!");
}

void vsetcsr_testcase1()
{
  int a, b, r;
  a = 0xA;
  b = 0xB;
  __kuiloong_ace_vsetcsr(a, VEURADDR1);
  r = __kuiloong_ace_vgetcsr(VEURADDR1);
  if (r != a) {
    ACENN_DEBUG("Read %x", r);
    ACENN_DEBUG("VEURADDR1 failed to set %x", a);
    asm("ebreak;");
  }
  __kuiloong_ace_vsetcsr(b, VEURADDR1);
  r = __kuiloong_ace_vgetcsr(VEURADDR1);
  if (r != b) {
    ACENN_DEBUG("Read %x", r);
    ACENN_DEBUG("VEURADDR1 failed to set %x", b);
    asm("ebreak;");
  }
  ACENN_DEBUG("VSETCSR PASS!");
}

void vgetcsr_testcase1()
{
  int a, b, r;
  a = 0xA;
  b = 0xB;
  __kuiloong_ace_vsetcsr(a, VEURADDR1);
  r = __kuiloong_ace_vgetcsr(VEURADDR1);
  if (r != a) {
    ACENN_DEBUG("Read %x", r);
    ACENN_DEBUG("VEURADDR1 failed to set %x", a);
    asm("ebreak;");
  }
  __kuiloong_ace_vsetcsr(b, VEURADDR1);
  r = __kuiloong_ace_vgetcsr(VEURADDR1);
  if (r != b) {
    ACENN_DEBUG("Read %x", r);
    ACENN_DEBUG("VEURADDR1 failed to set %x", b);
    asm("ebreak;");
  }
  ACENN_DEBUG("VGETCSR PASS!");
}

void vadd_testcase1()
{
  //  vector + vector
  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(128, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vadd((uint32_t)raddr1, (uint32_t)raddr2);
  for (int i = 0; i < 16; ++i) {
    if ((waddr[i] & 0xFF) != ((raddr1[i] + raddr2[i]) & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != (raddr1[%d] + raddr2[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, (raddr1[i] + raddr2[i]) & 0xFF);
      asm("ebreak;");
    }
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(2048, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vadd((uint32_t)raddr1, (uint32_t)raddr2);
  for (int i = 0; i < 256; ++i) {
    if ((waddr[i] & 0xFF) != ((raddr1[i] + raddr2[i]) & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != (raddr1[%d] + raddr2[%d]: %x != %x", i, i, i, waddr[i] & 0xFF, (raddr1[i] + raddr2[i]) & 0xFF);
      asm("ebreak;");
    }
  }
  ACENN_DEBUG("VADD PASS!");
}

void vadd_testcase2() {
  //  scalar + vector
  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(1) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(512, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vadd(0xFF, (uint32_t)raddr2);
  for (int i = 0; i < 64; ++i) {
    if ((waddr[i] & 0xFF) != ((0xFF + raddr2[i]) & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != (0xFF+ raddr2[%d]: %x != %x", i, i, waddr[i] & 0xFF, (0xFF + raddr2[i]) & 0xFF);
      asm("ebreak;");
    }
  }

//   for (int i = 0; i < 256; ++i) {
//     waddr[i] = 0x00;
//   }
//   __kuiloong_ace_vsetcsr( 
//                           SCALAR_EN(1) | 
//                           WRITE_BACK(1) | 
//                           CLEAR_OB(1) | 
//                           MODE(CFG_I8) | 
//                           SHIFT(0) | 
//                           RESET(0), VEUCFG);
//   __kuiloong_ace_vsetcsr(512, VEUVLEN);
//   __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
//   __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
//   __kuiloong_ace_vadd((uint32_t)raddr2, 0xFF);
//   for (int i = 0; i < 65; ++i) {
//     if ((waddr[i] & 0xFF) != ((0xFF + raddr2[i]) & 0xFF)) {
//       ACENN_DEBUG("waddr[%d] != (0xFF + raddr2[%d]: %x != %x", i, i, waddr[i] & 0xFF, (0xFF + raddr2[i]) & 0xFF);
//       asm("ebreak;");
//     }
//   }
//   if ((waddr[65] & 0xFF) != 0) {
//     ACENN_DEBUG("waddr[65] != 0: %x != 0", waddr[65] & 0xFF);
//     asm("ebreak;");
//   }
  ACENN_DEBUG("VADD PASS!");
}

void vsub_testcase1()
{
  //  vector - vector
  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr(
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(128, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vsub((uint32_t)raddr4, (uint32_t)raddr3);
  for (int i = 0; i < 16; ++i) {
    if ((waddr[i] & 0xFF) != ((raddr4[i] - raddr3[i]) & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != (raddr4[%d] - raddr3[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, (raddr4[i] - raddr3[i]) & 0xFF);
      asm("ebreak;");
    }
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(2048, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vsub((uint32_t)raddr4, (uint32_t)raddr3);
  for (int i = 0; i < 256; ++i) {
    if ((waddr[i] & 0xFF) != ((raddr4[i] - raddr3[i]) & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != (raddr4[%d] - raddr3[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, (raddr4[i] - raddr3[i]) & 0xFF);
      asm("ebreak;");
    }
  }
  ACENN_DEBUG("VSUB PASS!");
}

void vmin_testcase1()
{
  //  min(vector, vector)
  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(128, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmin((uint32_t)raddr3, (uint32_t)raddr4);
  for (int i = 0; i < 16; ++i) {
    if ((waddr[i] & 0xFF) != (raddr3[i] & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != min(raddr3[%d], raddr4[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, ((raddr3[i] & 0xFF) < (raddr4[i] & 0xFF)) ? raddr3[i] & 0xFF : raddr4[i] & 0xFF);
      asm("ebreak;");
    }
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(2048, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmin((uint32_t)raddr3, (uint32_t)raddr4);
  for (int i = 0; i < 256; ++i) {
    if ((waddr[i] & 0xFF) != (raddr3[i] & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != min(raddr3[%d], raddr4[%d]: %x != %x", i, i, i, waddr[i] & 0xFF, ((raddr3[i] & 0xFF) < (raddr4[i] & 0xFF)) ? raddr3[i] & 0xFF : raddr4[i] & 0xFF);
      asm("ebreak;");
    }
  }
  ACENN_DEBUG("VMIN PASS!");
}

void vminu_testcase1()
{
  //  minu(vector, vector)
  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_UI8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(128, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmin((uint32_t)raddr3, (uint32_t)raddr4);
  for (int i = 0; i < 16; ++i) {
    if ((waddr[i] & 0xFF) != (raddr3[i] & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != minu(raddr3[%d], raddr4[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, ((raddr3[i] & 0xFF) < (raddr4[i] & 0xFF)) ? raddr3[i] & 0xFF : raddr4[i] & 0xFF);
      asm("ebreak;");
    }
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_UI8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(2048, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmin((uint32_t)raddr3, (uint32_t)raddr4);
  for (int i = 0; i < 256; ++i) {
    if ((waddr[i] & 0xFF) != (raddr3[i] & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != minu(raddr3[%d], raddr4[%d]: %x != %x", i, i, i, waddr[i] & 0xFF, ((raddr3[i] & 0xFF) < (raddr4[i] & 0xFF)) ? raddr3[i] & 0xFF : raddr4[i] & 0xFF);
      asm("ebreak;");
    }
  }
  ACENN_DEBUG("VMINU PASS!");
}

void vmax_testcase1()
{
  //  max(vector, vector)
  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(128, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmax((uint32_t)raddr3, (uint32_t)raddr4);
  for (int i = 0; i < 16; ++i) {
    if ((waddr[i] & 0xFF) != (raddr4[i] & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != max(raddr3[%d], raddr4[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, ((raddr3[i] & 0xFF) > (raddr4[i] & 0xFF)) ? raddr3[i] & 0xFF : raddr4[i] & 0xFF);
      asm("ebreak;");
    }
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(2048, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmax((uint32_t)raddr3, (uint32_t)raddr4);
  for (int i = 0; i < 256; ++i) {
    if ((waddr[i] & 0xFF) != (raddr4[i] & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != max(raddr3[%d], raddr4[%d]: %x != %x", i, i, i, waddr[i] & 0xFF, ((raddr3[i] & 0xFF) > (raddr4[i] & 0xFF)) ? raddr3[i] & 0xFF : raddr4[i] & 0xFF);
      asm("ebreak;");
    }
  }
  ACENN_DEBUG("VMAX PASS!");
}

void vmaxu_testcase1()
{
  //  maxu(vector, vector)
  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_UI8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(128, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmax((uint32_t)raddr3, (uint32_t)raddr4);
  for (int i = 0; i < 16; ++i) {
    if ((waddr[i] & 0xFF) != (raddr4[i] & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != maxu(raddr3[%d], raddr4[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, ((raddr3[i] & 0xFF) > (raddr4[i] & 0xFF)) ? raddr3[i] & 0xFF : raddr4[i] & 0xFF);
      asm("ebreak;");
    }
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_UI8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(2048, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmax((uint32_t)raddr3, (uint32_t)raddr4);
  for (int i = 0; i < 256; ++i) {
    if ((waddr[i] & 0xFF) != (raddr4[i] & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != maxu(raddr3[%d], raddr4[%d]: %x != %x", i, i, i, waddr[i] & 0xFF, ((raddr3[i] & 0xFF) > (raddr4[i] & 0xFF)) ? raddr3[i] & 0xFF : raddr4[i] & 0xFF);
      asm("ebreak;");
    }
  }
  ACENN_DEBUG("VMAXU PASS!");
}

void vand_testcase1()
{
  //  vector & vector
  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(128, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vand((uint32_t)raddr3, (uint32_t)raddr4);
  for (int i = 0; i < 16; ++i) {
    if ((waddr[i] & 0xFF) != ((raddr3[i] & raddr4[i]) & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != (raddr3[%d] & raddr4[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, (raddr3[i] & raddr4[i]) & 0xFF);
      asm("ebreak;");
    }
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(2048, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vand((uint32_t)raddr3, (uint32_t)raddr4);
  for (int i = 0; i < 256; ++i) {
    if ((waddr[i] & 0xFF) != ((raddr3[i] & raddr4[i]) & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != (raddr3[%d] & raddr4[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, (raddr3[i] & raddr4[i]) & 0xFF);
      asm("ebreak;");
    }
  }
  ACENN_DEBUG("VAND PASS!");
}

void vor_testcase1()
{
  //  vector | vector
  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(128, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vor((uint32_t)raddr3, (uint32_t)raddr4);
  for (int i = 0; i < 16; ++i) {
    if ((waddr[i] & 0xFF) != ((raddr3[i] | raddr4[i]) & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != (raddr3[%d] | raddr4[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, (raddr3[i] | raddr4[i]) & 0xFF);
      asm("ebreak;");
    }
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(2048, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vor((uint32_t)raddr3, (uint32_t)raddr4);
  for (int i = 0; i < 256; ++i) {
    if ((waddr[i] & 0xFF) != ((raddr3[i] | raddr4[i]) & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != (raddr3[%d] | raddr4[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, (raddr3[i] | raddr4[i]) & 0xFF);
      asm("ebreak;");
    }
  }
  ACENN_DEBUG("VOR PASS!");
}

void vxor_testcase1()
{
  //  vector ^ vector
  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(128, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vxor((uint32_t)raddr3, (uint32_t)raddr4);
  for (int i = 0; i < 16; ++i) {
    if ((waddr[i] & 0xFF) != ((raddr3[i] ^ raddr4[i]) & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != (raddr3[%d] ^ raddr4[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, (raddr3[i] ^ raddr4[i]) & 0xFF);
      asm("ebreak;");
    }
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(2048, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vxor((uint32_t)raddr3, (uint32_t)raddr4);
  for (int i = 0; i < 256; ++i) {
    if ((waddr[i] & 0xFF) != ((raddr3[i] ^ raddr4[i]) & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != (raddr3[%d] ^ raddr4[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, (raddr3[i] ^ raddr4[i]) & 0xFF);
      asm("ebreak;");
    }
  }
  ACENN_DEBUG("VXOR PASS!");
}

void vmv_testcase1()
{
  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(128, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vmv((uint32_t)raddr4, (uint32_t)waddr);
  for (int i = 0; i < 16; ++i) {
    if ((waddr[i] & 0xFF) != (raddr4[i] & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != raddr4[%d]: %x != %x", i, i, waddr[i] & 0xFF, raddr4[i] & 0xFF);
      asm("ebreak;");
    }
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(2048, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vmv((uint32_t)raddr4, (uint32_t)waddr);
  for (int i = 0; i < 256; ++i) {
    if ((waddr[i] & 0xFF) != (raddr4[i] & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != raddr4[%d]: %x != %x", i, i, waddr[i] & 0xFF, raddr4[i] & 0xFF);
      asm("ebreak;");
    }
  }
  ACENN_DEBUG("VMV PASS!");
}

void vssrl_testcase1() {
  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(128, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vssrl(1, (uint32_t)raddr1);
  for (int i = 0; i < 16; ++i) {
    if ((waddr[i] & 0xFF) != ((raddr1[i] & 0xFF) >> 1)) {
      ACENN_DEBUG("waddr[%d] != (raddr1[%d] >> 1): %x != %x", i, i, waddr[i] & 0xFF, (raddr1[i] & 0xFF) >> 1);
      asm("ebreak;");
    }
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(2048, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vssrl(1, (uint32_t)raddr1);
  for (int i = 0; i < 256; ++i) {
    if ((waddr[i] & 0xFF) != ((raddr1[i] & 0xFF) >> 1)) {
      ACENN_DEBUG("waddr[%d] != (raddr1[%d] >> 1): %x != %x", i, i, waddr[i] & 0xFF, (raddr1[i] & 0xFF) >> 1);
      asm("ebreak;");
    }
  }
  ACENN_DEBUG("VSSRL PASS!");
}

void vssra_testcase1() {
  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(128, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vssra(1, (uint32_t)raddr1);
  for (int i = 0; i < 16; ++i) {
    if ((waddr[i] & 0xFF) != ((raddr1[i] >> 1) & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != (raddr1[%d] >> 1): %x != %x", i, i, waddr[i] & 0xFF, (raddr1[i] >> 1) & 0xFF);
      asm("ebreak;");
    }
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(2048, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vssra(1, (uint32_t)raddr1);
  for (int i = 0; i < 16; ++i) {
    if ((waddr[i] & 0xFF) != ((raddr1[i] >> 1) & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != (raddr1[%d] >> 1): %x != %x", i, i, waddr[i] & 0xFF, (raddr1[i] >> 1) & 0xFF);
      asm("ebreak;");
    }
  }
  ACENN_DEBUG("VSSRA PASS!");
}

void vnclip_testcase1()
{
	for (int i = 0; i < 256; ++i) {
		waddr[i] = 0xFF;
	}
	int16_t maxval = 64;
	int16_t minval = -64;
	uint32_t clip_para = ((maxval & 0xFF) << 16) | ((minval & 0xFF) & 0xFFFF);
	__kuiloong_ace_vsetcsr( 
							SCALAR_EN(1) | 
							WRITE_BACK(1) | 
							CLEAR_OB(1) | 
							MODE(CFG_UI8) | 
							SHIFT(0) | 
							RESET(0), VEUCFG);
	__kuiloong_ace_vsetcsr(128, VEUVLEN);
	__kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
	__kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
	__kuiloong_ace_vnclip(clip_para, (uint32_t)waddr);
	for (int i = 0; i < 16; ++i) {
		if ((waddr[i] & 0xFF) != 64) {
		ACENN_DEBUG("waddr[%d] != %x: %x != %d", i, 128, waddr[i] & 0xFF, 64);
		asm("ebreak;");
		}
	}

	for (int i = 0; i < 256; ++i) {
		waddr[i] = 0xFF;
	}
	__kuiloong_ace_vsetcsr( 
							SCALAR_EN(1) | 
							WRITE_BACK(1) | 
							CLEAR_OB(1) | 
							MODE(CFG_I8) | 
							SHIFT(0) | 
							RESET(0), VEUCFG);
	__kuiloong_ace_vsetcsr(128, VEUVLEN);
	__kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
	__kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
	__kuiloong_ace_vnclip(clip_para, (uint32_t)waddr);
	for (int i = 0; i < 16; ++i) {
		if ((waddr[i] & 0xFF) != ((-1) & 0xFF)) {
		ACENN_DEBUG("waddr[%d] != %x: %x != %d", i, 128, waddr[i] & 0xFF, -1);
		asm("ebreak;");
		}
	}
	ACENN_DEBUG("VNCLIP PASS!");
}

void vwredsum_testcase1()
{
	// 硬件中可能会避开这个实现方法，最好使用vredusum()
  //  vector * vector + output_buffer
  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr(RESET(1), VEUCFG);
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(0) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(128, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vwredsum((uint32_t)raddr4, (uint32_t)waddr);
  //  16*0x2 = 0x20
  if ((waddr[0] & 0xFF) != 0x20) {
    ACENN_DEBUG("waddr[%d] != redsum_low(raddr4, raddr3): %x != %x", 0, waddr[0] & 0xFF, 0x20);
    asm("ebreak;");
  }
  if ((waddr[1] & 0xFF) != 0x0) {
    ACENN_DEBUG("waddr[%d] != redsum_high(raddr4, raddr3): %x != %x", 1, waddr[1] & 0xFF, 0x0);
    asm("ebreak;");
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr(RESET(1), VEUCFG);
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(0) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(2048, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vwredsum((uint32_t)raddr4, (uint32_t)waddr);
  //  256*0x2 = 0x200
  if ((waddr[0] & 0xFF) != 0x0) {
    ACENN_DEBUG("waddr[%d] != redsum_low(raddr4, waddr): %x != %x", 0, waddr[0] & 0xFF, 0x0);
    asm("ebreak;");
  }
  if ((waddr[1] & 0xFF) != 0x2) {
    ACENN_DEBUG("waddr[%d] != redsum_high(raddr4, waddr): %x != %x", 1, waddr[1] & 0xFF, 0x2);
    asm("ebreak;");
  }
  ACENN_DEBUG("VWREDSUM PASS!");
}

void vredsum_testcase1()
{
	// 此方法的输出最好是16bit或32bit
	//  vector * vector + output_buffer
	for (int i = 0; i < 256; ++i) {
		waddr[i] = 0x00;
	}
	__kuiloong_ace_vsetcsr(RESET(1), VEUCFG);
	__kuiloong_ace_vsetcsr( 
							SCALAR_EN(0) | 
							WRITE_BACK(1) | 
							CLEAR_OB(0) | 
							MODE(CFG_I8) | 
							SHIFT(0) | 
							RESET(0), VEUCFG);
	__kuiloong_ace_vsetcsr(128, VEUVLEN);
	__kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
	__kuiloong_ace_vredsum((uint32_t)raddr4, (uint32_t)waddr);
	//  16*0x2 = 0x20
	if ((waddr[0] & 0xFF) != 0x20) {
		ACENN_DEBUG("waddr[%d] != redsum_low(raddr4, raddr3): %x != %x", 0, waddr[0] & 0xFF, 0x20);
		asm("ebreak;");
	}
	if ((waddr[1] & 0xFF) != 0x0) {
		ACENN_DEBUG("waddr[%d] != redsum_high(raddr4, raddr3): %x != %x", 1, waddr[1] & 0xFF, 0x0);
		asm("ebreak;");
	}
	if ((waddr[2] & 0xFF) != 0x0) {
		ACENN_DEBUG("waddr[%d] != redsum_low(raddr4, raddr3): %x != %x", 0, waddr[0] & 0xFF, 0x0);
		asm("ebreak;");
	}
	if ((waddr[3] & 0xFF) != 0x0) {
		ACENN_DEBUG("waddr[%d] != redsum_high(raddr4, raddr3): %x != %x", 1, waddr[1] & 0xFF, 0x0);
		asm("ebreak;");
	}

	for (int i = 0; i < 256; ++i) {
		waddr[i] = 0x00;
	}
	__kuiloong_ace_vsetcsr(RESET(1), VEUCFG);
	__kuiloong_ace_vsetcsr( 
							SCALAR_EN(0) | 
							WRITE_BACK(1) | 
							CLEAR_OB(0) | 
							MODE(CFG_I8) | 
							SHIFT(0) | 
							RESET(0), VEUCFG);
	__kuiloong_ace_vsetcsr(2048, VEUVLEN);
	__kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
	__kuiloong_ace_vredsum((uint32_t)raddr4, (uint32_t)waddr);
	//  256*0x2 = 0x200
	if ((waddr[0] & 0xFF) != 0x0) {
		ACENN_DEBUG("waddr[%d] != redsum_0(raddr4, waddr): %x != %x", 0, waddr[0] & 0xFF, 0x0);
		asm("ebreak;");
	}
	if ((waddr[1] & 0xFF) != 0x2) {
		ACENN_DEBUG("waddr[%d] != redsum_1(raddr4, waddr): %x != %x", 1, waddr[1] & 0xFF, 0x2);
		asm("ebreak;");
	}
	if ((waddr[2] & 0xFF) != 0x0) {
		ACENN_DEBUG("waddr[%d] != redsum_2(raddr4, waddr): %x != %x", 2, waddr[2] & 0xFF, 0x0);
		asm("ebreak;");
	}
	if ((waddr[3] & 0xFF) != 0x0) {
		ACENN_DEBUG("waddr[%d] != redsum_3(raddr4, waddr): %x != %x", 3, waddr[3] & 0xFF, 0x0);
		asm("ebreak;");
	}
	ACENN_DEBUG("VREDSUM PASS!");
}

void vmac_testcase1()
{
  //  vector * vector + output_buffer
  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr(RESET(1), VEUCFG);
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(0) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(128, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmac((uint32_t)raddr4, (uint32_t)raddr3);
  for (int i = 0; i < 16; ++i) {
    if ((waddr[i] & 0xFF) != ((raddr4[i] * raddr3[i]) & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != (raddr4[%d] * raddr3[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, (raddr4[i] * raddr3[i]) & 0xFF);
      asm("ebreak;");
    }
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr(RESET(1), VEUCFG);
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(0) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(2048, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmac((uint32_t)raddr4, (uint32_t)raddr3);
  for (int i = 0; i < 16; ++i) {
    if ((waddr[i] & 0xFF) != 16*((raddr4[i] * raddr3[i]) & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != 16*(raddr4[%d] * raddr3[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, 16*(raddr4[i] * raddr3[i]) & 0xFF);
      asm("ebreak;");
    }
  }
  ACENN_DEBUG("VMAC PASS!");
}

void vmadd_testcase1() {
  //  vector + vector
  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(128, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmadd((uint32_t)raddr4, (uint32_t)raddr4);
  for (int i = 0; i < 16; ++i) {
    if ((waddr[i] & 0xFF) != ((raddr4[i] * raddr4[i]) & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != (raddr4[%d] * raddr4[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, (raddr4[i] * raddr4[i]) & 0xFF);
      asm("ebreak;");
    }
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(1) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(128, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmadd((uint32_t)raddr4, (uint32_t)raddr4);
  for (int i = 0; i < 16; ++i) {
    if ((waddr[i] & 0xFF) != (((raddr4[i] * raddr4[i]) & 0xFF) >> 1)) {
      ACENN_DEBUG("waddr[%d] != ((raddr4[%d] * raddr4[%d]) >> 1): %x != %x", i, i, i, waddr[i] & 0xFF, ((raddr4[i] * raddr4[i]) & 0xFF) >> 1);
      asm("ebreak;");
    }
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(2048, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmadd((uint32_t)raddr4, (uint32_t)raddr4);
  for (int i = 0; i < 256; ++i) {
    if ((waddr[i] & 0xFF) != ((raddr4[i] * raddr4[i]) & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != (raddr4[%d] * raddr4[%d]: %x != %x", i, i, i, waddr[i] & 0xFF, (raddr4[i] * raddr4[i]) & 0xFF);
      asm("ebreak;");
    }
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(1) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(2048, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmadd((uint32_t)raddr4, (uint32_t)raddr4);
  for (int i = 0; i < 256; ++i) {
    if ((waddr[i] & 0xFF) != (((raddr4[i] * raddr4[i]) & 0xFF) >> 1)) {
      ACENN_DEBUG("waddr[%d] != ((raddr4[%d] * raddr4[%d]) >> 1): %x != %x", i, i, i, waddr[i] & 0xFF, ((raddr4[i] * raddr4[i]) >> 1) & 0xFF);
      asm("ebreak;");
    }
  }
  ACENN_DEBUG("VMADD PASS!");
}

void vmadd_testcase2() {
  //  scalar + vector
  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(1) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(512, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmadd((uint32_t)raddr2, 0x0F);
  for (int i = 0; i < 64; ++i) {
    if ((waddr[i] & 0xFF) != ((0x0F * raddr2[i]) & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != (0x0F * raddr2[%d]: %x != %x", i, i, waddr[i] & 0xFF, (0x0F * raddr2[i]) & 0xFF);
      asm("ebreak;");
    }
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(1) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(520, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmadd((uint32_t)raddr2, 0x0F);
  for (int i = 0; i < 65; ++i) {
    if ((waddr[i] & 0xFF) != ((0x0F * raddr2[i]) & 0xFF)) {
      ACENN_DEBUG("waddr[%d] != (0xFF * raddr2[%d]: %x != %x", i, i, waddr[i] & 0xFF, (0x0F * raddr2[i]) & 0xFF);
      asm("ebreak;");
    }
  }
  if ((waddr[65] & 0xFF) != 0) {
    ACENN_DEBUG("waddr[65] != 0: %x != 0", waddr[65] & 0xFF);
    asm("ebreak;");
  }
  ACENN_DEBUG("VMADD PASS!");
}

void vmul_testcase1()
{
  //  vector * vector
  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(128, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmul((uint32_t)raddr1, (uint32_t)raddr4);
  for (int i = 0; i < 16; ++i) {
    if ((waddr[i] & 0xFF) != 0xFE) {
      ACENN_DEBUG("waddr[%d] != low(raddr1[%d] * raddr4[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, 0xFE);
      asm("ebreak;");
    }
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(2048, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmul((uint32_t)raddr1, (uint32_t)raddr4);
  for (int i = 0; i < 256; ++i) {
    if ((waddr[i] & 0xFF) != 0xFE) {
      ACENN_DEBUG("waddr[%d] != low(raddr1[%d] * raddr4[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, 0xFE);
      asm("ebreak;");
    }
  }
  ACENN_DEBUG("VMUL PASS!");
}

void vmulh_testcase1()
{
  //  vector * vector
  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(128, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmulh((uint32_t)raddr1, (uint32_t)raddr4);
  for (int i = 0; i < 16; ++i) {
    if ((waddr[i] & 0xFF) != 0x1) {
      ACENN_DEBUG("waddr[%d] != high(raddr1[%d] * raddr4[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, 0x1);
      asm("ebreak;");
    }
  }

  for (int i = 0; i < 256; ++i) {
    waddr[i] = 0x00;
  }
  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(2048, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)waddr, VEUWADDR);
  __kuiloong_ace_vmulh((uint32_t)raddr1, (uint32_t)raddr4);
  for (int i = 0; i < 256; ++i) {
    if ((waddr[i] & 0xFF) != 0x1) {
      ACENN_DEBUG("waddr[%d] != high(raddr1[%d] * raddr4[%d]): %x != %x", i, i, i, waddr[i] & 0xFF, 0x1);
      asm("ebreak;");
    }
  }
  ACENN_DEBUG("VMULH PASS!");
}

void veu_benchmark()
{
	ACENN_DEBUG("VEU BENCHMARK START!");

	ACENN_DEBUG("raddr1: %x", (uint32_t)raddr1);
	ACENN_DEBUG("raddr2: %x", (uint32_t)raddr2);
	ACENN_DEBUG("raddr3: %x", (uint32_t)raddr3);
	ACENN_DEBUG("raddr4: %x", (uint32_t)raddr4);
	ACENN_DEBUG("waddr: %x", (uint32_t)waddr);

	// waddr[0] = 0xA;
	// asm("ebreak;");
	// waddr[0] = 0xC;
	// asm("ebreak;");

	vorcsr_testcase1();
	vandcsr_testcase1();
	vsetcsr_testcase1();
	vgetcsr_testcase1();
	vadd_testcase1();
	vadd_testcase2();
	vsub_testcase1();
	vmin_testcase1();
	vminu_testcase1();
	vmax_testcase1();
	vmaxu_testcase1();
	vand_testcase1();
	vor_testcase1();
	vxor_testcase1();
	vredsum_testcase1();
	vmac_testcase1();
	vmul_testcase1();
	vssrl_testcase1();
	vssra_testcase1();
	vnclip_testcase1();
	vmv_testcase1();
	
	// vmulh_testcase1();
	// vmadd_testcase1();
	// vmadd_testcase2();
	// vwredsum_testcase1();

	// __kuiloong_ace_msetins1(32, 32);
	// int ins1lsb = __kuiloong_ace_mgetins1lsb();
	// int ins1msb = __kuiloong_ace_mgetins1msb();

	// int a, b, c;
	// a = 0xA;
	// b = 0xB;
	// c = 0xC;

	// int c = test_vadd_asm(0x1, 0x2);
	// int d = __builtin_riscv_ace_vadd(0x1, 0x2);
	// int e = test_vorcsr_asm(0x1, 0x2);
	// int f = __builtin_riscv_ace_vorcsr(0x1, 0x2);

	// int r;
	// r = __kuiloong_ace_vorcsr(a, VEURADDR1) ;
	// Printf("vorcsr: %x\n", r);
	// r = __kuiloong_ace_vorcsr(b, VEURADDR1) ;
	// Printf("vorcsr: %x\n", r);
	// r = __kuiloong_ace_vorcsr(c, VEURADDR1) ;
	// Printf("vorcsr: %x\n", r);
	// r = __kuiloong_ace_vorcsr(0, VEURADDR1) ;
	// Printf("vorcsr: %x\n", r);

	// r = __kuiloong_ace_vorcsr(a, VEURADDR1)    
	// r = __kuiloong_ace_vandcsr(a, VEURADDR1)   
	// r = __kuiloong_ace_vsetcsr(a, VEURADDR1)   
	// r = __kuiloong_ace_vgetcsr(VEURADDR1)        
	// r = __kuiloong_ace_vadd(a, b)      
	// r = __kuiloong_ace_vsub(a, b)      
	// r = __kuiloong_ace_vmin(a, b)      
	// r = __kuiloong_ace_vmax(a, b)      
	// r = __kuiloong_ace_vredmin(a, b)   
	// r = __kuiloong_ace_vredmax(a, b)   
	// r = __kuiloong_ace_vand(a, b)      
	// r = __kuiloong_ace_vor(a, b)       
	// r = __kuiloong_ace_vxor(a, b)      
	// r = __kuiloong_ace_vslideup(a, b)  
	// r = __kuiloong_ace_vslidedown(a, b)
	// r = __kuiloong_ace_vmv(a, b)       
	// r = __kuiloong_ace_vssrl(a, b)     
	// r = __kuiloong_ace_vssra(a, b)     
	// r = __kuiloong_ace_vnclip(a, b)    
	// r = __kuiloong_ace_vwredsum(a, b)  
	// r = __kuiloong_ace_vredsum(a, b)   
	// r = __kuiloong_ace_vcompress(a, b) 
	// r = __kuiloong_ace_vmac(a, b)      
	// r = __kuiloong_ace_vmadd(a, b)     
	// r = __kuiloong_ace_vmul(a, b)      
	// r = __kuiloong_ace_vmulhsu(a, b)   
	// r = __kuiloong_ace_vmulh(a, b)     

	ACENN_DEBUG("VEU BENCHMARK PASS!");
}