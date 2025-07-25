#include "sau_benchmark_func8b.hpp"

void matmul_multi8(int8_t *input_blockA, int8_t *input_blockB, int8_t *output_blockD, int M, int K, int N) {
  const int us=8;
	ACENN_DEBUG("SAU matmul_multi8 START: A[%d,%d] B[%d,%d] -> D[%d,%d]", M, K, K, N, M, N);
  // 配置寄存器
  int32_t a,b,c,d,e,f;
  int32_t h_ch=0,h_x=K/us,v_ch=us,v_x=N/us,o_ch=1,o_x=N/us;
  int32_t id=0,bias_address=0,flow_loop_times=K/us;
  int32_t stride_mode=0,transpose_mode=1;
  int32_t cutbit=4, shift_mode=0;
  for (int i = 0; i < M/us; i++) {
    for (int j = 0; j < N/us; j++) {
      int32_t h_addr=(int32_t)(uintptr_t)(input_blockA + us*K*i);
      int32_t v_addr=(int32_t)(uintptr_t)(input_blockB + us*j);
      int32_t o_addr=(int32_t)(uintptr_t)(output_blockD + us*(N*i + j));
      a=(v_ch<<25)|(v_x<<20)|(v_addr & 0xFFFFF);
      b=(h_ch<<25)|(h_x<<20)|(h_addr & 0xFFFFF);
      c=(o_ch<<25)|(o_x<<20)|(o_addr & 0xFFFFF);
      d=(cutbit<<18)|(shift_mode<<17)|(stride_mode<<16)|(transpose_mode<<14)|(0<<12)|(0<<10)|(0<<8)|(0<<6)|flow_loop_times;
      e=1;
      f=(bias_address<<8)|id;
      __kuiloong_ace_msetins1(b,a);
      __kuiloong_ace_msetins2(d,c);
      __kuiloong_ace_msetins3(f,e);
    }
  }
}