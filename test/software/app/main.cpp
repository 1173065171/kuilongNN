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
	for (int i = 0; i < SIZE * SIZE; i++) {
		// if (matrixC[i] != matrixD[i]) {
		ACENN_DEBUG("Mistake at C[%d]: expected %d, got %d", 
					i, matrixD[i], matrixC[i]);
		// }
	}
	ACENN_DEBUG("Matrix dot PASS!");
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
	for (int k = 0; k < 16 ; k++){
	  ACENN_DEBUG("Avalue%d is hex: %x dec: %d\n", k,matrixA[k],matrixA[k]);
	  ACENN_DEBUG("Bvalue%d is hex: %x dec: %d\n", k,matrixB[k],matrixB[k]);
	  ACENN_DEBUG("Cvalue%d is hex: %x dec: %d\n", k,matrixC[k],matrixC[k]);
	}
	for (int i = 0; i < SIZE * SIZE; i++) {
	    if (matrixC[i] != matrixD[i]) {
	        ACENN_DEBUG("Mistake at C[%d]: expected %d, got %d", 
	                  i, matrixD[i], matrixC[i]);
	        asm("ebreak;");
	    }
	}
	ACENN_DEBUG("Matrix add PASS!");
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
	for (int k = 0; k < 16 ; k++){
		ACENN_DEBUG("Avalue%d is %d\n", k,matrixA[k]);
		ACENN_DEBUG("Bvalue%d is %d\n", k,matrixB[k]);
		ACENN_DEBUG("Cvalue%d is %d\n", k,matrixC[k]);
	}
	for (int i = 0; i < SIZE * SIZE; i++) {
	    if (matrixC[i] != matrixD[i]) {
	        ACENN_DEBUG("Mistake at C[%d]: expected %d, got %d", 
	                  i, matrixD[i], matrixC[i]);
	        asm("ebreak;");
	    }
	}
	ACENN_DEBUG("Matrix compare PASS!");
}






int main()
{ 
	long BeginInsn, EndInsn;
	BeginInsn = insn();

	malloc_initial();

	int8_t *matrix1=(int8_t *)malloc(1024);
	int8_t *matrix2=(int8_t *)malloc(1024);
	int8_t *matrix3=(int8_t *)malloc(1024);

	// test_printf_();
	// test_malloc();
	// asm("ebreak;");
	// veu_benchmark();
	// sau_benchmark();
	// matrix_multiply_fixed();

	matrix_dot();

	EndInsn = insn();
	ACENN_DEBUG("test insn: %ld", EndInsn - BeginInsn);

	asm("ebreak;"); // 中断，停止运行

}



