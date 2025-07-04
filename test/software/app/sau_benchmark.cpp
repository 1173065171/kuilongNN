#include "sau_benchmark.hpp"
#include "matrixA.hpp"
#include "matrixB.hpp"
#define SIZE 32
#define BLOCK_SIZE 16


void matrix_csr_set_test()
{
	ACENN_DEBUG("sau test once ins test start!");
	// 0xAB,A是高位
	// 4020041020100010
	// 0000758440204400
	// 0000000000000001
	int a, b, c, d, e, f, r;
	a = 0x40202020;
	b = 0x40100020;
	c = 0x00017304;
	d = 0x4020a000;
	e = 0x00000000;
	f = 0x00000001;

	__kuiloong_ace_msetins1(b, a);
	__kuiloong_ace_msetins2(d, c);
	__kuiloong_ace_msetins3(f, e);
	r = __kuiloong_ace_mgetins3lsb();
	if (r != b) {
		ACENN_DEBUG("Read %x", r);
		ACENN_DEBUG("VEURADDR1 failed to set %x", a);
		asm("ebreak;");
	}
	ACENN_DEBUG("matrix once ins test PASS!");
}


void matrix_multiply(int8_t *matrix1,int8_t *matrix2,int8_t *matrix3) {
	for (int i = 0; i < SIZE * SIZE; ++i){
		matrix1[i]=matrixA[i] ;
		matrix2[i]=matrixB[i] ;
	}
	ACENN_DEBUG("Matrix multiply start!");

	int a, b, c, d, e, f, r;
	e = 0x00000001;
	uint32_t h_ch = 1;    
	uint32_t h_x  = 2;   
	uint32_t v_ch = 1;   
	uint32_t v_x  = 2;    
	uint32_t o_ch = 1;    
	uint32_t o_x  = 2; 
	uint32_t id = 0;
	uint32_t v_addr = 0;
	uint32_t h_addr = 0;
	uint32_t o_addr = 0;
	uint32_t stride_mode = 0;
	uint32_t transpose_mode = 0;
	uint32_t conv_kernel = 0;
	uint32_t register_mode = 0;
	uint32_t flow_mode = 0;
	uint32_t work_mode = 0;
	int flow_mode_counter = 0;
	for (int i = 0; i < SIZE; i += BLOCK_SIZE) { 
		for (int j = 0; j < SIZE; j += BLOCK_SIZE) {
			for (int k = 0; k < SIZE; k += BLOCK_SIZE){
				v_addr = (uint32_t)(&matrix1[i * SIZE + k] - 0x50000000);
				h_addr = (uint32_t)(&matrix2[k * SIZE + j] - 0x50000000);
				o_addr = (uint32_t)(&matrix3[i * SIZE + j] - 0x50000000);
				if (flow_mode_counter < SIZE / 16 - 1) {
					flow_mode = 2; 
					} else {
					flow_mode = 0; 
					}
				flow_mode_counter = (flow_mode_counter + 1) % (SIZE / 16);
				b = (v_ch << 25) | (v_x << 20) | (v_addr);
				a = (h_ch << 25) | (h_x << 20) | (h_addr);
				c = (o_ch << 25) | (o_x << 20) | (o_addr);
				d = (1 << 16) | (2 << 14) |(0 << 12)|(0 << 10)|(flow_mode << 8)|(0 << 6)|(1);
				f = (0 << 8) | id;
				__kuiloong_ace_msetins1(a, b); 
				__kuiloong_ace_msetins2(c, d);       
				__kuiloong_ace_msetins3(e, f); 
				id += 1;
			}
		}
	}
	// for (int p = 0; p < SIZE * SIZE; p += 1){
	//     if (matrix3[p] != matrixD[p]){
	//       ACENN_DEBUG("Mistake at C[%d]: expected %d, got %d", p, matrixD[p], matrix3[p]);
	//     }   
	// }
	// ACENN_DEBUG("Matrix multiply completed!");
}


void sau_benchmark()
{
	ACENN_DEBUG("SAU BENCHMARK START!");

	matrix_csr_set_test();
	// matrix_multiply(matrixA, matrixB, matrixA);

	ACENN_DEBUG("SAU BENCHMARK PASS!");
}

