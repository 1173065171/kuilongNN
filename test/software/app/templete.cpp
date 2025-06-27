#include "stdint.h"
#include <string.h> 
#include "stdlib/printf.hpp"
#include "stdlib/malloc.hpp"
#include "veu_benchmark.hpp"
#include "sau_benchmark.hpp"
#include "matrixA.hpp"
#include "matrixB.hpp"
#include "matrixD.hpp"
#include "bias.hpp"
#include "input_block.hpp"
#include "output_block.hpp"
#include "kernel.hpp"

#define SIZE 32
#define BLOCK_SIZE 16
#define PADDING       1 
#define STRIDE        1  
#define KERNEL_SIZE   3
#define OUT_SIZE 32

#define Printf printf_


long insn()
{
	int insns;
	asm volatile ("rdinstret %0" : "=r"(insns));
	// printf("[insn() -> %d]", insns);
	return insns;
}

void matrix_dot() {
  // ACENN_DEBUG("Matrix dot start!");
  int8_t matrixC[SIZE * SIZE] = {0};

  __kuiloong_ace_vsetcsr(RADDR3_EN(0) | 
                        SCALAR_EN(0) |  
                        WRITE_BACK(1) | 
                        CLEAR_OB(0) | 
                        MODE(CFG_I8) | 
                        SHIFT(0) | 
                        RESET(0), VEUCFG);     
  __kuiloong_ace_vsetcsr(128, VEUVLEN);   
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)matrixC, VEUWADDR);
  for (int i = 0; i < SIZE * SIZE; i+=16) {
        __kuiloong_ace_vsetcsr(128, VEUVLEN);   
        __kuiloong_ace_vmul((uint32_t)&matrixA[i],  (uint32_t)&matrixB[i]); 
  }

  // for (int i = 0; i < SIZE * SIZE; i++) {
  //     // if (matrixC[i] != matrixD[i]) {
  //         ACENN_DEBUG("Mistake at C[%d]: expected %d, got %d", 
  //                   i, matrixD[i], matrixC[i]);
   
  //     // }
  // }
  // ACENN_DEBUG("Matrix dot PASS!");
}

void matrix_dotcpu(int8_t *matrix1,int8_t *matrix2) {
  int8_t matrixC[SIZE * SIZE] = {0};
	for (int i = 0; i < 1024; ++i){
		matrix1[i]=matrixA[i] ;
		matrix2[i]=matrixB[i] ;
  	}
  for(int i = 0; i < SIZE * SIZE; ++i){
    matrixC[i]=matrix1[i] * matrix2[i] ;
  }
//   return matrixC[0];
}

void matrix_addcpu(int8_t *matrix1,int8_t *matrix2) {
  int8_t matrixC[SIZE * SIZE] = {0};
	for (int i = 0; i < 1024; ++i){
		matrix1[i]=matrixA[i] ;
		matrix2[i]=matrixB[i] ;
  	}
  for(int i = 0; i < SIZE * SIZE; ++i){
    matrixC[i]=matrix1[i] + matrix2[i] ;
  }
//   return matrixC[0];
}



void matrix_add() {
  // ACENN_DEBUG("Matrix add start!");
  int8_t matrixC[SIZE * SIZE] = {0};

  __kuiloong_ace_vsetcsr(RADDR3_EN(0) | 
                        SCALAR_EN(0) |  
                        WRITE_BACK(1) | 
                        CLEAR_OB(0) | 
                        MODE(CFG_I8) | 
                        SHIFT(0) | 
                        RESET(0), VEUCFG);     
  __kuiloong_ace_vsetcsr(128, VEUVLEN);   
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)matrixC, VEUWADDR);
  for (int i = 0; i < SIZE * SIZE; i+=16) {
        __kuiloong_ace_vsetcsr(128, VEUVLEN);   
        __kuiloong_ace_vadd((uint32_t)&matrixA[i],  (uint32_t)&matrixB[i]); 
  }
  // for (int k = 0; k < 16 ; k++){
  //   ACENN_DEBUG("Avalue%d is hex: %x dec: %d\n", k,matrixA[k],matrixA[k]);
  //   ACENN_DEBUG("Bvalue%d is hex: %x dec: %d\n", k,matrixB[k],matrixB[k]);
  //   ACENN_DEBUG("Cvalue%d is hex: %x dec: %d\n", k,matrixC[k],matrixC[k]);
  // }
  // for (int i = 0; i < SIZE * SIZE; i++) {
  //     if (matrixC[i] != matrixD[i]) {
  //         ACENN_DEBUG("Mistake at C[%d]: expected %d, got %d", 
  //                   i, matrixD[i], matrixC[i]);
  //         asm("ebreak;");
  //     }
  // }
  // ACENN_DEBUG("Matrix add PASS!");
}
void matrix_comparecpu(int8_t *matrix1,int8_t *matrix2) {
  int8_t matrixC[SIZE * SIZE] = {0};
	for (int i = 0; i < 1024; ++i){
		matrix1[i]=matrixA[i] ;
		matrix2[i]=matrixB[i] ;
  	}
  for(int i = 0; i < SIZE * SIZE; ++i){
    if (matrix1[i] <= matrix2[i]){
      matrixC[i]=matrix1[i];
    }
      else{
        matrixC[i]=matrix2[i] ;
      }
  }

}

void matrix_compare() {
  // ACENN_DEBUG("Matrix compare start!");
  int8_t matrixC[SIZE * SIZE] = {0};

  __kuiloong_ace_vsetcsr(RADDR3_EN(0) | 
                        SCALAR_EN(0) |  
                        WRITE_BACK(1) | 
                        CLEAR_OB(0) | 
                        MODE(CFG_I8) | 
                        SHIFT(0) | 
                        RESET(0), VEUCFG);     
  __kuiloong_ace_vsetcsr(128, VEUVLEN);   
  __kuiloong_ace_vsetcsr(0xFFFFFFFF, VEUMASK);
  __kuiloong_ace_vsetcsr((uint32_t)matrixC, VEUWADDR);
  for (int i = 0; i < SIZE * SIZE; i+=16) {
        __kuiloong_ace_vsetcsr(128, VEUVLEN);   
        __kuiloong_ace_vmin((uint32_t)&matrixA[i],  (uint32_t)&matrixB[i]);
  }
  // for (int k = 0; k < 16 ; k++){
  //   ACENN_DEBUG("Avalue%d is %d\n", k,matrixA[k]);
  //   ACENN_DEBUG("Bvalue%d is %d\n", k,matrixB[k]);
  //   ACENN_DEBUG("Cvalue%d is %d\n", k,matrixC[k]);
  // }
  // for (int i = 0; i < SIZE * SIZE; i++) {
  //     if (matrixC[i] != matrixD[i]) {
  //         ACENN_DEBUG("Mistake at C[%d]: expected %d, got %d", 
  //                   i, matrixD[i], matrixC[i]);
  //         asm("ebreak;");4
  //     }
  // }
  // ACENN_DEBUG("Matrix compare PASS!");
}
void matmul_32x32(int8_t *matrix1,int8_t *matrix2,int8_t *matrix3) {

  for (int i = 0; i < SIZE * SIZE; ++i){
    matrix1[i]=matrixA[i] ;
    matrix2[i]=matrixB[i] ;
 
  }
    // 三重循环实现矩阵乘
    for (int i = 0; i < SIZE; ++i) {
        for (int k = 0; k < SIZE; ++k) {
            int32_t a_val =  matrix1[i * SIZE + k];
            for (int j = 0; j < SIZE; ++j) {
                // C[i][j] += A[i][k] * B[k][j];
                matrix3[i * SIZE + j] += a_val * matrix2[k * SIZE + j];
            }
        }
    }
}
// void matrix_multiply(int8_t *matrix1,int8_t *matrix2,int8_t *matrix3) {

//   for (int i = 0; i < SIZE * SIZE; ++i){
//     matrix1[i]=matrixA[i] ;
//     matrix2[i]=matrixB[i] ;
 
//   }

//   // ACENN_DEBUG("Matrix multiply start!");
//   int a, b, c, d, e, f, r;
  
//   e = 0x00000001;
//   uint32_t h_ch,h_x,v_ch,v_x,o_ch,o_x,id,v_addr,h_addr,o_addr,stride_mode,transpose_mode,conv_kernel,register_mode,flow_mode,work_mode;
//   h_ch = 1;    
//   h_x  = SIZE/16;   
//   v_ch = 1;   
//   v_x  = SIZE/16;    
//   o_ch = 1;    
//   o_x  = SIZE/16; 
//   id = 0;
//   v_addr = 0;
//   h_addr = 0;
//   o_addr = 0;
//   stride_mode = 0;
//   transpose_mode = 0;
//   conv_kernel = 0;
//   register_mode = 0;
//   flow_mode = 0;
//   work_mode = 0;
//   int flow_mode_counter = 0;

//   for (int i = 0; i < SIZE; i += BLOCK_SIZE) { 
//     for (int j = 0; j < SIZE; j += BLOCK_SIZE) {
//       for (int k = 0; k < SIZE; k += BLOCK_SIZE){
//        v_addr = (uint32_t)(&matrix1[i * SIZE + k] - 0x50000000);
//        h_addr = (uint32_t)(&matrix2[k * SIZE + j] - 0x50000000);
//        o_addr = (uint32_t)(&matrix3[i * SIZE + j] - 0x50000000);
//        if (flow_mode_counter < SIZE / 16 - 1) {
//         flow_mode = 2; 
//         } else {
//         flow_mode = 0; 
//         }
//        flow_mode_counter = (flow_mode_counter + 1) % (SIZE / 16);
       
//        b = (v_ch << 25) | (v_x << 20) | (v_addr);
//        a = (h_ch << 25) | (h_x << 20) | (h_addr);
//        c = (o_ch << 25) | (o_x << 20) | (o_addr);
//        d = (1 << 16) | (2 << 14) |(0 << 12)|(0 << 10)|(flow_mode << 8)|(0 << 6)|(1);
//        f = (0 << 8) | id;
//       //  ACENN_DEBUG("a=0x%08X", a);
//       //  ACENN_DEBUG("b=0x%08X", b);
//       //  ACENN_DEBUG("c=0x%08X", c);
//       //  ACENN_DEBUG("Avalue:%d", matrix1[i * SIZE + k]);
 
//        __kuiloong_ace_msetins1(a, b); 
//        __kuiloong_ace_msetins2(c, d);       
//        __kuiloong_ace_msetins3(e, f); 
//        id += 1;

//       }
//     }
//   }
//   // for (int p = 0; p < SIZE * SIZE; p += 1){
//   //     if (matrix3[p] != matrixD[p]){
//   //       ACENN_DEBUG("Mistake at C[%d]: expected %d, got %d", p, matrixD[p], matrix3[p]);
//   //     }   
//   // }
//   // ACENN_DEBUG("Matrix multiply completed!");
// }
void conv_standard_3x3(
    int8_t *kernel1,int8_t *input_block2,int8_t *output_block3
) {
    for (int i = 0; i < 2048; ++i){
    kernel1[i]=kernel[i] ;
  }
  for (int i = 0; i < 8192; ++i){
  input_block2[i]=input_block[i] ;
  }

    // 对每个输出通道
    for (int oc = 0; oc < 16; ++oc) {
        // 对输出空间的每个位置
        for (int oy = 0; oy < 30; ++oy) {
            for (int ox = 0; ox < 30; ++ox) {
                int32_t sum = 0;
                // 对每个输入通道应用 3×3 卷积
                for (int ic = 0; ic < 8; ++ic) {
                    // 遍历 3×3 卷积核
                    for (int ky = 0; ky < 3; ++ky) {
                        for (int kx = 0; kx < 3; ++kx) {
                            // 输入索引
                            int in_index = ic * (32 * 32)
                                         + (oy + ky) * 32
                                         + (ox + kx);
                            // 卷积核组偏移基址
                            int ker_base = oc * 16;
                            // 卷积核在组内的偏移（前 9 个是有效权重）
                            int ker_index = ker_base + ky * 3 + kx;
                            sum += input_block2[in_index] * kernel1[ker_index];
                        }
                    }
                }
                // 写回输出
                int out_index = oc * (30 * 30) + oy * 30 + ox;
                output_block[out_index] = sum;
            }
        }
    }
}

void conv(int8_t *kernel1,int8_t *input_block2,int8_t *output_block3) {
  for (int i = 0; i < 2048; ++i){
    kernel1[i]=kernel[i] ;
  }
  for (int i = 0; i < 8192; ++i){
  input_block2[i]=input_block[i] ;
  }
  int a, b, c, d, e, f, r;
  e = 0x00000001;
  int conv_in_ch = 8, conv_out_ch = 16, l = 0,input_offset = 0,kernel_offset = 0 ,output_offset = 0;
  uint32_t h_ch,h_x,v_ch,v_x,o_ch,o_x,id,v_addr,h_addr,o_addr,stride_mode,transpose_mode,conv_kernel,register_mode,flow_mode,work_mode,bias_address,flow_loop_times;
  h_ch = SIZE;    //SIZE = 32
  h_x  = SIZE/16;   
  v_ch = conv_out_ch;   
  v_x  = 1;    
  o_ch = 32;    
  o_x  = SIZE/16; 
  id = 0;
  v_addr = 0;
  h_addr = 0;
  o_addr = 0;
  stride_mode = 1;
  transpose_mode = 0;
  conv_kernel = 0;
  register_mode = 0;
  flow_mode = 1;
  work_mode = 2;
  bias_address = (uint32_t)(&bias[0]);
  flow_loop_times = 4;
    for (int j = 0; j < 2; j += 1) {
      for (int k = 0; k < OUT_SIZE * 4; k += 1){
          flow_mode = (id % 2 == 0) ? 2 : 1;
          work_mode = (id % 2 == 1) ? 2 : 0; 
          input_offset = (id % 2 == 0) ? 0 : 4096; 
          kernel_offset = (id % 2 == 0) ? 0 : 1024; 
          h_addr = (uint32_t)(&input_block2[k / 2 * OUT_SIZE + input_offset + 16 * j] - 0x50000000);
          v_addr = (uint32_t)(&kernel1[kernel_offset] - 0x50000000);
          o_addr = (uint32_t)(&output_block3[k / 2 * OUT_SIZE + 16 * j] - 0x50000000);
          l += 1;
          a = (v_ch << 25) | (v_x << 20) | (v_addr);
          b = (h_ch << 25) | (h_x << 20) | (h_addr);
          c = (o_ch << 25) | (o_x << 20) | (o_addr);
          d = (1 << 16) | (1 << 14) |(3 << 12)|(0 << 10)|(flow_mode << 8)|(work_mode << 6)|(flow_loop_times);
          f = (bias_address << 8) | id;
          __kuiloong_ace_msetins1(a, b); 
          __kuiloong_ace_msetins2(c, d);       
          __kuiloong_ace_msetins3(e, f); 
          id += 1;
      }
    }
  
  // for (int p = 0; p < 100; p += 1){
  //     if (output_block3[p] != output_block[p]){
  //       ACENN_DEBUG("Mistake at C[%d]: expected %d, got %d", p, output_block[p], output_block3[p]);
  //     }   
  // }
  // ACENN_DEBUG("conv completed!");
}


void conv3x3_stride1_no_padding(int8_t *kernel1,int8_t *input_block2,int8_t *output_block3) 
{
    // 遍历每个输出通道
    for (int oc = 0; oc < 32; ++oc) {
        // 遍历输出特征图的每个空间位置
        for (int oy = 0; oy < 30; ++oy) {
            for (int ox = 0; ox < 30; ++ox) {
                int32_t sum = 0;
                // 对应 16 个输入通道做 3×3 卷积
                for (int ic = 0; ic < 16; ++ic) {
                    // 计算输入通道在一维数组中的起始偏移
                    int in_base = ic * (32 * 32);
                    // 每个卷积核组的基址，每组 16 个元素
                    int ker_base = oc * 16;
                    // 3×3 核上的 9 个有效权重
                    for (int ky = 0; ky < 3; ++ky) {
                        for (int kx = 0; kx < 3; ++kx) {
                            // 输入索引：通道偏移 + 行偏移 + 列偏移
                            int in_index = in_base + (oy + ky) * 32 + (ox + kx);
                            // 卷积核索引：组基址 + ky*3 + kx
                            int ker_index = ker_base + ky * 3 + kx;
                            sum += input_block2[in_index] * kernel1[ker_index];
                        }
                    }
                }
                // 写回输出
                int out_index = oc * (30 * 30) + oy * 30 + ox;
                output_block[out_index] = sum;
            }
        }
    }
}
// void conv2(int8_t *kernel1,int8_t *input_block2,int8_t *output_block3) {

//   for (int i = 0; i < 8192; ++i){
//     kernel1[i]=kernel[i] ;
//   }
//   for (int i = 0; i < 16384; ++i){
//   input_block2[i]=input_block[i] ;
//   }

//   // ACENN_DEBUG("conv start!");
//   int a, b, c, d, e, f, r;
  
//   e = 0x00000001;
//   int conv_in_ch = 16, conv_out_ch = 32, l = 0, input_offset = 4096,  kernel_offset = 2048 , output_offset = 0;
//   uint32_t h_ch,h_x,v_ch,v_x,o_ch,o_x,id,v_addr,h_addr,o_addr,stride_mode,transpose_mode,conv_kernel,register_mode,flow_mode,work_mode,bias_address,flow_loop_times;
//   h_ch = SIZE;    
//   h_x  = SIZE/16;   
//   v_ch = conv_out_ch;   
//   v_x  = 1;    
//   o_ch = 16;    
//   o_x  = SIZE/16; 
//   id = 0;
//   v_addr = 0;
//   h_addr = 0;
//   o_addr = 0;
//   stride_mode = 0;
//   transpose_mode = 0;
//   conv_kernel = 0;
//   register_mode = 0;
//   flow_mode = 1;
//   work_mode = 2;
//   bias_address = 0;
//   flow_loop_times = 4;

//   for (int i = 0; i < 2; i += 1) {
//     for (int j = 0; j < 2; j += 1) {
//       for (int k = 0; k < OUT_SIZE * 4; k += 1){
        
   

//           flow_mode = (id % 4 == 3) ? 1 : 2; 
//           work_mode = (id % 4 == 3) ? 2 : 0; 


         
//           bias_address = (uint32_t)(&bias[i * 16]);
//           // ACENN_DEBUG("times: %d, ib : %d", id + 1,k / 4 * SIZE);
//           h_addr = (uint32_t)(&input_block2[k / 4 * SIZE + l * input_offset + 16 * j] - 0x50000000);
//           v_addr = (uint32_t)(&kernel1[l * kernel_offset + i * 16 * 16] - 0x50000000);
//           o_addr = (uint32_t)(&output_block3[k / 4 * SIZE + 16 * j + i * SIZE * SIZE * 16] - 0x50000000);
          
//           a = (v_ch << 25) | (v_x << 20) | (v_addr);
//           b = (h_ch << 25) | (h_x << 20) | (h_addr);
//           c = (o_ch << 25) | (o_x << 20) | (o_addr);
//           d = (1 << 16) | (1 << 14) |(3 << 12)|(0 << 10)|(flow_mode << 8)|(work_mode << 6)|(flow_loop_times);
//           f = (bias_address << 8) | id;
           
//           // ACENN_DEBUG("times: %d, ib : %d, kernel : %d, ob : %d", id + 1, k / 4 * SIZE + l * input_offset + 16 * j + i * SIZE * SIZE * 8, l * kernel_offset + i * 16 * 16,k / 4 * SIZE + 16 * j + i * SIZE * SIZE * 16);
//           // ACENN_DEBUG("ke : 0x%x", &kernel1[0]);
//           l = (l + 1) % 4;
//           __kuiloong_ace_msetins1(a, b); 
//           __kuiloong_ace_msetins2(c, d);       
//           __kuiloong_ace_msetins3(e, f); 
//           id += 1;
//           if (id = 255) {
//             id = 0;
//           }
//       }
//     }
//   }
  
//   // for (int p = 0; p < 64; p += 1){
      
//   //       ACENN_DEBUG("C[%d]: %d", p, output_block3[p]);
      
//   // }

//   // ACENN_DEBUG("conv completed!");
// }


// void conv2(int8_t *kernel1,int8_t *input_block2,int8_t *output_block3) {

//   for (int i = 0; i < 2048; ++i){
//     kernel1[i]=kernel[i] ;
//   }
//   for (int i = 0; i < 8192; ++i){
//   input_block2[i]=input_block[i] ;
//   }
  
//   // ACENN_DEBUG("conv start!");
//   int a, b, c, d, e, f, r;
  
//   e = 0x00000001;
//   int conv_in_ch = 8, conv_out_ch = 16, l = 0, input_offset = 2048,  kernel_offset = 512, output_offset = 0;
//   uint32_t h_ch,h_x,v_ch,v_x,o_ch,o_x,id,v_addr,h_addr,o_addr,stride_mode,transpose_mode,conv_kernel,register_mode,flow_mode,work_mode,bias_address,flow_loop_times;
//   h_ch = SIZE;    
//   h_x  = SIZE/16;   
//   v_ch = conv_out_ch;   
//   v_x  = 1;    
//   o_ch = 16;    
//   o_x  = OUT_SIZE/16; 
//   id = 0;
//   v_addr = 0;
//   h_addr = 0;
//   o_addr = 0;
//   stride_mode = 0;
//   transpose_mode = 0;
//   conv_kernel = 0;
//   register_mode = 0;
//   flow_mode = 1;
//   work_mode = 2;
//   bias_address = (uint32_t)(&bias[0]);
//   flow_loop_times = 2;

  
//     for (int j = 0; j < 2; j += 1) {
//       for (int k = 0; k < OUT_SIZE * 4; k += 1){

//         flow_mode = (id % 4 == 3) ? 1 : 2; 
//         work_mode = (id % 4 == 3) ? 2 : 0; 
        
        
//           // ACENN_DEBUG("input: %d",input_offset );
//           h_addr = (uint32_t)(&input_block2[k / 4 * 2 * SIZE + l * input_offset + 16 * j] - 0x50000000);
//           v_addr = (uint32_t)(&kernel1[l * kernel_offset] - 0x50000000);
//           o_addr = (uint32_t)(&output_block3[k / 4 * OUT_SIZE] - 0x50000000);
          
          
//           a = (v_ch << 25) | (v_x << 20) | (v_addr);
//           b = (h_ch << 25) | (h_x << 20) | (h_addr);
//           c = (o_ch << 25) | (o_x << 20) | (o_addr);
//           d = (2 << 16) | (1 << 14) |(3 << 12)|(0 << 10)|(flow_mode << 8)|(work_mode << 6)|(flow_loop_times);
//           f = (bias_address << 8) | id;
//           l = (l + 1) % 4;
           
//           // ACENN_DEBUG("times: %d, ib : %d, kernel : %d, ob : %d", id + 1, k / 4 * SIZE + l * input_offset + 16 * j + i * SIZE * SIZE * 8, l * kernel_offset + i * 16 * 16,k / 4 * SIZE + 16 * j + i * SIZE * SIZE * 16);
//           // ACENN_DEBUG("oa : 0x%x", &kernel1[0]);
         
//           __kuiloong_ace_msetins1(a, b); 
//           __kuiloong_ace_msetins2(c, d);       
//           __kuiloong_ace_msetins3(e, f); 
//           id += 1;
        
//       }
//     }
//     // ACENN_DEBUG("kernel address: 0x%x",&output_block3[0] );
  
//   // for (int p = 0; p < 100; p += 1){
//   //     if (output_block3[p] != output_block[p]){
//   //       ACENN_DEBUG("Mistake at C[%d]: expected %d, got %d", p, output_block[p], output_block3[p]);
//   //     }   
//   // }

//   // ACENN_DEBUG("conv completed!");
// }

void pointwise_conv_1x1(
int8_t *kernel1,int8_t *input_block2,int8_t *output_block3
) {
  for (int i = 0; i < 512; ++i){
    kernel1[i]=kernel[i] ;
  }
  for (int i = 0; i < 16384; ++i){
  input_block2[i]=input_block[i] ;
  }

    for (int oc = 0; oc < 32; ++oc) {
        for (int idx = 0; idx < 1024; ++idx) {
            int32_t acc = bias[oc];             // 初始化累加器为偏置
            for (int ic = 0; ic < 16; ++ic) {   // 遍历 16 个输入通道
                acc += input_block[ic * 1024 + idx]  // 取当前通道对应空间位置的输入
                     * kernel[oc * 16 + ic];        // 取对应的 1×1 卷积权重
            }
            output_block[oc * 1024 + idx] = acc;    // 写回输出
        }
    }
}

// void convp(int8_t *kernel1,int8_t *input_block2,int8_t *output_block3) {

//   for (int i = 0; i < 512; ++i){
//     kernel1[i]=kernel[i] ;
//   }
//   for (int i = 0; i < 16384; ++i){
//   input_block2[i]=input_block[i] ;
//   }



//   // ACENN_DEBUG("convp start!");
//   int a, b, c, d, e, f, r;
  
//   e = 0x00000001;
//   int conv_in_ch = 16, conv_out_ch = 32, l = 0;
//   uint32_t h_ch,h_x,v_ch,v_x,o_ch,o_x,id,v_addr,h_addr,o_addr,stride_mode,transpose_mode,conv_kernel,register_mode,flow_mode,work_mode,bias_address,flow_loop_times;
//   h_ch = SIZE;    
//   h_x  = SIZE/16;   
//   v_ch = 1;   
//   v_x  = SIZE/16;    
//   o_ch = 32;    
//   o_x  = SIZE/16; 
//   id = 0;
//   v_addr = 0;
//   h_addr = 0;
//   o_addr = 0;
//   stride_mode = 0;
//   transpose_mode = 0;
//   conv_kernel = 0;
//   register_mode = 0;
//   flow_mode = 0;
//   work_mode = 2;
//   bias_address = 0;
//   flow_loop_times = 1;

//   for (int i = 0; i < 2; i += 1) {
//     for (int j = 0; j < 2; j += 1) {
//       for (int k = 0; k < 32; k += 1){
         
//           bias_address = (uint32_t)(&bias[i * 16]);
//           // ACENN_DEBUG("times: %d, ib : %d", id + 1,k / 4 * SIZE);
//           h_addr = (uint32_t)(&input_block2[k * SIZE + 16 * j] - 0x50000000);
//           v_addr = (uint32_t)(&kernel1[i * 256] - 0x50000000);
//           o_addr = (uint32_t)(&output_block3[k * SIZE + 16 * j + i * SIZE * SIZE * 16] - 0x50000000);
          
//           a = (v_ch << 25) | (v_x << 20) | (v_addr);
//           b = (h_ch << 25) | (h_x << 20) | (h_addr);
//           c = (o_ch << 25) | (o_x << 20) | (o_addr);
//           d = (1 << 16) | (1 << 14) |(1 << 12)|(0 << 10)|(1 << 8)|(0 << 6)|(flow_loop_times);
//           f = (bias_address << 8) | id;
           
//           // ACENN_DEBUG("times: %d, ib : %d, kernel : %d, ob : %d", id + 1, k / 4 * SIZE + l * input_offset + 16 * j + i * SIZE * SIZE * 8, l * kernel_offset + i * 16 * 16,k / 4 * SIZE + 16 * j + i * SIZE * SIZE * 16);
//           // ACENN_DEBUG("ke : 0x%x", &kernel1[0]);
          
//           __kuiloong_ace_msetins1(a, b); 
//           __kuiloong_ace_msetins2(c, d);       
//           __kuiloong_ace_msetins3(e, f); 
//           id += 1;
//           // if (id > 255) {
//           //   id = 0;
//           // }
//       }
//     }
//   }
  
//   // for (int p = 0; p < 64; p += 1){
      
//   //       ACENN_DEBUG("C[%d]: %d", p, output_block3[p]);
      
//   // }

//   // ACENN_DEBUG("conv completed!");
// }



void convd(int8_t *kernel1,int8_t *input_block2,int8_t *output_block3) {

  for (int i = 0; i < 48; ++i){
    kernel1[i]=kernel[i] ;
  }
  for (int i = 0; i < 11536; ++i){
  input_block2[i]=input_block[i] ;
  }
  for (int c = 0; c < 3; ++c) { // 遍历每个通道
    int input_start = c * 62 * 62; // 原数据每个通道的起始位置
    int output_start = c * 64 * 64; // 填充后每个通道的起始位置

    // 填充顶部和底部的行（全0）
    for (int i = 0; i < 64; ++i) {
      input_block2[output_start + i] = 0; // 第一行
      input_block2[output_start + 63*64 + i] = 0; // 最后一行
    }

    // 处理中间的62行
    for (int row = 0; row < 62; ++row) {
      int output_row_start = output_start + (row + 1) * 64;
      input_block2[output_row_start] = 0; // 行首0

      // 复制原数据的62个元素到中间位置
      int input_row_start = input_start + row * 62;
      for (int col = 0; col < 62; ++col) {
        input_block2[output_row_start + 1 + col] = input_block[input_row_start + col];
      }

      input_block2[output_row_start + 63] = 0; // 行尾0
    }
  }
  // for(int i = 0; i < 12288; ++i){
    
  //  ACENN_DEBUG("input_block2[%d]: %d", i, input_block2[i]);
  // }
  // ACENN_DEBUG("conv start!");
  int a, b, c, d, e, f, r;
  
  e = 0x00000001;
  int conv_in_ch = 3, conv_out_ch = 3, l = 0, input_offset = 4096,  kernel_offset = 16, output_offset = 0;
  uint32_t h_ch,h_x,v_ch,v_x,o_ch,o_x,id,v_addr,h_addr,o_addr,stride_mode,transpose_mode,conv_kernel,register_mode,flow_mode,work_mode,bias_address,flow_loop_times;
  h_ch = 1;    
  h_x  = SIZE/16;   
  v_ch = 1;   
  v_x  = 0;    
  o_ch = 1;    
  o_x  = 0; 
  id = 0;
  v_addr = 0;
  h_addr = 0;
  o_addr = 0;
  stride_mode = 0;
  transpose_mode = 0;
  conv_kernel = 0;
  register_mode = 0;
  flow_mode = 1;
  work_mode = 2;
  bias_address = (uint32_t)(&bias[0]);
  flow_loop_times = 1;

  for(int i = 0; i < 2; i += 1) {
    for (int j = 0; j < 2; j += 1) {
      for (int k = 0; k < 61; k += 1){
          // ACENN_DEBUG("input: %d",input_offset );
          h_addr = (uint32_t)(&input_block2[k * 64 + 16 * j + i * input_offset] - 0x50000000);
          v_addr = (uint32_t)(&kernel1[i * kernel_offset] - 0x50000000);
          o_addr = (uint32_t)(&output_block3[k * 64 + 16 * j + i * input_offset] - 0x50000000);
          
          a = (v_ch << 25) | (v_x << 20) | (v_addr);
          b = (h_ch << 25) | (h_x << 20) | (h_addr);
          c = (o_ch << 25) | (o_x << 20) | (o_addr);
          d = (1 << 16) | (1 << 14) |(3 << 12)|(0 << 10)|(1 << 8)|(2 << 6)|(flow_loop_times);
          f = (bias_address << 8) | id;
          
           
          // ACENN_DEBUG("times: %d, ib : %d, kernel : %d, ob : %d", id + 1, k / 4 * SIZE + l * input_offset + 16 * j + i * SIZE * SIZE * 8, l * kernel_offset + i * 16 * 16,k / 4 * SIZE + 16 * j + i * SIZE * SIZE * 16);
          // ACENN_DEBUG("ke : 0x%x", &kernel1[0]);
         
          __kuiloong_ace_msetins1(a, b); 
          __kuiloong_ace_msetins2(c, d);       
          __kuiloong_ace_msetins3(e, f); 
          id += 1;
        
      }
    }
  }
  //   ACENN_DEBUG("kernel address: 0x%x",&output_block3[0] );
  
  // for (int p = 0; p < 100; p += 1){
  //     if (output_block3[p] != output_block[p]){
  //       ACENN_DEBUG("Mistake at C[%d]: expected %d, got %d", p, output_block[p], output_block3[p]);
  //     }   
  // }

  // ACENN_DEBUG("conv completed!");
}

void conv3x3_stride2_no_padding(
    int8_t *kernel1,int8_t *input_block2,int8_t *output_block3
) {
    for (int i = 0; i < 2048; ++i){
    kernel1[i]=kernel[i] ;
  }
  for (int i = 0; i < 8192; ++i){
  input_block2[i]=input_block[i] ;
  }
  
    // 输出空间大小：O = floor((32 - 3) / 2) + 1 = 15
    for (int oc = 0; oc < 16; ++oc) {
        // 遍历每个输出通道
        for (int oy = 0; oy < 15; ++oy) {
            for (int ox = 0; ox < 15; ++ox) {
                int32_t sum = 0;
                // 对应 8 个输入通道做 3×3 卷积
                for (int ic = 0; ic < 8; ++ic) {
                    // 输入通道在一维数组中的基址
                    int in_base = ic * 32 * 32;
                    // 卷积核组在一维数组中的基址（16 元素一组）
                    int ker_base = oc * 16;
                    // 遍历 3×3 核
                    for (int ky = 0; ky < 3; ++ky) {
                        for (int kx = 0; kx < 3; ++kx) {
                            int in_y = oy * 2 + ky;
                            int in_x = ox * 2 + kx;
                            int in_index = in_base + in_y * 32 + in_x;
                            int ker_index = ker_base + ky * 3 + kx;
                            sum += input_block2[in_index] * kernel1[ker_index];
                        }
                    }
                }
                // 写回输出
                int out_index = oc * 15 * 15 + oy * 15 + ox;
                output_block3[out_index] = sum;
            }
        }
    }
}



void depthwise_conv3x3_stride1_padding1(
    int8_t *kernel1,int8_t *input_block2,int8_t *output_block3
) {
    const int CH = 3;
    const int H = 112;
    const int W = 112;
    const int PAD = 1;
    const int KH = 3;
    const int KW = 3;
    
  for (int i = 0; i < 48; ++i){
    kernel1[i]=kernel[i] ;
  }
  for (int i = 0; i < 11536; ++i){
  input_block2[i]=input_block[i] ;
  }

    // 临时 padded 输入，每通道加上下左右 1 个像素：114×114
    int32_t padded_input[CH][H + 2 * PAD][W + 2 * PAD];
    memset(padded_input, 0, sizeof(padded_input));  // 全部初始化为0

    // 把输入复制进 padded_input 中（带padding）
    for (int c = 0; c < CH; ++c) {
        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                padded_input[c][y + PAD][x + PAD] = input_block2[c * H * W + y * W + x];
            }
        }
    }

    // 进行深度卷积（每个通道独立卷积）
    for (int c = 0; c < CH; ++c) {
        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                int32_t sum = 0;
                for (int ky = 0; ky < KH; ++ky) {
                    for (int kx = 0; kx < KW; ++kx) {
                        int input_val = padded_input[c][y + ky][x + kx];
                        int kernel_val = kernel1[c * 16 + ky * 3 + kx];  // 取前9个
                        sum += input_val * kernel_val;
                    }
                }
                output_block[c * H * W + y * W + x] = sum;
            }
        }
    }
}







int main()
{ 
  	long BeginInsn, EndInsn;
	BeginInsn = insn();
	// ACENN_DEBUG("TEST6 START!");

  malloc_initial();
  // // int8_t *kernel1=(int8_t *)malloc(2048);
  // // int8_t *input_block2=(int8_t *)malloc(8192);
  // // int8_t *output_block3=(int8_t *)malloc(14400);
  int8_t *matrix1=(int8_t *)malloc(1024);
  int8_t *matrix2=(int8_t *)malloc(1024);
  int8_t *matrix3=(int8_t *)malloc(1024);

//   test_printf_();
  // test_malloc();
  // asm("ebreak;");q
  //veu_benchmark();q
  //sau_benchmark();
  //matrix_multiply_fixed();

   matrix_dot();
  //  matrix_dotcpu(matrixA,matrixB);
  // matrix_addcpu(matrixA,matrixB);
  // matrix_comparecpu(matrixA,matrixB);
  // matrix_add();
  // matrix_compare();
  // matrix_multiply(matrix1,matrix2,matrix3);
  // matmul_32x32(matrix1,matrix2,matrix3);
  // conv(kernel1,input_block2,output_block3);
  // conv_standard_3x3(kernel1,input_block2,output_block3);
  // conv2(kernel1,input_block2,output_block3);
  // conv3x3_stride2_no_padding(kernel1,input_block2,output_block3);
  // conv3x3_stride1_no_padding(kernel1,input_block2,output_block3);
  // convp(kernel1,input_block2,output_block3);
  // pointwise_conv_1x1(kernel1,input_block2,output_block3);
  // convd(kernel1,input_block2,output_block3);
  // depthwise_conv3x3_stride1_padding1(kernel1,input_block2,output_block3);

	EndInsn = insn();
	ACENN_DEBUG("test insn: %ld", EndInsn - BeginInsn);

  // Printf("TEST6 PASS!\n");
  asm("ebreak;"); // 中断，停止运行
  // while (1)
  // {
  // }
}



