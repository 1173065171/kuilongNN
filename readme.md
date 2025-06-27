# ACENN算子开发

本工程为kuiloong计算平台的计算库测试工程，欢迎大家的驻足并提供指导性意见。

## 工程结构

- acenn
算子库工程
- test
测试工程
	- sim/simulator
	硬件仿真器
	- software/app
	算子测试开发

## 模板样例

test/software/app/templete.cpp\
test/software/app/sau_benchmark.cpp\
test/software/app/veu_benchmark.cpp

## 开发流程
- 1、编译仿真器
	
	通过swig、python编译运行库：
	终端cd到` test/sim/simulator/`目录下，执行	`make build`

- 2、编写`test/software/app/main.cpp`程序
	
	实现内容包括数据加载、算子运行、数据正确性验证

- 3、编译`test/software/app/main.cpp`程序

	通过kuiloong工具链编译算子程序
	终端cd到`test/software/app/`目录下，执行`make sim`
	这一步会将程序编译成仿真器可以运行的可执行文件，并移动到`test/sim/simulator/firmware/gcc_backend/`下

- 4、运行算子程序

	终端在` test/sim/simulator/`目录下，运行`make all`
	执行的打印log会出现在终端或者`gcc_backend.log`文件中

## 注解
- 此版本为8bit计算版本，所有的输入和输出数据均为8bit版本
- `test/testcase/`目录下提供了部分测试数据，以及对应的算子信息，数据也是8bit，两字符的存储情况，
- 可以通过AI寻求帮助，将自己生成或提供的测试数据转换成` test/software/app/matrixA.hpp`的格式，直接以C++数组的情况存储在全局堆中，在程序里调用。
- 小人力成本下的验证工程，如遇到问题，请移步群里提问，或私聊，本仓库会在培训过程中以及后续长期更新。
- 非常期待您的算法部署出现在我们的芯片系统中！