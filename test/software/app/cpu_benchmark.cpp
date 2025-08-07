# include "cpu_benchmark.hpp"


void test_component(){
	ACENN_DEBUG("-------- hello kuiloong! ----------");
	test_printf_();
	test_malloc();
	test_malloc_boundary();
	ACENN_DEBUG("-------- test component over! ----------");
}


void test_gemm_proc() {
    ACENN_DEBUG("test_cpu_proc START!");
    
    // 1. 创建 acenn_matrix 结构体
    acenn_matrix<int8_t> *matrix_a = static_cast<acenn_matrix<int8_t>*>(malloc(sizeof(acenn_matrix<int8_t>)));
    acenn_matrix<int8_t> *matrix_b = static_cast<acenn_matrix<int8_t>*>(malloc(sizeof(acenn_matrix<int8_t>)));
    
    if (matrix_a == nullptr || matrix_b == nullptr) {
        ACENN_DEBUG("ERROR: Failed to allocate matrix structures!");
        asm("ebreak;");
        return;
    }
    ACENN_DEBUG("create acenn_matrix struct!");

    
    // 2. 初始化矩阵结构体 - 修改为16x16
    matrix_a->dims = 1;
    matrix_a->channels = 1;
    matrix_a->rows = 16;
    matrix_a->cols = 16;
    matrix_a->matrix = create_tensor<int8_t>(1, 1, 16, 16);
    
    matrix_b->dims = 1;
    matrix_b->channels = 1;
    matrix_b->rows = 16;
    matrix_b->cols = 16;
    matrix_b->matrix = create_tensor<int8_t>(1, 1, 16, 16);
    
    if (matrix_a->matrix == nullptr || matrix_b->matrix == nullptr) {
        ACENN_DEBUG("ERROR: Failed to allocate matrix data!");
        free(matrix_a);
        free(matrix_b);
        asm("ebreak;");
        return;
    }
    ACENN_DEBUG("create acenn_matrix tensor!");

    
    // 3. 生成测试数据 - 修改为16x16
    int8_t C_ref[16][16];
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            int idx = i * 16 + j;
            matrix_a->matrix[i * 16 + j] = int8_t(idx % 256 - 128);
            matrix_b->matrix[i * 16 + j] = int8_t((255 - (idx % 256)) - 128);
        }
    }
    ACENN_DEBUG("change acenn_matrix tensor!");

    
    // 4. CPU 参考实现 C_ref = A×B - 修改为16x16
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            int32_t sum = 0;
            for (int k = 0; k < 16; k++) {
                sum += matrix_a->matrix[i * 16 + k] * matrix_b->matrix[k * 16 + j];
            }
            C_ref[i][j] = int8_t(sum);
        }
    }
    ACENN_DEBUG("get reference tensor!");

    
    // 5. 调用模板函数进行矩阵乘法
    uint8_t shift_fig = 0;  // 根据需要设置位移值
    acenn_matrix<int8_t> *result = matrix_multiply<int8_t>(matrix_a, matrix_b, shift_fig);
    
    if (result == nullptr) {
        ACENN_DEBUG("ERROR: Matrix multiplication failed!");
        free(matrix_a->matrix);
        free(matrix_a);
        free(matrix_b->matrix);
        free(matrix_b);
        asm("ebreak;");
        return;
    }
    ACENN_DEBUG("get output tensor!");

    
    // 6. 验证结果
    int errors = 0;
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            int8_t hw = result->matrix[i * 16 + j];
            int8_t sw = C_ref[i][j];
            if (hw != sw) {
                errors++;
                ACENN_DEBUG("Mismatch at [%d][%d]: hw=%d, sw=%d", i, j, hw, sw);
            }
        }
    }
    ACENN_DEBUG("difference output tensor!");
	
    
    if (errors == 0) {
        ACENN_DEBUG("PASS: Hardware result matches reference.");
    } else {
        ACENN_DEBUG("FAIL: %d mismatches found.", errors);
    }
    
    // 7. 清理内存
    free(matrix_a->matrix);
    free(matrix_a);
    free(matrix_b->matrix);
    free(matrix_b);
    free(result->matrix);
    free(result);
    ACENN_DEBUG("free acenn_matrix tensor!");

}

void test_fc_proc() {
    ACENN_DEBUG("test_fc_proc START!");
    
    // 1. 创建 acenn_matrix 结构体 (输入、权重、偏置、输出)
    acenn_matrix<int8_t> *input = static_cast<acenn_matrix<int8_t>*>(malloc(sizeof(acenn_matrix<int8_t>)));
    acenn_matrix<int8_t> *weight = static_cast<acenn_matrix<int8_t>*>(malloc(sizeof(acenn_matrix<int8_t>)));
    acenn_matrix<int8_t> *bias = static_cast<acenn_matrix<int8_t>*>(malloc(sizeof(acenn_matrix<int8_t>)));
    
    if (input == nullptr || weight == nullptr || bias == nullptr) {
        ACENN_DEBUG("ERROR: Failed to allocate matrix structures!");
        asm("ebreak;");
        return;
    }
    ACENN_DEBUG("create fc acenn_matrix struct!");

    
    // 2. 初始化矩阵结构体 - 输入16x1, 权重8x16, 偏置8x1
    input->dims = 1;
    input->channels = 1;
    input->rows = 16;
    input->cols = 1;
    input->matrix = create_tensor<int8_t>(1, 1, 16, 1);
    
    weight->dims = 1;
    weight->channels = 1;
    weight->rows = 8;
    weight->cols = 16;
    weight->matrix = create_tensor<int8_t>(1, 1, 8, 16);
    
    bias->dims = 1;
    bias->channels = 1;
    bias->rows = 8;
    bias->cols = 1;
    bias->matrix = create_tensor<int8_t>(1, 1, 8, 1);
    
    if (input->matrix == nullptr || weight->matrix == nullptr || bias->matrix == nullptr) {
        ACENN_DEBUG("ERROR: Failed to allocate matrix data!");
        free(input);
        free(weight);
        free(bias);
        asm("ebreak;");
        return;
    }
    ACENN_DEBUG("create fc acenn_matrix tensor!");

    
    // 3. 生成测试数据
    int8_t output_ref[8];
    
    // 初始化输入数据 (16x1)
    for (int i = 0; i < 16; i++) {
        input->matrix[i] = int8_t(i % 32 - 16);
    }
    
    // 初始化权重数据 (8x16)
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 16; j++) {
            weight->matrix[i * 16 + j] = int8_t((i + j) % 64 - 32);
        }
    }
    
    // 初始化偏置数据 (8x1)
    for (int i = 0; i < 8; i++) {
        bias->matrix[i] = int8_t(i % 16 - 8);
    }
    ACENN_DEBUG("change fc acenn_matrix tensor!");

    
    // 4. CPU 参考实现 output = weight × input + bias
    for (int i = 0; i < 8; i++) {
        int32_t sum = 0;
        for (int j = 0; j < 16; j++) {
            sum += weight->matrix[i * 16 + j] * input->matrix[j];
        }
        sum += bias->matrix[i];
        output_ref[i] = int8_t(sum);
    }
    ACENN_DEBUG("get fc reference tensor!");

    
    // 5. 调用全连接层处理函数
    uint8_t shift_fig = 0;  // 根据需要设置位移值
    acenn_matrix<int8_t> *result = matrix_multiply<int8_t>(weight, input, shift_fig);
    
    if (result == nullptr) {
        ACENN_DEBUG("ERROR: FC matrix multiplication failed!");
        free(input->matrix);
        free(input);
        free(weight->matrix);
        free(weight);
        free(bias->matrix);
        free(bias);
        asm("ebreak;");
        return;
    }
    
    // 添加偏置
    for (int i = 0; i < 8; i++) {
        result->matrix[i] += bias->matrix[i];
    }
    ACENN_DEBUG("get fc output tensor!");

    
    // 6. 验证结果
    int errors = 0;
    for (int i = 0; i < 8; i++) {
        int8_t hw = result->matrix[i];
        int8_t sw = output_ref[i];
        if (hw != sw) {
            errors++;
            ACENN_DEBUG("FC Mismatch at [%d]: hw=%d, sw=%d", i, hw, sw);
        }
    }
    ACENN_DEBUG("difference fc output tensor!");
	
    
    if (errors == 0) {
        ACENN_DEBUG("FC PASS: Hardware result matches reference.");
    } else {
        ACENN_DEBUG("FC FAIL: %d mismatches found.", errors);
    }
    
    // 7. 清理内存
    free(input->matrix);
    free(input);
    free(weight->matrix);
    free(weight);
    free(bias->matrix);
    free(bias);
    free(result->matrix);
    free(result);
    ACENN_DEBUG("free fc acenn_matrix tensor!");
}