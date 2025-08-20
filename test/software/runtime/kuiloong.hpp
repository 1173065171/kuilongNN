// kuiloong.hpp
#ifndef KUILOONG_HPP
#define KUILOONG_HPP

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

// 声明main_graph函数
// 输入0: model_concatenate_concat
//   原始维度: 8 x 396 (大小: 3168)
//   对齐维度: 8 x 400 (大小: 3200)
// 输出数量: 1
// 输出1: Relu__58_0
//   原始维度: 8 x 64 (大小: 512)
//   对齐维度: 8 x 64 (大小: 512)
int8_t* main_graph(
    int8_t* input_0
);

#ifdef __cplusplus
}
#endif

#endif // KUILOONG_HPP