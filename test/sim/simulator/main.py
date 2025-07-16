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
from dataclasses import dataclass

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

@dataclass
class csr_bus:
	csr_re: int = 0  # csr read enable
	csr_we: int = 0  # csr write enable
	csr_write_type: int = 0  # csr write type
	csr_addr: int = 0  # csr address
	csr_wdata1: int = 0  # csr write data1
	csr_wdata2: int = 0  # csr write data2
	csr_rdata: int = 0  # csr read data


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


def sim():
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
	sau = SAU(1, 16)
	sau_debug_flag = None

	# sram init
	sram_ins =SramSp(width=32, depth=65536, base_addr=0x00000000)
	sram_ins.read_from_file(sram_ins_file)
	sram1 =SramSp(width=128, depth=32768, base_addr=0x10000000, burst_bytes=16)
	sram1.read_from_file(sram_data_file)

	# csr_bus
	csr = csr_bus()
	

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
					# 1.接收csr
					csr.csr_re = spirit.get_csr_bus_re()
					csr.csr_we = spirit.get_csr_bus_we()
					csr.csr_write_type = spirit.get_csr_bus_write_type()
					csr.csr_addr = spirit.get_csr_bus_addr() & 0xF0F
					csr.csr_wdata1 = spirit.get_csr_bus_wdata1()
					csr.csr_wdata2 = spirit.get_csr_bus_wdata2()
					csr.csr_vestart = spirit.get_csr_bus_vestart()
					# 2.读取csr
					csr.csr_rdata = veu.get_csr(csr.csr_addr)
					spirit.set_csr_bus_rdata(csr.csr_rdata)
					# 3.写入csr
					veu.set_csr(csr_en=csr.csr_we,
								operation=csr.csr_write_type,
								csr_addr=csr.csr_addr,
								csr_wdata1=csr.csr_wdata1,
								csr_wdata2=csr.csr_wdata2,
								csr_status=csr.csr_vestart)
					# log csr read and write
					logger.info(f"🐱--VEU now_pc: {hex(pc)}, csr: {csr}")
					# 4.获取工作信号
					running = veu.get_csr(veu.csr_addr.VEU_STATUS)
					# 5.硬件循环 （running = 1）
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
					# 1.接收csr
					csr.csr_re = spirit.get_csr_bus_re()
					csr.csr_we = spirit.get_csr_bus_we()
					csr.csr_write_type = spirit.get_csr_bus_write_type()
					csr.csr_addr = spirit.get_csr_bus_addr() & 0xF0F
					csr.csr_wdata1 = spirit.get_csr_bus_wdata1()
					csr.csr_wdata2 = spirit.get_csr_bus_wdata2()
					# 2.读取csr
					csr.csr_rdata = sau.get_csr(csr.csr_addr)
					spirit.set_csr_bus_rdata(csr.csr_rdata)
					# 3、写入 csr 缓存区
					sau.set_csr(csr_en=csr.csr_we, 
				 				operation=csr.csr_write_type, 
								csr_addr=csr.csr_addr, 
								csr_wdata1=csr.csr_wdata1, 
								csr_wdata2=csr.csr_wdata2)

					# log csr read and write
					logger.info(f"🐱--SAU now_pc: {hex(pc)}, csr: {csr}")
					# 4、获取工作信号
					running = sau.status.running
					# 5.硬件循环 （running = 1）
					while running:
						# 更新寄存器
						sau.update_status()
						# burst读数据
						sau.update_input_m1(sram1.read_burst(True, sau.status.A_address, sau.status.A_step, sau.status.A_count, sau.status.A_kernel))
						sau.update_input_m2(sram1.read_burst(True, sau.status.B_address, sau.status.B_step, sau.status.B_count, sau.status.B_kernel))
						sau.update_input_m3(sram1.read(True, sau.status.C_address))

						# 执行一次 systolic array 的 flow
						sau.run()

						# 写回结果
						if sau.status.flow_i == sau.config.flow_loop_times:
							# burst写数据
							sram1.write_burst(True, sau.status.D_wstrb, sau.status.D_address, sau.status.D_step, sau.status.D_count, sau.output_matrix.matrix)
							# 写回后修补操作
							sau.repair_backward()

						running = sau.status.running
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
							if (whetherbreak == "e" or whetherbreak == "exit" or whetherbreak == 'q' or whetherbreak == 'quit'):
								break
					else:
						debug_print("get a error command!\n", YELLOWC)
		break
	# 5、打印运行时间
	cpu_runtime = spirit.get_csr(1)
	run_time.accumulate_cpu_time(cpu_runtime)
	run_time.print_run_time()


def main():
	sim()

if __name__ == "__main__":
	main()