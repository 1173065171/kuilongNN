// kuiloong.hpp
#ifndef KUILOONG_HPP
#define KUILOONG_HPP

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

// 声明全局变量（从Python文件中提取的常量）
extern int8_t KuiGemm_model_dense_MatMul_model_dense_Relu_model_dense_BiasAdd_Gemm__142_const_fold_opt__359_quant_1[16];
extern int8_t KuiGemm_model_dense_MatMul_model_dense_Relu_model_dense_BiasAdd_Gemm__142_model_dense_BiasAdd_ReadVariableOp_scaled_round_1[64];
extern int8_t KuiGemm_model_dense_MatMul_model_dense_Relu_model_dense_BiasAdd_Gemm__142_model_dense_MatMul_model_dense_Relu_model_dense_BiasAdd_Gemm__142_scale_1[1];
extern int8_t KuiGemm_model_dense_MatMul_model_dense_Relu_model_dense_BiasAdd_Gemm__142_clip_min_1[1];
extern int8_t KuiGemm_model_dense_MatMul_model_dense_Relu_model_dense_BiasAdd_Gemm__142_clip_max_1[1];

// 声明main_graph函数
// 参数：输入矩阵指针，输出矩阵指针，其他维度参数
int8_t* main_graph(
    int8_t* model_concatenate_concat,  // 输入数据
    int8_t* const_fold_opt__359_quant  // 第二个参数（在Python中定义但未使用）
);


#ifdef __cplusplus
}
#endif

#endif // KUILOONG_HPP