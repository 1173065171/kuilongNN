# 配置仿真器环境
# 由于macos的限制，需要对gcc有独特的配置

ifeq ($(IS_MACOS),1)
    export CC=gcc-11
    export CXX=g++-11
    export MACOS_FLAG=-undefined dynamic_lookup
    export PYTHON=/opt/anaconda3/envs/npu_sim/bin/python
    export PYTHONH=/opt/anaconda3/envs/npu_sim/include/python3.10
else
    export CC=gcc
    export CXX=g++
    export MACOS_FLAG=
    export PYTHON=/home/zbn/software/miniconda3/envs/lpnpu_sim/bin/python
    export PYTHONH=/home/zbn/software/miniconda3/envs/lpnpu_sim/include/python3.10
endif
export SWIG=swig

# work dir
export SIMULATOR_HOME=$(shell pwd)

# swig-work dir
include ${SIMULATOR_HOME}/src/Spirit/Makefile
include ${SIMULATOR_HOME}/src/Device/Makefile
include ${SIMULATOR_HOME}/src/UART/Makefile

# main
build:
	cd ./src/Spirit && make swig_cpu -B
	cd ./src/Device && make swig_device -B
	cd ./src/UART && make swig_uart -B

all:
	${PYTHON} main_kui.py | tee ${SIMULATOR_HOME}/log/record.log

test:
	${PYTHON} main_kui.py

# remove \x1b
r1b:
	${PYTHON} remove_ansi_escapes.py

clean_log:
	rm -vrf *.log

clean:
	cd ./src/Spirit && make clean_cpu
	cd ./src/Device && make clean_device
	cd ./src/UART && make clean_uart
	rm -vrf *.log
