// kuiloong.hpp
#ifndef KUILOONG_HPP
#define KUILOONG_HPP

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif


// 声明main_graph函数
// 参数：输入矩阵指针，输出矩阵指针，其他维度参数
int8_t* main_graph(
    int8_t* model_concatenate_concat
);


#ifdef __cplusplus
}
#endif

#endif // KUILOONG_HPP