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
    a_ch=0,a_x=K/us,b_ch=us,b_x=N/us,d_ch=1,d_x=N/us;
    int32_t id=0,bias_address=0;
    int32_t stride_mode=0,transpose_mode=1;
    int32_t cutbit=4, shift_mode=0;
    int32_t register_mode=0, conv_kernel=0, work_mode=0;
    for (int i = 0; i < M/us; i++) {
      for (int j = 0; j < N/us; j++) {
        int32_t flow_loop_times=K/us;
        int32_t new_flow_loop_times;
        int32_t flow_mode=0;
        if (flow_loop_times >= 64) {
          // flow_times分段 256byte / 32次
          int chunk = K/us/32;
          int rest = flow_loop_times - 32*chunk;
          if (rest == 0) {
            for (int k = 0; k < chunk; k++) {
              new_flow_loop_times = 32;
              if (k == chunk - 1)
                flow_mode = 0;
              else
                flow_mode = 2;
              a_addr=(int32_t)(uintptr_t)(input_blockA + 256*k + us*K*i);
              b_addr=(int32_t)(uintptr_t)(input_blockB + N*256*k + us*j);
              d_addr=(int32_t)(uintptr_t)(output_blockD + us*(N*i + j));
              ins2 = (transpose_mode&0x03)|((cutbit&0x1f) << 2);
              ins1 = (register_mode&0x3)|((conv_kernel&0x3)<<2)|((stride_mode&0x1)<<4)|((shift_mode&0x1)<< 5);
              ins4 = (b_ch&0xFF)|((a_ch&0xFF)<<8)|((d_ch&0xFF)<<16);
              ins3 = (b_x&0x7F)|((a_x&0x7F)<<8)|((d_x&0x7F)<<16);
              ins6 = a_addr&0xFFFFF;
              ins5 = b_addr&0xFFFFF;
              ins8 = (0&0xFFFFF)|((work_mode&0x3)<<20)|((flow_mode&0x3)<<22)|((new_flow_loop_times&0x3F)<<24);
              ins7 = (1&0x1)|((id&0xFF)<<1)|((d_addr&0xFFFFF)<<9);
              __kuiloong_ace_msetins1(ins1, ins2);
              __kuiloong_ace_msetins2(ins3, ins4);
              __kuiloong_ace_msetins3(ins5, ins6);
              __kuiloong_ace_msetins4(ins7, ins8);
            }
          } else {
            for (int k = 0; k < chunk; k++) {
              new_flow_loop_times = 32;
              flow_mode = 2;
              a_addr=(int32_t)(uintptr_t)(input_blockA + 256*k + us*K*i);
              b_addr=(int32_t)(uintptr_t)(input_blockB + N*256*k + us*j);
              d_addr=(int32_t)(uintptr_t)(output_blockD + us*(N*i + j));
              ins2 = (transpose_mode&0x03)|((cutbit&0x1f) << 2);
              ins1 = (register_mode&0x3)|((conv_kernel&0x3)<<2)|((stride_mode&0x1)<<4)|((shift_mode&0x1)<< 5);
              ins4 = (b_ch&0xFF)|((a_ch&0xFF)<<8)|((d_ch&0xFF)<<16);
              ins3 = (b_x&0x7F)|((a_x&0x7F)<<8)|((d_x&0x7F)<<16);
              ins6 = a_addr&0xFFFFF;
              ins5 = b_addr&0xFFFFF;
              ins8 = (0&0xFFFFF)|((work_mode&0x3)<<20)|((flow_mode&0x3)<<22)|((new_flow_loop_times&0x3F)<<24);
              ins7 = (1&0x1)|((id&0xFF)<<1)|((d_addr&0xFFFFF)<<9);
              __kuiloong_ace_msetins1(ins1, ins2);
              __kuiloong_ace_msetins2(ins3, ins4);
              __kuiloong_ace_msetins3(ins5, ins6);
              __kuiloong_ace_msetins4(ins7, ins8);
            }
            new_flow_loop_times = rest;
            flow_mode = 0;
            a_addr=(int32_t)(uintptr_t)(input_blockA + 256*(chunk-1) + rest*us + us*K*i);
            b_addr=(int32_t)(uintptr_t)(input_blockB + N*256*(chunk-1) + N*rest*us + us*j);
            d_addr=(int32_t)(uintptr_t)(output_blockD + us*(N*i + j));
            ins2 = (transpose_mode&0x03)|((cutbit&0x1f) << 2);
            ins1 = (register_mode&0x3)|((conv_kernel&0x3)<<2)|((stride_mode&0x1)<<4)|((shift_mode&0x1)<< 5);
            ins4 = (b_ch&0xFF)|((a_ch&0xFF)<<8)|((d_ch&0xFF)<<16);
            ins3 = (b_x&0x7F)|((a_x&0x7F)<<8)|((d_x&0x7F)<<16);
            ins6 = a_addr&0xFFFFF;
            ins5 = b_addr&0xFFFFF;
            ins8 = (0&0xFFFFF)|((work_mode&0x3)<<20)|((flow_mode&0x3)<<22)|((new_flow_loop_times&0x3F)<<24);
            ins7 = (1&0x1)|((id&0xFF)<<1)|((d_addr&0xFFFFF)<<9);
            __kuiloong_ace_msetins1(ins1, ins2);
            __kuiloong_ace_msetins2(ins3, ins4);
            __kuiloong_ace_msetins3(ins5, ins6);
            __kuiloong_ace_msetins4(ins7, ins8);
          }
        } else {
          a_addr=(int32_t)(uintptr_t)(input_blockA + us*K*i);
          b_addr=(int32_t)(uintptr_t)(input_blockB + us*j);
          d_addr=(int32_t)(uintptr_t)(output_blockD + us*(N*i + j));
          ins2 = (transpose_mode&0x03)|((cutbit&0x1f) << 2);
          ins1 = (register_mode&0x3)|((conv_kernel&0x3)<<2)|((stride_mode&0x1)<<4)|((shift_mode&0x1)<< 5);
          ins4 = (b_ch&0xFF)|((a_ch&0xFF)<<8)|((d_ch&0xFF)<<16);
          ins3 = (b_x&0x7F)|((a_x&0x7F)<<8)|((d_x&0x7F)<<16);
          ins6 = a_addr&0xFFFFF;
          ins5 = b_addr&0xFFFFF;
          ins8 = (0&0xFFFFF)|((work_mode&0x3)<<20)|((flow_mode&0x3)<<22)|((flow_loop_times&0x3F)<<24);
          ins7 = (1&0x1)|((id&0xFF)<<1)|((d_addr&0xFFFFF)<<9);
          __kuiloong_ace_msetins1(ins1, ins2);
          __kuiloong_ace_msetins2(ins3, ins4);
          __kuiloong_ace_msetins3(ins5, ins6);
          __kuiloong_ace_msetins4(ins7, ins8);
        }
      }
  }

}

void fc_multi8(int8_t *input_blockA, int8_t *input_blockB, int8_t *intput_bias, int8_t *output_blockD, int M, int K, int N) {
  const int us=8;
	ACENN_DEBUG("SAU fc_multi8 START: A[%d,%d] B[%d,%d] + bias[%d] -> D[%d,%d]", M, K, K, N, N, M, N);
  // 配置寄存器
  uint32_t ins1,ins2,ins3,ins4,ins5,ins6,ins7,ins8;
  uint32_t a_addr,b_addr,d_addr;
  uint32_t a_ch,b_ch,d_ch,a_x,b_x,d_x;
  a_ch=0,a_x=K/us,b_ch=us,b_x=N/us,d_ch=1,d_x=N/us;
  uint32_t id=0,bias_address=0;
  uint32_t stride_mode=0,transpose_mode=1;
  uint32_t cutbit=4, shift_mode=0;
  uint32_t register_mode=0, conv_kernel=0, work_mode=1;
  uint32_t flow_loop_times =K/us, flow_mode = 0;
  for (int i = 0; i < M/us; i++) {
    for (int j = 0; j < N/us; j++) {
      a_addr=(int32_t)(uintptr_t)(input_blockA + us*K*i);
      b_addr=(int32_t)(uintptr_t)(input_blockB + us*j);
      d_addr=(int32_t)(uintptr_t)(output_blockD + us*(N*i + j));
      int32_t bias_address=(int32_t)(uintptr_t)(intput_bias + us*j);
      ins2 = (transpose_mode&0x03)|((cutbit&0x1f) << 2);
      ins1 = (register_mode&0x3)|((conv_kernel&0x3)<<2)|((stride_mode&0x1)<<4)|((shift_mode&0x1)<< 5);
      ins4 = (b_ch&0xFF)|((a_ch&0xFF)<<8)|((d_ch&0xFF)<<16);
      ins3 = (b_x&0x7F)|((a_x&0x7F)<<8)|((d_x&0x7F)<<16);
      ins6 = a_addr&0xFFFFF;
      ins5 = b_addr&0xFFFFF;
      ins8 = (bias_address&0xFFFFF)|((work_mode&0x3)<<20)|((flow_mode&0x3)<<22)|((flow_loop_times&0x3F)<<24);
      ins7 = (1&0x1)|((id&0xFF)<<1)|((d_addr&0xFFFFF)<<9);
      __kuiloong_ace_msetins1(ins1, ins2);
      __kuiloong_ace_msetins2(ins3, ins4);
      __kuiloong_ace_msetins3(ins5, ins6);
      __kuiloong_ace_msetins4(ins7, ins8);
    }
  }
}