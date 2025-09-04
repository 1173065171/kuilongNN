#include "veu_benchmark_8x8func.hpp"

void gather_trick(int8_t *input_block, int M, int K, int col, int8_t *output_vector)
{
  ACENN_DEBUG("VEU gather_trick START: In[%d,%d] col = %d -> Out[%d]", M, K, col, K);
  uint32_t raddr = (uint32_t)(uintptr_t)(input_block + col*K);
  uint32_t waddr = (uint32_t)(uintptr_t)output_vector;
  ACENN_DEBUG("raddr=0x%x, waddr=0x%x", raddr, waddr);
  __kuiloong_ace_vsetcsr(
                          SCALAR_EN(0) |
                          WRITE_BACK(1) |
                          CLEAR_OB(1) |
                          MODE(CFG_I8) |
                          SHIFT(0) |
                          RESET(0), VEUCFG);
  __kuiloong_ace_vsetcsr(8*K, VEUVLEN);
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr(waddr, VEUWADDR);
  __kuiloong_ace_vmv(raddr);
}

// 注意：这里使用的是 uint8_t 类型，模拟的是 exp(-x) 的递减行为
const uint8_t piecewise_exp_lut_32[256] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6,
    6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8,
    9, 9, 9,10,10,10,10,11,11,11,12,12,12,13,13,13,
    14,14,15,15,16,16,17,17,18,18,19,19,20,20,21,22,
    22,23,24,25,25,26,27,27,28,29,30,31,32,33,34,35,
    36,37,39,40,41,42,44,45,46,48,49,51,53,54,56,57,
    59,61,63,65,67,69,71,73,76,78,81,84,86,89,91,94,
    97,101,104,107,111,114,117,121,125,129,133,138,142,146,150,155,
    160,166,171,177,182,188,193,199,206,213,220,227,234,241,248,255
};

void lookup_softmax8(int8_t *input_vector, uint8_t *output_vector, int N) {
  ACENN_DEBUG("VEU lookup_softmax8 START: In[%d] N = %d -> Out[%d]", N, N, N);
  int8_t* max_val = (int8_t*)malloc(sizeof(int8_t));
  *max_val = input_vector[0];
  uint32_t raddr = (uint32_t)(uintptr_t)input_vector;
  uint32_t waddr = (uint32_t)(uintptr_t)max_val;
  if (N < 16) {
    for (int i = 1; i < N; ++i) {
      if (input_vector[i] > *max_val)
        *max_val = input_vector[i];
    }
  } else {

    while(__kuiloong_ace_vgetcsr(VEUSTATUS)){

    }
    __kuiloong_ace_vsetcsr(
                            SCALAR_EN(0) |
                            WRITE_BACK(1) |
                            CLEAR_OB(1) |
                            MODE(CFG_I8) |
                            SHIFT(0) |
                            RESET(0), VEUCFG);
    __kuiloong_ace_vsetcsr(8*N, VEUVLEN);

    __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
    __kuiloong_ace_vsetcsr(waddr, VEUWADDR);
    __kuiloong_ace_vredmax(raddr);
  }
  ACENN_DEBUG("max = %d", *max_val);
  for (int i = 0; i < N; ++i) {
    int16_t shifted = (int16_t)input_vector[i] - *max_val;
    uint8_t index = (shifted < -255) ? 0 : (uint8_t)(shifted + 255);
    output_vector[i] = piecewise_exp_lut_32[index];
  }
  free(max_val);
}
