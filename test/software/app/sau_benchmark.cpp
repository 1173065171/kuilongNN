#include "sau_benchmark.hpp"
#define SIZE 32


void matrix_csr_set_test()
{
	ACENN_DEBUG("matrix once ins test start!");
	// 0xAB,A是高位
	// 4020041020100010
	// 0000758440204400
	// 0000000000000001
	int a, b, c, d, e, f, r;//int horizontal。。。
	a = 0x40202020;
	b = 0x40100020;
	c = 0x00017304;
	d = 0x4020a000;
	e = 0x00000000;
	f = 0x00000001;
	// 写一条指令，并读取指令观察寄存器  指令项拼成32位
	int horizontal_address = 0x02020;
	int vertical_address = 0x100020;
	int output_address = 0x17304;


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


/* void vector_add_fixed() {
    ACENN_DEBUG("Vector addition start!");
	int a, b, c, d, e, f, r;//int horizontal。。。
	a = 0x40202020;
	b = 0x40100020;
	c = 0x00017304;
	d = 0x4020a000;
	e = 0x00000000;
	f = 0x00000001;

	int horizontal_address = 0x02020;
	int vertical_address = 0x100020;
	int output_address = 0x17304;

    
    __kuiloong_ace_vsetcsr(SIZE, VEUVLEN);    
    __kuiloong_ace_vsetcsr(CFG_I8, VEUCFG);   
    
    __kuiloong_ace_msetins1(a, b);            
    __kuiloong_ace_msetins2(c, d);            
    __kuiloong_ace_msetins3(f, e);         

    int8_t vectorC[SIZE];
    
    for (int i = 0; i < SIZE; i++) {
        vectorC[i] = 0;
    }
    
    for (int i = 0; i < SIZE; i++) {
        vectorC[i] = (int8_t)(vectorA[i] + vectorB[i]); 

    // __kuiloong_ace_vadd();  
    
    ACENN_DEBUG("Vector addition completed!");
} */

void sau_benchmark()
{
	ACENN_DEBUG("SAU BENCHMARK START!");

	matrix_csr_set_test();

	ACENN_DEBUG("SAU BENCHMARK PASS!");
}

