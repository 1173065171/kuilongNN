# 这个文件是模仿夔龙仿真的内容
from src.Spirit import Spirit_Emulator
from src.UART import UART_Emulator

from src.VEU.VEU import VEU
from src.SAU.SAU import SAU
from src.SRAM.SRAM import SramSp
from tools.run_time import SimulatorTime
from tools.sram_connect import get_sram_from_stack, set_sram_from_stack

import re
import time
import logging

# logging level：DEBUG, INFO, WARNING, ERROR, CRITICAL
logger= logging.getLogger("main_kui")
logger.setLevel(logging.INFO)
if not logger.handlers:
	fh = logging.FileHandler('log/main_running.log', mode='w', encoding='utf-8')
	fh.setFormatter(logging.Formatter('[%(levelname)s] : %(message)s (line:%(lineno)d [%(filename)s]'))
	logger.addHandler(fh)


# define color
REDC = '\033[91m'
GREENC = '\033[92m'
YELLOWC = '\033[93m'
RESETC = '\033[0m'  # reset color


def debug_print(output, color=REDC):
	print(color + output + RESETC)


def parse_txt_file(file_path, pc_value):
	pc_data = int(pc_value, 16)
	function_name = None
	lines = []
	pc_index = -1
	try:
		with open(file_path, 'r') as file:
			current_function = None
			for line in file:
				line_stripped = line.strip()
				match = re.search(r'<(.*?)>:', line_stripped)
				if match:
					current_function = match.group(1)
				if line_stripped.startswith(pc_value + ':'):
					function_name = current_function
					pc_index = len(lines)
				lines.append((current_function, line))
	except FileNotFoundError:
		print(f"Error: Failed to find {file_path}。")
		return None, []
	except Exception as e:
		print(f"Error: Occurred unknown error {e}。")
		return None, []

	if pc_index == -1:
		return None, []

	start_index = max(0, pc_index - 4)
	end_index = min(len(lines), pc_index + 5)
	result_lines = [line[1].strip() for line in lines[start_index:end_index]]
	return function_name, result_lines


def step_debug(asm_file_path, pc_value):
	function_name, result_lines = parse_txt_file(asm_file_path, pc_value)
	if function_name is not None:
		debug_print("The function name where the current PC is located: " + function_name, GREENC)
		debug_print("The nearby context:", GREENC)
		for line in result_lines:
			if pc_value + ":" in line:
				debug_print(line, REDC)
			else:
				debug_print(line, GREENC)
	else:
		debug_print("No matching PC value was found.", GREENC)


def main():
	# log start
	logger.info("program start running!")
	# load file
	asm_file_path =  './firmware/gcc_backend/firmware.txt'
	sram_ins_file =  './firmware/gcc_backend/instruction.hex'
	sram_data_file = './firmware/gcc_backend/memory.hex'
	log_file_path = './log/gcc_backend.log'

	# cpu init
	spirit = Spirit_Emulator.Spirit()
	spirit.verbose = False
	spirit.use_one_sram = False
	spirit.INST_START_ADDR_INIT = int("0x00000000", 16)
	spirit.DATA_START_ADDR_INIT = int("0x10000000", 16)
	spirit.UART_BASEADDR_INIT = int("0x20000000", 16)
	spirit.emulator_init()
	current_cpu_cnt = 0
	last_cpu_cnt = 0

	# config uart output file
	uart = UART_Emulator.UART()
	uart.BASE_ADDRESS = int("0x20000000", 16)
	uart.set_file_name(log_file_path)
	uart.init()
	uart.clean()

	# veu init
	veu = VEU(1, 16)
	veu.init()
	veu_debug_flag = None

	# sau init
	sau = SAU(1, 16, 'int8')
	sau = SAU(2, 16, 'int8')
	sau_debug_flag = None

	# sram init
	sram_ins =SramSp(width=32, depth=65536, base_addr=0x00000000)
	sram_ins.read_from_file(sram_ins_file)
	sram1 =SramSp(width=128, depth=32768, base_addr=0x10000000)
	sram1.read_from_file(sram_data_file)

	logger.info(sram1.data[2374])
	

	# 初始化时间统计模块
	run_time = SimulatorTime()

	"""  spirit.peripherals_req
	1: ready to write peripherals
	2: ready to read peripherals
	3: finish writed peripherals
	4: ready to write and read csr bus
	5: finish writed csr bus
	"""
	while 1:
		while spirit.trap != 1:
			# 获取当前指令，并送入cpu
			pc = spirit.get_current_pc()
			pc_intruction_lsb_int = int.from_bytes(sram_ins.read(True,pc), byteorder='big')
			pc_instruction_msb_int = int.from_bytes(sram_ins.read(True,pc+4), byteorder='big')
			spirit.set_instr(pc_instruction_msb_int, pc_intruction_lsb_int)
			spirit.emulate()

			# log当前指令的pc
			last_cpu_cnt = current_cpu_cnt
			current_cpu_cnt = spirit.get_csr(0)
			logger.debug(f"🐱--cpu current cnt is: {current_cpu_cnt}, last cnt is: {last_cpu_cnt}, now_pc_is: {hex(pc)}")

			# run extra devices
			if (spirit.peripherals_req == 1) | (spirit.peripherals_req == 2):
				# (1)write device // (2)read device
				if (spirit.get_peripherals_bus_addr() & 0xF000_0000) == uart.BASE_ADDRESS:
					# 针对 uart 的读写操作
					uart.set_peripherals_bus_enable(spirit.get_peripherals_bus_enable())
					uart.set_peripherals_bus_wstrb(spirit.get_peripherals_bus_wstrb())
					uart.set_peripherals_bus_addr(spirit.get_peripherals_bus_addr() & 0xFF)
					uart.set_peripherals_bus_wdata(spirit.get_peripherals_bus_wdata())
					uart.run()
					spirit.set_peripherals_bus_rdata(uart.get_peripherals_bus_rdata())
					# log printf 函数的信息
					logger.debug(f"--uart address: {hex(uart.get_peripherals_bus_addr())}, wstrb: {uart.get_peripherals_bus_wstrb()}, wdata: {hex(uart.get_peripherals_bus_wdata())}")
					spirit.peripherals_req = 3

				elif (spirit.get_peripherals_bus_addr() & 0xF000_0000) == sram1.base_addr:
					# 收集 cpu2sram 的读写信息
					sram_cpu_en = spirit.get_peripherals_bus_enable()
					sram_cpu_wstrb = spirit.get_peripherals_bus_wstrb()
					sram_cpu_addr = spirit.get_peripherals_bus_addr() & 0xFFF_FFFC
					# 进行读写操作
					sram_cpu_wdata = spirit.get_peripherals_bus_wdata()
					sram_cpu_rdata = sram1.read_cpu(sram_cpu_en, sram_cpu_addr)
					spirit.set_peripherals_bus_rdata(sram_cpu_rdata)
					sram1.write_cpu(sram_cpu_en, sram_cpu_wstrb, sram_cpu_addr, sram_cpu_wdata)
					# finish once operation
					spirit.peripherals_req = 3
					# log printf 函数的信息
					if sram_cpu_addr == 37984:
						logger.info("🐱--Main CPU read and write is pc: %s, en: %s, wstrb: %s, addr: %s, wdata: %s, rdata: %s",
									  hex(pc), sram_cpu_en, sram_cpu_wstrb, hex(sram_cpu_addr), hex(sram_cpu_wdata), hex(sram_cpu_rdata))
					logger.debug("🐱--Main CPU read and write is pc: %s, en: %s, wstrb: %s, addr: %s, wdata: %s, rdata: %s",
						hex(pc), sram_cpu_en, sram_cpu_wstrb, hex(sram_cpu_addr), hex(sram_cpu_wdata), hex(sram_cpu_rdata))
			# run the Heterogeneous device
			elif spirit.peripherals_req == 4:  # write and read csr
				# -----------------  run the Heterogeneous (veu)  --------------------------
				if (spirit.get_csr_bus_addr() & 0xF00) == 0x100:
					# 接收csr
					csr_re = spirit.get_csr_bus_re()
					csr_we = spirit.get_csr_bus_we()
					csr_write_type = spirit.get_csr_bus_write_type()
					csr_addr = spirit.get_csr_bus_addr() & 0xF0F
					csr_wdata1 = spirit.get_csr_bus_wdata1()
					csr_wdata2 = spirit.get_csr_bus_wdata2()
					csr_vestart = spirit.get_csr_bus_vestart()
					# 读取csr
					csr_rdata = veu.get_csr(csr_addr)
					spirit.set_csr_bus_rdata(csr_rdata)
					# 写入csr
					veu.set_csr(csr_en=csr_we,
								operation=csr_write_type,
								csr_addr=csr_addr,
								csr_wdata1=csr_wdata1,
								csr_wdata2=csr_wdata2,
								csr_status=csr_vestart)
					# log csr read and write
					logger.info(f"🐱--VEU now_pc: {hex(pc)}, csr_en: {csr_re}, csr_we: {csr_we}, csr_write_type: {csr_write_type}, addr: {csr_addr}, csr_wdata1: {csr_wdata1}, csr_wdata2: {csr_wdata2}, csr_vstart:{csr_vestart}, csr_rdata:{csr_rdata}")
					# 获取工作信号
					running = veu.get_csr(veu.csr_addr.VEU_STATUS)
					# 硬件循环 （running = 1）
					while (running & 0x1) != 0:
						# 更新status和config寄存器
						veu.update_status()
						veu.update_config()
						# 读取
						veu.update_input_buffer1(sram1.read(True,veu.csr.raddr1))
						veu.update_input_buffer2(sram1.read(True,veu.csr.raddr2))
						# 运行指令
						veu.run()
						# 写回
						if veu.config.write_back:
							sram1.write(True,veu.sram_wstrb,veu.csr.waddr,veu.output_buffer.bank)
							# 修补操作
							veu.repair_backward()
						# 更新状态寄存器
						running = veu.csr.status
						# 累计计算运行时间
						vector_len = veu.csr.vlen  # 向量bit长度
						running_time = vector_len // 128 * 3
						run_time.accumulate_veu_time(running_time)
				# -----------------  run the Heterogeneous (sau)  --------------------------
				elif (spirit.get_csr_bus_addr() & 0xF00) == 0x200:
					# 1、获取csr接口数据
					sau_en = spirit.get_csr_bus_we() | spirit.get_csr_bus_re()
					csr_re = spirit.get_csr_bus_re()
					csr_we = spirit.get_csr_bus_we()
					sau_operation = spirit.get_csr_bus_write_type()
					sau_addr = spirit.get_csr_bus_addr() & 0x00f
					sau_wdata1 = spirit.get_csr_bus_wdata1()
					sau_wdata2 = spirit.get_csr_bus_wdata2()
					# 2、写入 csr 缓存区
					if sau_en == 1 & sau_operation == 1:
						sau.set_csr_bus(sau_en, sau_operation, sau_addr, sau_wdata2, sau_wdata1)

					if sau_en == 1 & (sau_addr // 2 == 2):
						sau.set_sau()
					# 4、解析出指令项 start==1 启动SAU执行，硬件循环在里面
					if sau.ins.start == 1:
						# TODO： sram的分块映射，考虑多核地址
						# 这里对 spirit.sram 做一个虚拟映射，处理完后再映射回去，read_sram和write_sram可能会需要循环
						# sram_arrayd的格式是list[int32, int32, int32, ...]
						if sau_debug_flag:
							# 测试一些仿真器搬移操作需要多少时间
							start_time = time.time()
							i_sram_array = get_sram_from_stack(spirit, 0x50000000, 32768)
							end_time = time.time()
							print("get sram_array time:", end_time - start_time)
							print("sram_array element type:", type(i_sram_array[0]))
							print("sram_array size:", len(i_sram_array))
							with open("i_sram_array_output.txt", "w") as file:
								for item in i_sram_array:
									file.write(f"{item}\n")
						else:
							# 直接搬移
							i_sram_array = get_sram_from_stack(spirit, 0x50000000, 32768)
						sau.read_sram(i_sram_array)
						# 执行一次sau指令运算操作
						sau.run()
						# 将sau的ram数据写回 spirit.sram
						o_sram_array = sau.write_sram()
						set_sram_from_stack(spirit, 0x50000000, o_sram_array)

					# 累计计算运行时间
					running_time = sau.total_time
					run_time.accumulate_sau_time(running_time)

				spirit.peripherals_req = 5
			if spirit.trap == 1 or spirit.trap == 2:
				while 1:
					command = input(REDC + "wait your command:" + RESETC)
					if command == 's' or command == 'step':
						debug_print("exec step command!", GREENC)
						pc_value = hex(spirit.get_current_pc())[2:]
						step_debug(asm_file_path, pc_value)
						spirit.trap = 2
						break
					elif command == 'c' or command == 'continue':
						debug_print("exec continue command!", GREENC)
						spirit.trap = 0
						break
					elif command == 'b' or command == 'break out':
						debug_print("exect and calculation time!", GREENC)
						spirit.trap = 1
						break
					elif command == 'vt' or command == 'verbosetrue':
						spirit.verbose = True
						debug_print("set verbose True!", GREENC)
					elif command == 'vf' or command == 'verbosefalse':
						spirit.verbose = False
						debug_print("set verbose False!", GREENC)
					elif command == 'e' or command == 'exit' or command == 'q' or command == 'quit':
						debug_print("exit by python!\n", GREENC)
						break
					elif command == 'g' or command == 'getsram':
						debug_print("exec getsram command!", GREENC)
						while 1:
							str_addr = input("please input addr:")
							decimal_addr = int(str_addr, 16)
							# debug_sram_data = spirit.get_sram(decimal_addr)
							debug_sram_data = sram1.read_cpu(en=1, address=decimal_addr)
							debug_print("sram[" + hex(decimal_addr & 0xFFFFFFFC) + "]: " + hex(debug_sram_data), GREENC)
							whetherbreak = input("exit or continue:")
							if (
									whetherbreak == "e" or whetherbreak == "exit" or whetherbreak == 'q' or whetherbreak == 'quit'):
								break
					else:
						debug_print("get a error command!\n", YELLOWC)
		break
	# 5、打印运行时间
	cpu_runtime = spirit.get_csr(1)
	run_time.accumulate_cpu_time(cpu_runtime)
	run_time.print_run_time()



if __name__ == "__main__":
	main()