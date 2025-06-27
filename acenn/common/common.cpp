//////////////////////////////////////////////////////////////////////////////////
// Author:          yintao liu - berialest@shu.edu.cn                           //
//                                                                              //
// Additional contributions by:                                                 //
//                                                                              //
// File Name:       common.cpp                                                  //
// Project Name:    ACENN                                                       //
// Language:        cpp                                                         //
//                                                                              //
// Description:     Common structs, miaros, functions, etc.                     //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#ifndef __COMMON_CPP__
#define __COMMON_CPP__

#include "type_traits"
#include "stdint.h"
#include "stdlib/malloc.hpp"
#include "stdlib/printf.hpp"
#include "common/common.hpp"

template<typename T>
struct acenn_matrix {
    uint32_t dims;
    uint32_t channels;
    uint32_t rows;
    uint32_t cols;
    T* matrix;
};

//  define enmu of data types
enum class TensorType {
    I8_TYPE,
    I16_TYPE,
    I32_TYPE,
    FLOAT_TYPE
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : create_tensor
// Description:
//      This is a template function designed to allocate memory and create a tensor.
//      A tensor here is conceptually a multi-dimensional data structure (akin to a
//      matrix in higher dimensions), and the specific data type of its elements is
//      determined by the template parameter 'T'. The function takes in parameters
//      that define the dimensions of the tensor, namely 'dims' (number of overall
//      dimensions), 'channels' (e.g., color channels in image-like tensors), 'rows',
//      and 'cols' (which are relevant for the 2D slices within the tensor). Based on
//      these dimensions and the size of the data type 'T', it calculates the required
//      memory size and allocates that memory block. It's a flexible way to create
//      tensors of different types and dimensions for various applications in fields
//      like deep learning or numerical computing where handling multi-dimensional
//      data is common.
//
// Parameters:
//      uint32_t dims:         This parameter specifies the total number of dimensions of
//                             the tensor to be created. 
//                             It is crucial for determining the overall size of the tensor
//                             during memory allocation.
//      uint32_t channels:     In the context of tensors that might represent images or
//                             other data with channel-based information, this parameter
//                             indicates the number of channels. For example, in an RGB
//                             image tensor, 'channels' would be 3. It contributes to the
//      uint32_t rows:         This parameter defines the number of rows within the tensor,
//                             particularly relevant when considering the 2D slices or sub-
//                             matrices that make up the tensor. It is one of the factors
//                             used in calculating the total memory footprint of the tensor.
//      uint32_t cols:         Similar to 'rows', it represents the number of columns in the
//                             2D slices of the tensor. Together with 'rows', 'channels', and
//                             'dims', it helps in precisely determining the amount of memory
//                             that needs to be allocated for the tensor.
//
// Return:
//      T*:                    A pointer to the allocated memory block that represents the
//                             newly created tensor. If the memory allocation is successful,
//                             this pointer can be used to access and manipulate the data
//                             within the tensor. In case of any issues during allocation
//                             (such as insufficient memory), the pointer might be a null
//                             pointer, indicating that the creation of the tensor failed.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
inline T* create_tensor(uint32_t dims, uint32_t channels, uint32_t rows, uint32_t cols) {
    T* matrix = nullptr;
    if constexpr (std::is_same<T, uint8_t>::value) {
        matrix = reinterpret_cast<T*>(malloc((dims * channels * rows * cols) * sizeof(uint8_t)));
    }
    else if constexpr (std::is_same<T, int8_t>::value) {
        matrix = reinterpret_cast<T*>(malloc((dims * channels * rows * cols) * sizeof(int8_t)));
    }
    else if constexpr (std::is_same<T, uint16_t>::value) {
        matrix = reinterpret_cast<T*>(malloc((dims * channels * rows * cols) * sizeof(uint16_t)));
    }
    else if constexpr (std::is_same<T, int16_t>::value) {
        matrix = reinterpret_cast<T*>(malloc((dims * channels * rows * cols) * sizeof(int16_t)));
    }
    else if constexpr (std::is_same<T, uint32_t>::value) {
        matrix = reinterpret_cast<T*>(malloc((dims * channels * rows * cols) * sizeof(uint32_t)));
    }
    else if constexpr (std::is_same<T, int32_t>::value) {
        matrix = reinterpret_cast<T*>(malloc((dims * channels * rows * cols) * sizeof(int32_t)));
    }
    else if constexpr (std::is_same<T, float>::value) {
        matrix = reinterpret_cast<T*>(malloc((dims * channels * rows * cols) * sizeof(float)));
    }
    else {
        ACENN_DEBUG("Unsupported data type in create_tensor function!");
    }
    return matrix;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : free_tensor
// Description:
//      This is a template function that is used to free the memory allocated for a tensor.
//      The tensor is represented by a pointer 'matrix' of type 'T*'. It makes use of the
//      standard C library function 'free' to release the memory that was previously
//      allocated for the tensor. This helps in avoiding memory leaks by properly deallocating
//      the memory when it is no longer needed. The function is declared as 'inline' which
//      suggests to the compiler that it should be expanded in-place when called, potentially
//      improving performance by reducing the overhead of a function call in some cases.
//
// Parameters:
//      T* matrix:             Pointer to the memory block representing the tensor that needs to
//                             be deallocated. This pointer should point to the memory that was
//                             previously allocated, typically using functions like 'create_tensor'
//                             or similar memory allocation mechanisms for tensors.
//
// Return:
//      void:                  This function does not return a value. Its purpose is solely to free
//                             the memory pointed to by the 'matrix' pointer.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
inline void free_tensor(T* matrix) {
    free(matrix);
}


#endif