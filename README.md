# kuilongNN

> 一句话描述：面向“夔龙”（Kuiloong）RISC-V NPU 的神经网络算子库（acenn）、裸机测试程序与 Python/C 周期级仿真器。

## 项目是什么

kuilongNN 是给一款自研低功耗 NPU（代码中称“夔龙 / kuiloong”，CPU 核名为 Spirit）配套的软件栈。它的核心是 `acenn` 算子库：在 RV32IMC 上加了自定义扩展 `xcvnn`（`-march=rv32imc_xcvnn`），用 CSR 指令控制两个加速单元：

- **VEU**：向量计算单元（向量加/乘、ReLU、Softmax、ReduceSum 等）
- **SAU**：2D 脉动/矩阵计算单元（GEMM、卷积、全连接等，支持 8bit 和 16bit）

仓库里还有裸机测试/基准程序，以及一个用 Python 串起 C 模块（SWIG / C 扩展）的软件仿真器，在没有硬件时也能跑固件。

## 主要功能

- **acenn 算子库**（头文件为主，C++）
  - `matrix/`：conv、fc、gemm、matmul（调用 SAU）
  - `vector/`：vecadd、vecmul、relu、leakyrelu、softmax、concat、gather、vecreducesum（调用 VEU）
  - `scalar/`：同类算子的纯 CPU 标量实现（标准卷积/深度可分离/分组/逐点卷积、Gemm、Softmax、ReLU 等），可作参考或对照
  - `stdlib/`：裸机运行时（printf、UART、malloc，内存分配有 buddy / first-fit / slab 三种）
  - `common/xcvnn.hpp`：xcvnn 扩展指令与 VEU/SAU CSR 地址定义
- **测试与基准**（`test/software/app`）：CPU、VEU、SAU（8b/16b、8x8）基准，用 `rdcycle`/`rdinstret` 统计周期和指令数；Kconfig/menuconfig 选择测试用例
- **固件构建脚本**：ELF → bin → `instruction.hex` / `memory.hex`，也能生成 mif / coe（FPGA 用）及 mod4 拆分 hex
- **仿真器**（`test/sim/simulator`）：`main_kui_8.py` 把 Spirit CPU、UART、Device、SRAM、VEU、SAU 模块连起来，按周期加载 hex 跑固件，日志写到 `log/`
- `sau_ins.md`：SAU 8bit×8bit 算子指令表文档

## 目录结构

```
kuilongNN/
├── sau_ins.md                 # SAU 算子指令表
├── acenn/                     # 算子库
│   ├── common/                # xcvnn 指令/CSR 定义、公共头
│   ├── matrix/                # SAU 矩阵类算子
│   ├── vector/                # VEU 向量类算子
│   ├── scalar/                # 纯 CPU 标量算子
│   └── stdlib/                # printf/uart/malloc 等裸机库
└── test/
    ├── software/
    │   ├── app/               # 基准/测试固件（Makefile、Kconfig、benchmark/、scripts/）
    │   └── runtime/           # kuiloongNN 运行时示例（main.cpp、kuiloong.hpp、ELF 分析脚本）
    ├── sim/simulator/         # Python + C 仿真器（src/{Spirit,Device,UART,SRAM,SAU,VEU}、tools/）
    └── testcase/              # 测试用例数据与上传脚本
```

## 怎么使用

### 依赖
- 支持 `xcvnn` 扩展的 RISC-V 工具链：`riscv32-unknown-linux-gnu-gcc/objcopy/objdump`（标准 GCC 不认识 `rv32imc_xcvnn`，需要配套的定制工具链，仓库里没有附带）
- Python 3（仿真器的预编译模块是 `cpython-310`，即 Python 3.10）、`swig`、gcc/g++
- 可选：`menuconfig`（kconfiglib）

### 1. 编译测试固件
```bash
cd test/software/app
make menu          # 可选：选择测试用例（写入 .config）
make genhpp        # 可选：把 testcase 数据转成头文件
make all -B        # 编译出 build/firmware.elf 及 instruction.hex / memory.hex
make mod4          # 生成 mod4 拆分 hex
# 或一步：./my_testcase.sh <用例名>   # 编译并把 hex/txt 复制到 test/testcase/<用例名>
make move          # 把 hex 复制到仿真器的 firmware/gcc_backend/
```
要跑哪个基准，在 `main.cpp` 里切换 `#include "benchmark/..."`。

### 2. 运行仿真
```bash
cd test/sim/simulator
# 先改 Makefile 里的 PYTHON / PYTHONH 路径（目前写死为作者本机的 conda 环境）
make build         # SWIG 编译 Spirit / Device / UART 模块
make all           # 运行 main_kui_8.py，输出同时写到 log/record.log
make clean
```
macOS 下用 `make IS_MACOS=1 ...`（会用 gcc-11）。仿真器从 `./firmware/gcc_backend/instruction.hex` 和 `memory.hex` 读取固件。

## 待确认 / 注意事项
- 仓库没有 README、许可证和依赖清单；定制工具链从哪里获取、对应的硬件 RTL 在哪，代码里都没说明。
- 多处路径写死在作者环境（`/home/zbn/...`、`env.sh` 里的 `source /home/ic/env.sh`、`upload_sftp.sh` 里的内网服务器地址），换机器需要自己改。
- 仓库里提交了编译产物（`.so`、`.o`），而且是平台相关的。
- 代码作者署名 yintao liu（shu.edu.cn），推测是上海大学的科研项目，未经核实。
