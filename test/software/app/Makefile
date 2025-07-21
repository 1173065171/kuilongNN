CC=riscv32-unknown-linux-gnu-gcc
OBJCOPY=riscv32-unknown-linux-gnu-objcopy
OBJDUMP=riscv32-unknown-linux-gnu-objdump
PYTHON=python3
ABI=ilp32
ARCH=rv32imc_xcvnn
CFLAGS=-c -MD -MF $(BUILD_DIR)/acenn.d -Os  -ffreestanding -nostdlib -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/stdlib
LDFLAGS=-MD -Os -ffreestanding -nostdlib -Wl,-Bstatic,-T,$(SCRIPT_DIR)/firmware.lds,-Map,$(BUILD_DIR)/$(patsubst %.elf,%.map,$(TARGET)),--strip-debug \
		-Xlinker -verbose
TARGET=firmware.elf
C_FILE=$(wildcard *.cpp)
C_FILE+=$(wildcard $(INCLUDE_DIR)/common/*.cpp)
C_FILE+=$(wildcard $(INCLUDE_DIR)/stdlib/*.cpp)
ASM_FILE=$(realpath ./start.S)
LINK_FILE=firmware.lds
INCLUDE_DIR=$(realpath ../../../acenn)
SCRIPT_DIR=$(realpath ./scripts)
BUILD_DIR=$(realpath ./build)
TESTCASE_DIR=$(realpath ../../testcase)
TESTCASE_CHOSEN := $(shell grep '^CONFIG_TESTCASE_CHOSEN=' .config | sed 's/^CONFIG_TESTCASE_CHOSEN="\(.*\)"$$/\1/')

# 确保 build 文件夹存在
build:
	mkdir -p ./build

# 修改规则，将 .o 文件生成到 build 文件夹下且文件名不带路径前缀
%.o: %.cpp | $(BUILD_DIR)
	$(CC) -mabi=$(ABI) -march=$(ARCH) $(CFLAGS) $< -o $@

%.o: %.S | $(BUILD_DIR)
	$(CC) -mabi=$(ABI) -march=$(ARCH) $(CFLAGS) $< -o $@

# 静态链接库只能加在后面
all: build $(patsubst %.cpp,%.o,$(C_FILE)) $(patsubst %.S,%.o,$(ASM_FILE))
	$(CC) -mabi=$(ABI) -march=$(ARCH) $(LDFLAGS) -o $(BUILD_DIR)/$(TARGET) $(patsubst %.cpp,%.o,$(C_FILE)) $(patsubst %.S,%.o,$(ASM_FILE)) -lm -lc -lgcc
	$(OBJCOPY) -O binary $(BUILD_DIR)/$(TARGET) $(BUILD_DIR)/$(patsubst %.elf,%.bin,$(TARGET))
	$(OBJDUMP) -d -a $(BUILD_DIR)/$(TARGET) > $(BUILD_DIR)/$(patsubst %.elf,%.txt,$(TARGET))
# 移动 .o 文件到 build 文件夹
	@for file in $(patsubst %.cpp,%.o,$(C_FILE)) $(patsubst %.S,%.o,$(ASM_FILE)); do \
		if [ -f "$$file" ]; then \
			mv "$$file" $(BUILD_DIR)/; \
		fi \
	done
# 移动 .d 文件到 build 文件夹
	@for file in $(patsubst %.cpp,%.d,$(C_FILE)) $(patsubst %.S,%.d,$(ASM_FILE)); do \
		if [ -f "$$file" ]; then \
			mv "$$file" $(BUILD_DIR)/; \
		fi \
	done
	$(PYTHON) $(SCRIPT_DIR)/make_instr_hex.py $(BUILD_DIR)/$(patsubst %.elf,%.bin,$(TARGET)) 65536 > ./build/instruction.hex
	$(PYTHON) $(SCRIPT_DIR)/make_mem_hex.py $(BUILD_DIR)/$(patsubst %.elf,%.bin,$(TARGET)) 65536 > ./build/memory.hex

mod4:
	$(PYTHON) $(SCRIPT_DIR)/genhex_mod4.py ./build/instruction.hex
	$(PYTHON) $(SCRIPT_DIR)/genhex_mod4.py ./build/memory.hex

move:
	cp $(BUILD_DIR)/*.hex ../../sim/simulator/firmware/gcc_backend/
	cp $(BUILD_DIR)/*.txt ../../sim/simulator/firmware/gcc_backend/

menu:
	@menuconfig

genhpp:
ifeq ($(filter testcase1 testcase2, $(TESTCASE_CHOSEN)), $(TESTCASE_CHOSEN))
	@python3 ./scripts/genhpp.py $(TESTCASE_DIR)/$(TESTCASE_CHOSEN)/matrixA.hex matrixA.hpp matrixA
	@python3 ./scripts/genhpp.py $(TESTCASE_DIR)/$(TESTCASE_CHOSEN)/matrixB.hex matrixB.hpp matrixB
	@python3 ./scripts/genhpp.py $(TESTCASE_DIR)/$(TESTCASE_CHOSEN)/matrixD.hex matrixD.hpp matrixD
else ifeq ($(filter testcase3 testcase4 testcase5 testcase6 testcase7, $(TESTCASE_CHOSEN)), $(TESTCASE_CHOSEN))
	@python3 ./scripts/genhpp.py $(TESTCASE_DIR)/$(TESTCASE_CHOSEN)/bias.hex bias.hpp bias
	@python3 ./scripts/genhpp.py $(TESTCASE_DIR)/$(TESTCASE_CHOSEN)/kernel.hex kernel.hpp kernel
	@python3 ./scripts/genhpp.py $(TESTCASE_DIR)/$(TESTCASE_CHOSEN)/input_block.hex input_block.hpp input_block
	@python3 ./scripts/genhpp.py $(TESTCASE_DIR)/$(TESTCASE_CHOSEN)/output_block.hex output_block.hpp output_block
endif

sim:
	make all -B
	make move

clean:
	@rm -rf build
	@rm -rf *.o ../../../acenn/*.o ../../../acenn/*.o *.d ../../../acenn/*.d *.elf *.bin *.hex *.coe *.mif *.map *.txt ../*.hex *.log
	@rm -rf ../support/*.d ../support/*.o

help:
	@echo "no help"
