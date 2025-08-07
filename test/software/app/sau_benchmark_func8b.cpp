#include "sau_benchmark_func8b.hpp"

void _split_bigmat_400K(int8_t *input_blockA, int8_t *input_blockB,
                        int M, int K, int N, int8_t *newA[4], int8_t *newB[4])
{
  if (K != 400) {
    ACENN_DEBUG("_split_bigmat_400K, K != 400");
  }
  int minblock = 128;
  // For A: M x K →  split into [M x 128, M x 128, M x 128, M x 16]
  for (int m = 0; m < M; ++m) {
    for (int k = 0; k < K; ++k) {
      int8_t val = input_blockA[m * K + k];
      if (k < 128)
        newA[0][m * 128 + k] = val;
      else if (k < 256)
        newA[1][m * 128 + (k - 128)] = val;
      else if (k < 384)
        newA[2][m * 128 + (k - 256)] = val;
      else
        newA[3][m * 16 + (k - 384)] = val;
    }
  }
  // For B: K x N → split into [128 x N, 128 x N, 128 x N, 16 x N]
  for (int n = 0; n < N; ++n) {
    for (int k = 0; k < K; ++k) {
      int8_t val = input_blockB[k * N + n];
      if (k < 128)
        newB[0][k * N + n] = val;
      else if (k < 256)
        newB[1][(k - 128) * N + n] = val;
      else if (k < 384)
        newB[2][(k - 256) * N + n] = val;
      else
        newB[3][(k - 384) * N + n] = val;
    }
  }
}

void matmul_multi8(int8_t *input_blockA, int8_t *input_blockB, int8_t *output_blockD, int M, int K, int N) {
  const int us=8;
	ACENN_DEBUG("SAU matmul_multi8 START: A[%d,%d] B[%d,%d] -> D[%d,%d]", M, K, K, N, M, N);
  // 配置寄存器
  uint32_t ins1,ins2,ins3,ins4,ins5,ins6,ins7,ins8;
  uint32_t a_addr,b_addr,d_addr;
  uint32_t a_ch,b_ch,d_ch,a_x,b_x,d_x;
  // if ((K == 400) && (N < 128)) {
  //   int minblock = 128;
  //   int chunk = K/minblock;
  //   int rest = K - chunk*minblock;
	//   ACENN_DEBUG("K==%d, split!, chunk=%d, rest=%d", K, chunk, rest);
	//   ACENN_DEBUG("A: %d, %d, %d, %d", input_blockA[0], input_blockA[1], input_blockA[2], input_blockA[3]);
	//   ACENN_DEBUG("B: %d, %d, %d, %d", input_blockB[0], input_blockB[1], input_blockB[2], input_blockB[3]);
  //   int8_t *newA[4], *newB[4];
  //   for (int i = 0; i < chunk; i++) {
  //     newA[i] = (int8_t*)malloc(M*minblock*sizeof(int8_t));
  //     newB[i] = (int8_t*)malloc(minblock*N*sizeof(int8_t));
  //   }
  //   newA[3] = (int8_t*)malloc(M*rest*sizeof(int8_t));
  //   newB[3] = (int8_t*)malloc(rest*N*sizeof(int8_t));
  //   _split_bigmat_400K(input_blockA, input_blockB, M, K, N, newA, newB);
  //   // ACENN_DEBUG("============ input test ============");
  //   // for(int i = 0;i < M*minblock; ++i){if(newA[0][i] != A_split_0_sa[i]){ACENN_DEBUG("Mismatch[%d]: A_split_0_sa=%d sa=%d", i, A_split_0_sa[i], newA[0][i]);}}
  //   // for(int i = 0;i < M*minblock; ++i){if(newA[1][i] != A_split_1_sa[i]){ACENN_DEBUG("Mismatch[%d]: A_split_1_sa=%d sa=%d", i, A_split_1_sa[i], newA[1][i]);}}
  //   // for(int i = 0;i < M*minblock; ++i){if(newA[2][i] != A_split_2_sa[i]){ACENN_DEBUG("Mismatch[%d]: A_split_2_sa=%d sa=%d", i, A_split_2_sa[i], newA[2][i]);}}
  //   // for(int i = 0;i < M*rest; ++i)    {if(newA[3][i] != A_split_3_sa[i]){ACENN_DEBUG("Mismatch[%d]: A_split_3_sa=%d sa=%d", i, A_split_3_sa[i], newA[3][i]);}}
  //   // for(int i = 0;i < minblock*N; ++i){if(newB[0][i] != B_split_0_sa[i]){ACENN_DEBUG("Mismatch[%d]: B_split_0_sa=%d sa=%d", i, B_split_0_sa[i], newB[0][i]);}}
  //   // for(int i = 0;i < minblock*N; ++i){if(newB[1][i] != B_split_1_sa[i]){ACENN_DEBUG("Mismatch[%d]: B_split_1_sa=%d sa=%d", i, B_split_1_sa[i], newB[1][i]);}}
  //   // for(int i = 0;i < minblock*N; ++i){if(newB[2][i] != B_split_2_sa[i]){ACENN_DEBUG("Mismatch[%d]: B_split_2_sa=%d sa=%d", i, B_split_2_sa[i], newB[2][i]);}}
  //   // for(int i = 0;i < rest*N; ++i)    {if(newB[3][i] != B_split_3_sa[i]){ACENN_DEBUG("Mismatch[%d]: B_split_3_sa=%d sa=%d", i, B_split_3_sa[i], newB[3][i]);}}
  //   // ACENN_DEBUG("============ input test ============");
  //   // ACENN_DEBUG("============ output 0 test ============");
  //   // matmul_multi8(newA[0], newB[0], output_blockD, M, minblock, N);
  //   // for(int i = 0;i<M*N; ++i){if(output_blockD[i] != D_split_0_sa[i]){ACENN_DEBUG("D_split_0_sa: Mismatch[%d]: ref=%d sa=%d", i, D_split_0_sa[i], output_blockD[i]);}}
  //   // ACENN_DEBUG("============ output 1 test ============");
  //   // matmul_multi8(newA[1], newB[1], output_blockD, M, minblock, N);
  //   // for(int i = 0;i<M*N; ++i){if(output_blockD[i] != D_split_1_sa[i]){ACENN_DEBUG("D_split_1_sa: Mismatch[%d]: ref=%d sa=%d", i, D_split_1_sa[i], output_blockD[i]);}}
  //   // ACENN_DEBUG("============ output 2 test ============");
  //   // matmul_multi8(newA[2], newB[2], output_blockD, M, minblock, N);
  //   // for(int i = 0;i<M*N; ++i){if(output_blockD[i] != D_split_2_sa[i]){ACENN_DEBUG("D_split_2_sa: Mismatch[%d]: ref=%d sa=%d", i, D_split_2_sa[i], output_blockD[i]);}}
  //   // ACENN_DEBUG("============ output 3 test ============");
  //   // matmul_multi8(newA[3], newB[3], output_blockD, M, rest, N);
  //   // for(int i = 0;i<M*N; ++i){if(output_blockD[i] != D_split_3_sa[i]){ACENN_DEBUG("D_split_3_sa: Mismatch[%d]: ref=%d sa=%d", i, D_split_3_sa[i], output_blockD[i]);}}
  //   // ACENN_DEBUG("============ output test ============");
  //   for (int i = 0; i < M/us; i++) {
  //     for (int j = 0; j < N/us; j++) {
  //       for (int k = 0; k < chunk; k++) {
  //         int newK = minblock;
  //         int32_t h_ch=0,h_x=newK/us,v_ch=us,v_x=N/us,o_ch=1,o_x=N/us;
  //         int32_t id=0,bias_address=0,flow_loop_times=newK/us;
  //         int32_t stride_mode=0,transpose_mode=1;
  //         int32_t cutbit=4, shift_mode=0;
  //         int32_t h_addr=(int32_t)(uintptr_t)(newA[k] + us*newK*i);
  //         int32_t v_addr=(int32_t)(uintptr_t)(newB[k] + us*j);
  //         int32_t o_addr=(int32_t)(uintptr_t)(output_blockD + us*(N*i+j));
  //         int32_t conv_kernel=0, register_mode=0, flow_mode=2, work_mode=0;
  //         a=(v_ch<<25)|(v_x<<20)|(v_addr & 0xFFFFF);
  //         b=(h_ch<<25)|(h_x<<20)|(h_addr & 0xFFFFF);
  //         c=(o_ch<<25)|(o_x<<20)|(o_addr & 0xFFFFF);
  //         d=(cutbit<<18)|(shift_mode<<17)|(stride_mode<<16)|(transpose_mode<<14)|(conv_kernel<<12)|(register_mode<<10)|(flow_mode<<8)|(work_mode<<6)|flow_loop_times;
  //         e=1;
  //         f=(bias_address<<8)|id;
  //         __kuiloong_ace_msetins1(b,a);
  //         __kuiloong_ace_msetins2(d,c);
  //         __kuiloong_ace_msetins3(f,e);
  //       }

  //       int32_t newK = rest;
  //       int32_t h_ch=0,h_x=newK/us,v_ch=us,v_x=N/us,o_ch=1,o_x=N/us;
  //       int32_t id=0,bias_address=0,flow_loop_times=newK/us;
  //       int32_t stride_mode=0,transpose_mode=1;
  //       int32_t cutbit=4, shift_mode=0;
  //       int32_t h_addr=(int32_t)(uintptr_t)(newA[chunk] + us*newK*i);
  //       int32_t v_addr=(int32_t)(uintptr_t)(newB[chunk] + us*j);
  //       int32_t o_addr=(int32_t)(uintptr_t)(output_blockD + us*(N*i+j));
  //       int32_t conv_kernel=0, register_mode=0, flow_mode=0, work_mode=0;
  //       a=(v_ch<<25)|(v_x<<20)|(v_addr & 0xFFFFF);
  //       b=(h_ch<<25)|(h_x<<20)|(h_addr & 0xFFFFF);
  //       c=(o_ch<<25)|(o_x<<20)|(o_addr & 0xFFFFF);
  //       d=(cutbit<<18)|(shift_mode<<17)|(stride_mode<<16)|(transpose_mode<<14)|(conv_kernel<<12)|(register_mode<<10)|(flow_mode<<8)|(work_mode<<6)|flow_loop_times;
  //       e=1;
  //       f=(bias_address<<8)|id;
  //       __kuiloong_ace_msetins1(b,a);
  //       __kuiloong_ace_msetins2(d,c);
  //       __kuiloong_ace_msetins3(f,e);
  //     }
  //   }
  //   for (int i = 0; i <= chunk; i++) {
  //     free(newA[i]);
  //     free(newB[i]);
  //   }
  // } else {
    // int32_t h_ch=0,h_x=K/us,v_ch=us,v_x=N/us,o_ch=1,o_x=N/us;
    a_ch=0,a_x=K/us,b_ch=us,b_x=N/us,d_ch=1,d_x=N/us;
    int32_t id=0,bias_address=0,flow_loop_times=K/us;
    int32_t stride_mode=0,transpose_mode=1;
    int32_t cutbit=4, shift_mode=0;
    int32_t register_mode=0, conv_kernel=0, work_mode=0, flow_mode=0;
    for (int i = 0; i < M/us; i++) {
      for (int j = 0; j < N/us; j++) {
        a_addr=(int32_t)(uintptr_t)(input_blockA + us*K*i);
        b_addr=(int32_t)(uintptr_t)(input_blockB + us*j);
        d_addr=(int32_t)(uintptr_t)(output_blockD + us*(N*i + j));
        ins1 = (transpose_mode&0x03)|((cutbit&0x1f) << 2);
        ins2 = (register_mode&0x3)|((conv_kernel&0x3)<<2)|((stride_mode&0x1)<<4)|((shift_mode&0x1)<< 5);
        ins3 = (b_ch&0xFF)|((a_ch&0xFF)<<8)|((d_ch&0xFF)<<16);
        ins4 = (b_x&0x7F)|((a_x&0x7F)<<8)|((d_x&0x7F)<<16);
        ins5 = a_addr&0xFFFFF;
        ins6 = b_addr&0xFFFFF;
        ins7 = (d_addr&0xFFFFF)|((work_mode&0x3)<<20)|((flow_mode&0x3)<<22)|((flow_loop_times&0x3F)<<24);
        ins8 = (1&0x1)|((id&0xFF)<<1);
        __kuiloong_ace_msetins1(ins1, ins2);
        __kuiloong_ace_msetins2(ins3, ins4);
        __kuiloong_ace_msetins3(ins5, ins6);
        __kuiloong_ace_msetins4(ins7, ins8);
        // a=(v_ch<<25)|(v_x<<20)|(v_addr & 0xFFFFF);
        // b=(h_ch<<25)|(h_x<<20)|(h_addr & 0xFFFFF);
        // c=(o_ch<<25)|(o_x<<20)|(o_addr & 0xFFFFF);
        // d=(cutbit<<18)|(shift_mode<<17)|(stride_mode<<16)|(transpose_mode<<14)|(0<<12)|(0<<10)|(0<<8)|(0<<6)|flow_loop_times;
        // e=1;
        // f=(bias_address<<8)|id;
        // __kuiloong_ace_msetins1(b,a);
        // __kuiloong_ace_msetins2(d,c);
        // __kuiloong_ace_msetins3(f,e);
      }
    // }
  }

}

void fc_multi8(int8_t *input_blockX, int8_t *input_blockW, int8_t *intput_bias, int8_t *output_blockY, int B, int Di, int Do) {
  const int us=8;
	ACENN_DEBUG("SAU fc_multi8 START: A[%d,%d] B[%d,%d] + bias[%d] -> D[%d,%d]", B, Di, Di, Do, Do, B, Do);
  // 配置寄存器
  int32_t a,b,c,d,e,f;
  int32_t h_ch=0,h_x=Di/us,v_ch=us,v_x=Do/us,o_ch=1,o_x=Do/us;
  int32_t id=0, flow_loop_times=Di/us;
  int32_t stride_mode=0,transpose_mode=1;
  int32_t cutbit=4, shift_mode=0;
  for (int i = 0; i < B/us; i++) {
    for (int j = 0; j < Do/us; j++) {
      int32_t h_addr=(int32_t)(uintptr_t)(input_blockX + us*Di*i);
      int32_t v_addr=(int32_t)(uintptr_t)(input_blockW + us*j);
      int32_t o_addr=(int32_t)(uintptr_t)(output_blockY + us*(Do*i + j));
      int32_t bias_address=(int32_t)(uintptr_t)(intput_bias + us*j);
      a=(v_ch<<25)|(v_x<<20)|(v_addr & 0xFFFFF);
      b=(h_ch<<25)|(h_x<<20)|(h_addr & 0xFFFFF);
      c=(o_ch<<25)|(o_x<<20)|(o_addr & 0xFFFFF);
      int32_t conv_kernel=0, register_mode=0, flow_mode=0, work_mode=1;
      d=(cutbit<<18)|(shift_mode<<17)|(stride_mode<<16)|(transpose_mode<<14)|(conv_kernel<<12)|(register_mode<<10)|(flow_mode<<8)|(work_mode<<6)|flow_loop_times;
      e=1;
      f=(bias_address<<8)|id;
      __kuiloong_ace_msetins1(b,a);
      __kuiloong_ace_msetins2(d,c);
      __kuiloong_ace_msetins3(f,e);
    }
  }
}