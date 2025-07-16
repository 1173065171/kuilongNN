#include "sau_benchmark_8x8.hpp"
#define SIZE 32
#define BLOCK_SIZE 16


void matrix_csr_set_test_8x8()
{
	ACENN_DEBUG("sau test once ins test start!");

	int a, b, c, d, e, f, r;
	a = 0x10144000;
	b = 0x10144400;
	c = 0x00004001;
	d = 0x10144800;
	e = 0x00000000;
	f = 0x00000001;


	__kuiloong_ace_msetins1(a, b);
	__kuiloong_ace_msetins2(c, d);
	__kuiloong_ace_msetins3(e, f);
	r = __kuiloong_ace_mgetins3lsb();
	if (r != f) {
		ACENN_DEBUG("Read %x", r);
		ACENN_DEBUG("SAU failed to set %x", f);
		asm("ebreak;");
	}
	ACENN_DEBUG("matrix once ins test PASS!");
}

void minimum_matmul_multi_8x8(int8_t *input_block1, int8_t *input_block2, int8_t *output_block3) {
	ACENN_DEBUG("SAU minimum_matmul_multi_8x8 START!");
    const int H=8, MID=16, W=8;
    static const int8_t A_sa[128] = {
    -115,-114,-114,-113,-112,-111,-111,-110,-109,-108,-107,-107,-106,-105,-104,-104,
	-115,-114,-114,-113,-112,-111,-111,-110,-109,-108,-107,-107,-106,-105,-104,-104,
	-103,-102,-101,-100,-100,-99,-98,-97,-97,-96,-95,-94,-93,-93,-92,-91,
	-103,-102,-101,-100,-100,-99,-98,-97,-97,-96,-95,-94,-93,-93,-92,-91,
	-90,-90,-89,-88,-87,-86,-86,-85,-84,-83,-83,-82,-81,-80,-79,-79,
	-90,-90,-89,-88,-87,-86,-86,-85,-84,-83,-83,-82,-81,-80,-79,-79,
	-78,-77,-76,-76,-75,-74,-73,-72,-72,-71,-70,-69,-69,-68,-67,-66,
	-78,-77,-76,-76,-75,-74,-73,-72,-72,-71,-70,-69,-69,-68,-67,-66 

};
    static const int8_t B_sa[128] = {
        -1,-2,-3,-4,-5,-6,-7,-8,
        -1,-2,-3,-4,-5,-6,-7,-8,
        -1,-2,-3,-4,-5,-6,-7,-8,
        -1,-2,-3,-4,-5,-6,-7,-8,
        -1,-2,-3,-4,-5,-6,-7,-8,
        -1,-2,-3,-4,-5,-6,-7,-8,
        -1,-2,-3,-4,-5,-6,-7,-8,
        -1,-2,-3,-4,-5,-6,-7,-8,

		1,2,3,4,5,6,7,8,
		1,2,3,4,5,6,7,8,
		1,2,3,4,5,6,7,8,
		1,2,3,4,5,6,7,8,
		1,2,3,4,5,6,7,8,
		1,2,3,4,5,6,7,8,
		1,2,3,4,5,6,7,8,
		1,2,3,4,5,6,7,8
    };
    static const int8_t C_ref_int[128] = {
    3,6,9,12,15,18,21,25,
    3,6,9,12,15,18,21,25,
    3,6,9,12,15,18,21,24,
    3,6,9,12,15,18,21,24,
    3,6,9,12,15,18,21,25,
    3,6,9,12,15,18,21,25,
    3,6,9,12,15,18,21,24,
    3,6,9,12,15,18,21,24
    };
    // 写入缓冲
    for(int i=0;i<128;++i) input_block1[i]=A_sa[i];
    for(int i=0;i<128;++i) input_block2[i]=B_sa[i];
    for(int i=0;i<128;++i) output_block3[i]=0;
    // 配置寄存器
    int32_t a,b,c,d,e,f;
    e=1;
    int32_t h_ch=0,h_x=2,v_ch=8,v_x=1,o_ch=1,o_x=1;  //h_ch到底是1还是0？
    int32_t id=0,bias_address=0,flow_loop_times=2;
    int32_t h_addr=(int32_t)(uintptr_t)input_block1;
    int32_t v_addr=(int32_t)(uintptr_t)input_block2;
    int32_t o_addr=(int32_t)(uintptr_t)output_block3;
    int32_t stride_mode=0,transpose_mode=1;
    int32_t cutbit=4, shift_mode=0;
    a=(v_ch<<25)|(v_x<<20)|(v_addr & 0xFFFFF);
    b=(h_ch<<25)|(h_x<<20)|(h_addr & 0xFFFFF);
    c=(o_ch<<25)|(o_x<<20)|(o_addr & 0xFFFFF);
    d=(cutbit<<18)|(shift_mode<<17)|(stride_mode<<16)|(transpose_mode<<14)|(0<<12)|(0<<10)|(0<<8)|(0<<6)|flow_loop_times;
    f=(bias_address<<8)|id;
    __kuiloong_ace_msetins1(b,a);
    __kuiloong_ace_msetins2(d,c);
    __kuiloong_ace_msetins3(f,e);

    bool pass=true;
    for(int i=0;i<128;++i){if(output_block3[i]!=C_ref_int[i]){pass=false;ACENN_DEBUG("Mismatch[%d]: ref=%d sa=%d", i, C_ref_int[i], output_block3[i]);}}
    ACENN_DEBUG(pass?"PASS minimum_matmul_multi":"FAIL minimum_matmul_multi");
    asm("ebreak;");
}


void minimum_matmul_single_8x8(int8_t *input_block1, int8_t *input_block2, int8_t *output_block3) {
	ACENN_DEBUG("SAU minimum_matmul_single START!");
	// 1. 生成测试矩阵 A(16×16), B(16×16)，值域限制在 int8 [-128,127]
    int8_t A[8][8], B[8][8], C_ref[8][8];
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int idx = i * 8 + j;
            A[i][j] = int8_t(idx - 128);
            B[i][j] = int8_t((255 - idx) - 128);  // =127-idx
        }
    }
    // 2. CPU 参考实现 C_ref = A×B
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int32_t sum = 0;
            for (int k = 0; k < 8; k++) {
				sum += A[i][k] * B[k][j];
				// ACENN_DEBUG("A[%d][%d] * B[%d][%d] = %d * %d = %d", i, k, k, j, A[i][k], B[k][j], A[i][k] * B[k][j]);
			}
            C_ref[i][j] = int8_t(sum);
        }
    }

	// 【3】拷贝到硬件缓冲并清零输出
	for (int i = 0; i < 64; i++) {
		input_block1[i] = ((int8_t*)A)[i];
		input_block2[i] = ((int8_t*)B)[i];
		output_block3[i] = 0;
	}

	// 4. 按例程顺序配置 a-f 寄存器字段
    int32_t a, b, c, d, e, f;
    // e = start flag, 0x1 为启动标志
    f = 0x00000001;
    // 通道与步长配置
    int32_t h_ch = 0, h_x = 1;
    int32_t v_ch = 0, v_x = 1;
    int32_t o_ch = 0, o_x = 1;
    int32_t id = 0;
    int32_t bias_address = 0;
    int32_t flow_loop_times = 1;
    // 基址偏移
    int32_t h_addr = (int32_t)(uintptr_t)input_block1;
    int32_t v_addr = (int32_t)(uintptr_t)input_block2;
    int32_t o_addr = (int32_t)(uintptr_t)output_block3;
    // 模式字段
    int32_t stride_mode = 0;
    int32_t transpose_mode = 1;
    int32_t conv_kernel = 0;
    int32_t register_mode = 0;
    int32_t flow_mode = 0;
    int32_t work_mode = 0;

    // 组装 a-f
    a = (h_ch << 25) | (h_x << 20) | h_addr;
    b = (v_ch << 25) | (v_x << 20) | v_addr;
    c = (stride_mode << 16)
      | (transpose_mode << 14)
      | (conv_kernel << 12)
      | (register_mode << 10)
      | (flow_mode << 8)
      | (work_mode << 6)
      | flow_loop_times;
    d = (o_ch << 25) | (o_x << 20) | o_addr;
	e = (bias_address << 8) | id;

    // 5. 下发指令：直接传入 int32_t a–f
    __kuiloong_ace_msetins1(a, b);
    __kuiloong_ace_msetins2(c, d);
    __kuiloong_ace_msetins3(e, f);

	// 6. 等待硬件完成（可加循环查询状态寄存器，此处假设立即完成）

	// 7. 验证结果
	int errors = 0;
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			int8_t hw = output_block3[i*16 + j];
			int8_t sw = C_ref[i][j];
			if (hw != sw) {
				errors++;
				ACENN_DEBUG("Mismatch at [%d][%d]: hw=%d, sw=%d\n", i, j, hw, sw);
			}
		}
	}
	if (errors == 0) {
		ACENN_DEBUG("PASS: Hardware result matches reference.\n");
	} else {
		ACENN_DEBUG("FAIL: %d mismatches found.\n", errors);
	}
	asm("ebreak;"); // 中断，停止运行
}

void minimum_pwconv_single_8x8(int8_t *input_block1, int8_t *input_block2, int8_t *output_block3) {
	ACENN_DEBUG("SAU minimum_pwconv_single_8x8 START!");
    // 1. 直接写入预定义 Active, Weight, Result 数组到 input_block1, input_block2, output_block3
    static const int8_t Active[384] = {
    -115,-114,-114,-113,-112,-111,-111,-110,11,11,11,11,11,11,11,11,
	-115,-114,-114,-113,-112,-111,-111,-110,11,11,11,11,11,11,11,11,
	-103,-102,-101,-100,-100,-99,-98,-97,11,11,11,11,11,11,11,11,
	-103,-102,-101,-100,-100,-99,-98,-97,11,11,11,11,11,11,11,11,
	-90,-90,-89,-88,-87,-86,-86,-85,11,11,11,11,11,11,11,11,
	-90,-90,-89,-88,-87,-86,-86,-85,11,11,11,11,11,11,11,11,
	-78,-77,-76,-76,-75,-74,-73,-72,11,11,11,11,11,11,11,11,
	-78,-77,-76,-76,-75,-74,-73,-72,11,11,11,11,11,11,11,11,

    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,

    -109,-108,-107,-107,-106,-105,-104,-104,11,11,11,11,11,11,11,11,
    -109,-108,-107,-107,-106,-105,-104,-104,11,11,11,11,11,11,11,11,
    -97,-96,-95,-94,-93,-93,-92,-91,11,11,11,11,11,11,11,11,
    -97,-96,-95,-94,-93,-93,-92,-91,11,11,11,11,11,11,11,11,
    -84,-83,-83,-82,-81,-80,-79,-79,11,11,11,11,11,11,11,11,
    -84,-83,-83,-82,-81,-80,-79,-79,11,11,11,11,11,11,11,11,
    -72,-71,-70,-69,-69,-68,-67,-66,11,11,11,11,11,11,11,11,
    -72,-71,-70,-69,-69,-68,-67,-66,11,11,11,11,11,11,11,11
    };
    static const int8_t Weight[128] = {
        -1,-2,-3,-4,-5,-6,-7,-8,
        -1,-2,-3,-4,-5,-6,-7,-8,
        -1,-2,-3,-4,-5,-6,-7,-8,
        -1,-2,-3,-4,-5,-6,-7,-8,
        -1,-2,-3,-4,-5,-6,-7,-8,
        -1,-2,-3,-4,-5,-6,-7,-8,
        -1,-2,-3,-4,-5,-6,-7,-8,
        -1,-2,-3,-4,-5,-6,-7,-8,

		1,2,3,4,5,6,7,8,
		1,2,3,4,5,6,7,8,
		1,2,3,4,5,6,7,8,
		1,2,3,4,5,6,7,8,
		1,2,3,4,5,6,7,8,
		1,2,3,4,5,6,7,8,
		1,2,3,4,5,6,7,8,
		1,2,3,4,5,6,7,8
    };
    static const int8_t Result[64] = {
    1,1,1,1,1,1,1,1,
    3,3,3,3,3,3,3,3,
    4,4,4,4,4,4,4,4,
    6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,8,7,
    9,9,9,9,9,9,9,9,
    10,10,10,10,10,10,11,10,
    12,12,12,12,12,12,13,12
    };
        // 将静态数组写入硬件缓冲
    for (int i = 0; i < 384; ++i) {
        input_block1[i] = Active[i];
    }
    for (int i = 0; i < 128; ++i) {
        input_block2[i] = Weight[i];
        output_block3[i] = 0;
    }
    
    
    // 4. 按例程顺序配置 a-f 寄存器字段
    int32_t a, b, c, d, e, f;
    // e = start flag, 0x1 为启动标志
    f = 0x00000001;
    // 通道与步长配置
    int32_t h_ch = 16, h_x = 2;
    int32_t v_ch = 8, v_x = 1;
    int32_t o_ch = 1, o_x = 1;
    int32_t id = 0;
    int32_t bias_address = 0;
    int32_t flow_loop_times = 2;
    // 基址偏移
    int32_t h_addr = (int32_t)(uintptr_t)input_block1;
    int32_t v_addr = (int32_t)(uintptr_t)input_block2;
    int32_t o_addr = (int32_t)(uintptr_t)output_block3;
    // 模式字段
    int32_t stride_mode = 0;
    int32_t transpose_mode = 0;
    int32_t conv_kernel = 1;
    int32_t register_mode = 0;
    int32_t flow_mode = 1;
    int32_t work_mode = 0;
    int32_t cutbit=5, shift_mode=0;
    // 组装 a-f
    a = (h_ch << 25) | (h_x << 20) | (h_addr & 0xFFFFF);
    b = (v_ch << 25) | (v_x << 20) | (v_addr & 0xFFFFF);
    c = (cutbit<<18)
      | (shift_mode<<17)
      | (stride_mode << 16)
      | (transpose_mode << 14)
      | (conv_kernel << 12)
      | (register_mode << 10)
      | (flow_mode << 8)
      | (work_mode << 6)
      | flow_loop_times;
    d = (o_ch << 25) | (o_x << 20) | (o_addr & 0xFFFFF);
	e = (bias_address << 8) | id;

    // 5. 下发指令：直接传入 int32_t a–f
    __kuiloong_ace_msetins1(a, b);
    __kuiloong_ace_msetins2(c, d);
    __kuiloong_ace_msetins3(e, f);

	// 6. 对比处理结果
	bool pass=true;
    for(int i=0;i<64;++i){
        if(output_block3[i]!=Result[i])
        {pass=false;ACENN_DEBUG("Mismatch[%d]: ref=%d sa=%d", i, Result[i], output_block3[i]);}}
    ACENN_DEBUG(pass?"PASS minimum_pwconv_single":"FAIL minimum_pwconv_single");
}

void minimum_conv_single_8x8(int8_t *input_block1, int8_t *input_block2, int8_t *output_block3, int8_t *input_block4, int8_t stride=1) {
	ACENN_DEBUG("SAU minimum_conv_single_8x8 START!");
    // 1. 直接写入预定义 Active, Weight, Result 数组到 input_block1, input_block2, output_block3
    static const int8_t Active[216] = {
        -115,-114,-114,-113,-112,-111,-111,-110,
		-109,-108,-107,-107,-106,-105,-104,-104,
        88,88,11,11,11,11,11,11,

        -103,-102,-101,-100,-100,-99,-98,-97,
		-97,-96,-95,-94,-93,-93,-92,-91,
        77,77,11,11,11,11,11,11,

        -90,-90,-89,-88,-87,-86,-86,-85,
		-84,-83,-83,-82,-81,-80,-79,-79,
        66,66,11,11,11,11,11,11,


        11,11,11,11,11,11,11,11,
        11,11,11,11,11,11,11,11,
        11,11,11,11,11,11,11,11,

        11,11,11,11,11,11,11,11,
        11,11,11,11,11,11,11,11,
        11,11,11,11,11,11,11,11,

        11,11,11,11,11,11,11,11,
        11,11,11,11,11,11,11,11,
        11,11,11,11,11,11,11,11,

        -88,-88,-114,-113,-112,-111,-111,-110,
		-109,-108,-107,-107,-106,-105,-104,-104,
        88,88,11,11,11,11,11,11,
        -77,-77,-101,-100,-100,-99,-98,-97,
		-97,-96,-95,-94,-93,-93,-92,-91,
        77,77,11,11,11,11,11,11,
        -66,-66,-89,-88,-87,-86,-86,-85,
		-84,-83,-83,-82,-81,-80,-79,-79,
        66,66,11,11,11,11,11,11
    };
    static const int8_t Weight[288] = {
        -1,-2,-3,-4,-5,-6,-7,-8,11,11,11,11,11,11,11,11,
        -1,-2,-3,-4,-5,-6,-7,-8,11,11,11,11,11,11,11,11,
        -1,-2,-3,-4,-5,-6,-7,-8,11,11,11,11,11,11,11,11,
        -1,-2,-3,-4,-5,-6,-7,-8,11,11,11,11,11,11,11,11,
        -1,-2,-3,-4,-5,-6,-7,-8,11,11,11,11,11,11,11,11,
        -1,-2,-3,-4,-5,-6,-7,-8,11,11,11,11,11,11,11,11,
        -1,-2,-3,-4,-5,-6,-7,-8,11,11,11,11,11,11,11,11,
        -1,-2,-3,-4,-5,-6,-7,-8,11,11,11,11,11,11,11,11,
        -1,-2,-3,-4,-5,-6,-7,-8,11,11,11,11,11,11,11,11,
        1,2,3,4,5,6,7,8,11,11,11,11,11,11,11,11,
        1,2,3,4,5,6,7,8,11,11,11,11,11,11,11,11,
        1,2,3,4,5,6,7,8,11,11,11,11,11,11,11,11,
        1,2,3,4,5,6,7,8,11,11,11,11,11,11,11,11,
        1,2,3,4,5,6,7,8,11,11,11,11,11,11,11,11,
        1,2,3,4,5,6,7,8,11,11,11,11,11,11,11,11,
        1,2,3,4,5,6,7,8,11,11,11,11,11,11,11,11,
        1,2,3,4,5,6,7,8,11,11,11,11,11,11,11,11,
        1,2,3,4,5,6,7,8,11,11,11,11,11,11,11,11
    };

    static const int8_t Bias[8] = {
    2,2,2,2,2,2,2,2
    };
    static const int8_t Result[64] = {
    6,4,2,2,2,2,2,2,
    11,6,2,2,2,2,2,2,
    16,9,2,2,2,2,2,2,
    21,11,2,2,2,2,2,2,
    25,13,2,2,2,2,2,2,
    30,16,2,2,2,2,2,2,
    35,18,2,2,2,2,2,2,
    40,20,2,2,2,2,2,2
    };
    static const int8_t Result_stride[64] = {
    6,2,2,2,2,2,2,2,
    11,2,2,2,2,2,2,2,
    16,2,2,2,2,2,2,2,
    21,2,2,2,2,2,2,2,
    25,2,2,2,2,2,2,2,
    30,2,2,2,2,2,2,2,
    35,2,2,2,2,2,2,2,
    40,2,2,2,2,2,2,2
    };


        // 将静态数组写入硬件缓冲
    for (int i = 0; i < 216; ++i) {
        input_block1[i] = Active[i];
    }
    for (int j = 0; j < 288; ++j) {
        input_block2[j] = Weight[j];
        output_block3[j] = 0;
    }
    for (int k = 0; k < 8; ++k) {
        input_block4[k] = Bias[k];
    }

    // 4. 按例程顺序配置 a-f 寄存器字段
    int32_t a, b, c, d, e, f;
    // e = start flag, 0x1 为启动标志
    f = 0x00000001;
    // 通道与步长配置
    int32_t h_ch = 6, h_x = 3;
    int32_t v_ch = 9, v_x = 2;
    int32_t o_ch = 1, o_x = 1;
    int32_t id = 0;
    int32_t flow_loop_times = 2;
    // 基址偏移
    int32_t h_addr = (int32_t)(uintptr_t)input_block1;
    int32_t v_addr = (int32_t)(uintptr_t)input_block2;
    int32_t o_addr = (int32_t)(uintptr_t)output_block3;
    int32_t b_addr = (int32_t)(uintptr_t)input_block4;
    // 模式字段
    int32_t stride_mode = stride;
    int32_t transpose_mode = 0;
    int32_t conv_kernel = 3;
    int32_t register_mode = 0;
    int32_t flow_mode = 1;
    int32_t work_mode = 2;
    int32_t cutbit=5, shift_mode=0;
    // 组装 a-f
    a = (h_ch << 25) | (h_x << 20) | (h_addr & 0xFFFFF);
    b = (v_ch << 25) | (v_x << 20) | (v_addr & 0xFFFFF);
    c = (cutbit<<18)
      | (shift_mode<<17)
      | (stride_mode << 16)
      | (transpose_mode << 14)
      | (conv_kernel << 12)
      | (register_mode << 10)
      | (flow_mode << 8)
      | (work_mode << 6)
      | flow_loop_times;
    d = (o_ch << 25) | (o_x << 20) | (o_addr & 0xFFFFF);
	e = ((b_addr & 0xFFFFF) << 8) | id;

    // 5. 下发指令：直接传入 int32_t a–f
    __kuiloong_ace_msetins1(a, b);
    __kuiloong_ace_msetins2(c, d);
    __kuiloong_ace_msetins3(e, f);

	bool pass=true;
    if(stride_mode==0){
        for(int i=0;i<64;++i){if(output_block3[i]!=Result[i]){pass=false;ACENN_DEBUG("Mismatch[%d]: ref=%d sa=%d", i, Result[i], output_block3[i]);}}
    }else{
        for(int i=0;i<64;++i){if(output_block3[i]!=Result_stride[i]){pass=false;ACENN_DEBUG("Mismatch[%d]: ref=%d sa=%d", i, Result_stride[i], output_block3[i]);}}
    }
    ACENN_DEBUG(pass?"PASS minimum_conv_single_8x8":"FAIL minimum_conv_single_8x8");

}

void minimum_dwconv_single_8x8(int8_t *input_block1, int8_t *input_block2, int8_t *output_block3, int8_t *input_block4, int8_t stride=1) {
	ACENN_DEBUG("SAU minimum_dwconv_single_8x8 START!");
    // 1. 直接写入预定义 Active, Weight, Result 数组到 input_block1, input_block2, output_block3
    static const int8_t Active[432] = {
    //ROW0
        -115,-114,-114,-113,-112,-111,-111,-110,-109,-108,-107,-107,-106,-105,-104,-104,
        88,11,11,11,11,11,11,11,
    //ROW1
        -103,-102,-101,-100,-100,-99,-98,-97,-97,-96,-95,-94,-93,-93,-92,-91,
        77,11,11,11,11,11,11,11,
    //ROW2
        -90,-90,-89,-88,-87,-86,-86,-85,-84,-83,-83,-82,-81,-80,-79,-79,
        66,11,11,11,11,11,11,11,
    //ROW3
        -78,-77,-76,-76,-75,-74,-73,-72,-72,-71,-70,-69,-69,-68,-67,-66,
        88,11,11,11,11,11,11,11,
    //ROW4
        -65,-64,-63,-62,-61,-60,-59,-58,-57,-56,-55,-54,-53,-52,-51,-50,
        77,11,11,11,11,11,11,11,
    //ROW5
        -49,-48,-47,-46,-45,-44,-43,-42,-41,-40,-39,-38,-37,-36,-35,-34,
        66,11,11,11,11,11,11,11,
    //ROW6
        -33,-32,-31,-30,-29,-28,-27,-26,-25,-24,-23,-22,-21,-20,-19,-18,
        88,11,11,11,11,11,11,11,
    //ROW7
        -17,-16,-15,-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,
        77,11,11,11,11,11,11,11,
    //ROW8
        -3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,
        66,11,11,11,11,11,11,11,
    //ROW9
        13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,
        88,11,11,11,11,11,11,11,
    //ROW10
        29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,
        77,11,11,11,11,11,11,11,
    //ROW11
        45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,
        66,11,11,11,11,11,11,11,
    //ROW12
        61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,
        88,11,11,11,11,11,11,11,
    //ROW13
        77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,
        77,11,11,11,11,11,11,11,
    //ROW14
        93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,
        66,11,11,11,11,11,11,11,
    //ROW15
        109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,
        88,11,11,11,11,11,11,11,
    //ROW16
        -115,-114,-114,-113,-112,-111,-111,-110,-109,-108,-107,-107,-106,-105,-104,-104,
        77,11,11,11,11,11,11,11,
    //ROW17
        -103,-102,-101,-100,-100,-99,-98,-97,-97,-96,-95,-94,-93,-93,-92,-91,
        66,11,11,11,11,11,11,11
    };
    static const int8_t Weight[72] = {
        -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1
    };

    static const int8_t Bias[8] = {
    2,2,2,2,2,2,2,2,
    };
    static const int8_t Result[64] = {
    16,16,16,16,15,15,15,15,
    14,14,14,14,14,14,13,13,
    12,12,12,12,12,12,12,11,
    10,10,10,10,10,10,10,9,
    8,8,8,8,8,8,7,7,
    6,6,6,6,5,5,5,5,
    4,4,4,3,3,3,3,3,
    2,2,1,1,1,1,1,1
    };
    static const int8_t Result_stride[64] = {
    16,16,15,15,15,15,15,6,
    12,12,12,12,11,11,11,4,
    8,8,8,7,7,7,7,1,
    4,4,3,3,3,2,2,-2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2
    };


        // 将静态数组写入硬件缓冲
    for (int i = 0; i < 432; ++i) {
        input_block1[i] = Active[i];
    }
    for (int j = 0; j < 72; ++j) {
        input_block2[j] = Weight[j];
        output_block3[j] = 0;
    }
    for (int k = 0; k < 8; ++k) {
        input_block4[k] = Bias[k];
    }

    // 4. 按例程顺序配置 a-f 寄存器字段
    int32_t a, b, c, d, e, f;
    // e = start flag, 0x1 为启动标志
    f = 0x00000001;
    // 通道与步长配置
    int32_t h_ch = 3, h_x = 3;
    int32_t v_ch = 0, v_x = 1;
    int32_t o_ch = 1, o_x = 1;
    int32_t id = 0;
    int32_t flow_loop_times = 8/(stride+1);
    // 基址偏移
    int32_t h_addr = (int32_t)(uintptr_t)input_block1;
    int32_t v_addr = (int32_t)(uintptr_t)input_block2;
    int32_t o_addr = (int32_t)(uintptr_t)output_block3;
    int32_t b_addr = (int32_t)(uintptr_t)input_block4;
    // 模式字段
    int32_t stride_mode = stride;
    int32_t transpose_mode= 0;
    int32_t conv_kernel = 3;
    int32_t register_mode = 2;
    int32_t flow_mode = 1;
    int32_t work_mode = 2;
    int32_t cutbit=6, shift_mode=0;
    // 组装 a-f
    a = (h_ch << 25) | (h_x << 20) | (h_addr & 0xFFFFF);
    b = (v_ch << 25) | (v_x << 20) | (v_addr & 0xFFFFF);
    c = (cutbit<<18)
      | (shift_mode<<17)
      | (stride_mode << 16)
      | (transpose_mode << 14)
      | (conv_kernel << 12)
      | (register_mode << 10)
      | (flow_mode << 8)
      | (work_mode << 6)
      | flow_loop_times;
    d = (o_ch << 25) | (o_x << 20) | (o_addr & 0xFFFFF);
	e = ((b_addr & 0xFFFFF) << 8) | id;

    // 5. 下发指令：直接传入 int32_t a–f
    __kuiloong_ace_msetins1(a, b);
    __kuiloong_ace_msetins2(c, d);
    __kuiloong_ace_msetins3(e, f);

	bool pass=true;
    if(stride_mode==0){
        for(int i=0;i<64;++i){if(output_block3[i]!=Result[i]){pass=false;ACENN_DEBUG("Mismatch[%d]: ref=%d sa=%d", i, Result[i], output_block3[i]);}}
    }else{
        for(int i=0;i<64;++i){if(output_block3[i]!=Result_stride[i]){pass=false;ACENN_DEBUG("Mismatch[%d]: ref=%d sa=%d", i, Result_stride[i], output_block3[i]);}}
    }
    ACENN_DEBUG(pass?"PASS minimum_dwconv_single_8x8":"FAIL minimum_dwconv_single_8x8");

}
void sau_benchmark_8x8(int8_t *input_block1, int8_t *input_block2, int8_t *output_block3, int8_t *input_block4)
{
	ACENN_DEBUG("SAU BENCHMARK START!");


	minimum_matmul_multi_8x8(input_block1, input_block2, output_block3);

	minimum_pwconv_single_8x8(input_block1, input_block2, output_block3);

    minimum_conv_single_8x8(input_block1, input_block2, output_block3, input_block4, 0);
    minimum_conv_single_8x8(input_block1, input_block2, output_block3, input_block4, 1);

    minimum_dwconv_single_8x8(input_block1, input_block2, output_block3, input_block4, 0);
    minimum_dwconv_single_8x8(input_block1, input_block2, output_block3, input_block4, 1);
	ACENN_DEBUG("SAU BENCHMARK PASS!");
}

