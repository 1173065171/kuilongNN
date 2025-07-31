#include "veu_benchmark.hpp"

void gather_trick(int8_t *input_block, int M, int K, int col, int8_t *output_vector)
{
    ACENN_DEBUG("VEU gather_trick START: In[%d,%d] col = %d -> Out[%d]", M, K, col, K);
    uint32_t raddr = (uint32_t)(uintptr_t)(input_block + col*K);
    uint32_t waddr = (uint32_t)(uintptr_t)output_vector;

  __kuiloong_ace_vsetcsr( 
                          SCALAR_EN(0) | 
                          WRITE_BACK(1) | 
                          CLEAR_OB(1) | 
                          MODE(CFG_I8) | 
                          SHIFT(0) | 
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(8*K, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vmv(raddr, waddr);
}
