from src.Spirit import Spirit_Emulator
from src.VEU import VEU_Emulator
from src.SAU.SAU import SAU
from src.UART import UART_Emulator
from tools.run_time import Simulator_time
import re
import time

# define color
REDC = '\033[91m'
GREENC = '\033[92m'
YELLOWC = '\033[93m'
RESETC = '\033[0m'  # reset color

def DEBUG_PRINT(output, color = REDC):
	print(color + output + RESETC)

def get_sram_from_stack(spirit, start_addr, length):
	lists = []
	offset = 0
	for i in range(length):
		data = spirit.get_sram(start_addr + offset)
		lists.append(f"{data & 0xFFFFFFFF:08X}")
		offset += 4
	return lists

def set_sram_from_stack(spirit, start_addr, lists):
	offset = 0
	for data in lists:
		spirit.set_sram(start_addr + offset, int(data, 16))
		offset += 4

def get_sram_data(waddr_wstrb, new_sram_data, origin_sram_data):
	temp_sram_0 = 0
	temp_sram_1 = 0
	temp_sram_2 = 0
	temp_sram_3 = 0
	if ((waddr_wstrb & 0b0001) == 0b0001):
		temp_sram_0 = (new_sram_data & 0x000000FF)
	else:
		temp_sram_0 = (origin_sram_data & 0x000000FF)

	if ((waddr_wstrb & 0b0010) == 0b0010):
		temp_sram_1 = (new_sram_data & 0x0000FF00)
	else:
		temp_sram_1 = (origin_sram_data & 0x0000FF00)

	if ((waddr_wstrb & 0b0100) == 0b0100):
		temp_sram_2 = (new_sram_data & 0x00FF0000)
	else:
		temp_sram_2 = (origin_sram_data & 0x00FF0000)

	if ((waddr_wstrb & 0b1000) == 0b1000):
		temp_sram_3 = (new_sram_data & 0xFF000000)
	else:
		temp_sram_3 = (origin_sram_data & 0xFF000000)
	return (temp_sram_3 | temp_sram_2 | temp_sram_1 | temp_sram_0)

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
				if line_stripped.startswith(pc_value+':'):
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
	if function_name != None:
		DEBUG_PRINT("The function name where the current PC is located: "+function_name, GREENC)
		DEBUG_PRINT("The nearby context:", GREENC)
		for line in result_lines:
			if pc_value + ":" in line:
				DEBUG_PRINT(line, REDC)
			else:
				DEBUG_PRINT(line, GREENC)
	else:
		DEBUG_PRINT("No matching PC value was found.", GREENC)

def one_cnt_16bit(mask):
	result = 0
	for i in range(16):
		if ((mask >> i) & 0x1) == 1:
			result = result + 1
	return result
	

def main():
	# spirit                      = Spirit_Emulator.Spirit()
	# spirit.verbose              = False
	# spirit.use_one_sram         = False
	# spirit.INST_START_ADDR_INIT = int("0x00000000", 16)
	# spirit.DATA_START_ADDR_INIT = int("0x00000000", 16)
	# spirit.UART_BASEADDR_INIT   = int("0x10000000", 16)
	# spirit.emulator_init("./firmware/rv32im/instruction.hex", "./firmware/rv32im/memory.hex")
	# spirit.emulator_init("./firmware/rv32imc/instruction.hex", "./firmware/rv32imc/memory.hex")
	# spirit.emulator_init("./firmware/dhrystone/instruction.hex", "./firmware/dhrystone/memory.hex")
	# spirit.set_sram(int("0x10000000", 16)>>2, 0)
	# spirit.set_sram(int("0x10000008", 16)>>2, 0)
	# spirit.set_sram(int("0x1000000C", 16)>>2, 0)

	# uart                = UART_Emulator.UART()
	# uart.BASE_ADDRESS   = int("0x10000000", 16)
	# uart.set_file_name("./rv32im.log")
	# uart.set_file_name("./rv32imc.log")
	# uart.set_file_name("./dhrystone.log")
	# uart.init()
	# uart.clean()

	spirit                      = Spirit_Emulator.Spirit()
	spirit.verbose              = False
	spirit.use_one_sram         = False
	spirit.INST_START_ADDR_INIT = int("0x00000000", 16)
	spirit.DATA_START_ADDR_INIT = int("0x40000000", 16)
	spirit.UART_BASEADDR_INIT   = int("0x81000000", 16)
	# spirit.emulator_init("./firmware/test_printf/instruction.hex", "./firmware/test_printf/memory.hex")
	# spirit.emulator_init("./firmware/mobilenet_v1/instruction.hex", "./firmware/mobilenet_v1/memory.hex")
	spirit.emulator_init("./firmware/gcc_backend/instruction.hex", "./firmware/gcc_backend/memory.hex")
	# spirit.emulator_init("./firmware/median/instruction.hex", "./firmware/median/memory.hex")
	# spirit.emulator_init("./firmware/memcpy/instruction.hex", "./firmware/memcpy/memory.hex")
	# spirit.emulator_init("./firmware/multiply/instruction.hex", "./firmware/multiply/memory.hex")
	# spirit.emulator_init("./firmware/qsort/instruction.hex", "./firmware/qsort/memory.hex")
	# spirit.emulator_init("./firmware/rsort/instruction.hex", "./firmware/rsort/memory.hex")
	# spirit.emulator_init("./firmware/spmv/instruction.hex", "./firmware/spmv/memory.hex")
	# spirit.emulator_init("./firmware/towers/instruction.hex", "./firmware/towers/memory.hex")
	# spirit.emulator_init("./firmware/vvadd/instruction.hex", "./firmware/vvadd/memory.hex")
	# spirit.emulator_init("./firmware/vec-daxpy/instruction.hex", "./firmware/vec-daxpy/memory.hex")

	asm_file_path = 'firmware/gcc_backend/firmware.txt'

	uart                = UART_Emulator.UART()
	uart.BASE_ADDRESS   = int("0x81000000", 16)
	# uart.set_file_name("./test_printf.log")
	# uart.set_file_name("./mobilenet_v1.log")
	uart.set_file_name("./gcc_backend.log")
	# uart.set_file_name("./median.log")
	# uart.set_file_name("./memcpy.log")
	# uart.set_file_name("./multiply.log")
	# uart.set_file_name("./qsort.log")
	# uart.set_file_name("./rsort.log")
	# uart.set_file_name("./spmv.log")
	# uart.set_file_name("./towers.log")
	# uart.set_file_name("./vvadd.log")
	# uart.set_file_name("./vec-daxpy.log")
	uart.init()
	uart.clean()

	veu = VEU_Emulator.VEU()
	veu.init()

	sau = SAU(1,16,'int8')
	sau = SAU(2,16,'int8')
	SAU_debuf_flag = None

	# 初始化时间统计模块
	run_time = Simulator_time()

	"""  spirit.peripherals_req
	1: ready to write peripherals
	2: ready to read peripherals
	3: finish writed peripherals
	4: ready to write and read csr bus
	5: finish writed csr bus
	"""
	while 1:
		while spirit.trap != 1:
			spirit.emulate()
			if spirit.peripherals_req == 1: #   write device
				if (spirit.get_peripherals_bus_addr() & 0xFF00_0000) == uart.BASE_ADDRESS:
					uart.set_peripherals_bus_enable(spirit.get_peripherals_bus_enable())
					uart.set_peripherals_bus_wstrb(spirit.get_peripherals_bus_wstrb())
					uart.set_peripherals_bus_addr(spirit.get_peripherals_bus_addr() & 0xFF)
					uart.set_peripherals_bus_wdata(spirit.get_peripherals_bus_wdata())
					uart.run()
					spirit.set_peripherals_bus_rdata(uart.get_peripherals_bus_rdata())
					spirit.peripherals_req = 3
			elif spirit.peripherals_req == 2: #   read device
				if (spirit.get_peripherals_bus_addr() & 0xFF00_0000) == uart.BASE_ADDRESS:
					uart.set_peripherals_bus_enable(spirit.get_peripherals_bus_enable())
					uart.set_peripherals_bus_wstrb(spirit.get_peripherals_bus_wstrb())
					uart.set_peripherals_bus_addr(spirit.get_peripherals_bus_addr() & 0xFF)
					uart.set_peripherals_bus_wdata(spirit.get_peripherals_bus_wdata())
					uart.run()
					spirit.set_peripherals_bus_rdata(uart.get_peripherals_bus_rdata())
					spirit.peripherals_req = 3
			elif spirit.peripherals_req == 4: # write and read csr
				# print(spirit.get_csr_bus_addr())
				if (spirit.get_csr_bus_addr() & 0xF00) == 0x100:
					veu.set_csr_bus_re(spirit.get_csr_bus_re())
					veu.set_csr_bus_we(spirit.get_csr_bus_we())
					veu.set_csr_bus_write_type(spirit.get_csr_bus_write_type())
					veu.set_csr_bus_addr(spirit.get_csr_bus_addr() & 0xF0F)
					veu.set_csr_bus_wdata1(spirit.get_csr_bus_wdata1())
					veu.set_csr_bus_wdata2(spirit.get_csr_bus_wdata2())
					veu.set_csr_bus_vestart(spirit.get_csr_bus_vestart())
					veu.update_csr()
					running = veu.read_csr(veu.VEUSTATUS)
					vector_len = veu.read_csr(veu.VEUVLEN) #向量bit长度

					while (running & 0x1) != 0:
						buffer1_bank0 = 0
						buffer1_bank1 = 0
						buffer1_bank2 = 0
						buffer1_bank3 = 0
						buffer2_bank0 = 0
						buffer2_bank1 = 0
						buffer2_bank2 = 0
						buffer2_bank3 = 0
						# get input_buffer1
						# vnclip
						if (((veu.read_csr(veu.VEUSTATUS) >> 15) & 0x1) or ((veu.read_csr(veu.VEUDEBUG) >> 15) & 0b1)):
							buffer1_bank0   =   veu.read_csr(veu.VEURADDR1)
							buffer1_bank1   =   veu.read_csr(veu.VEURADDR1)
							buffer1_bank2   =   veu.read_csr(veu.VEURADDR1)
							buffer1_bank3   =   veu.read_csr(veu.VEURADDR1)
						else:
							if (veu.read_csr(veu.VEURADDR1) & 0x3) == 0x0:
								buffer1_bank0 = spirit.get_sram(veu.read_csr(veu.VEURADDR1))
								buffer1_bank1 = spirit.get_sram(veu.read_csr(veu.VEURADDR1) + 0x4)
								buffer1_bank2 = spirit.get_sram(veu.read_csr(veu.VEURADDR1) + 0x8)
								buffer1_bank3 = spirit.get_sram(veu.read_csr(veu.VEURADDR1) + 0xC)
							if (veu.read_csr(veu.VEURADDR1) & 0x3) == 0x1: # discard low 8 bit
								buffer1_bank0 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR1)) >> 8) & 0xFFFFFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0x4)) << 24) & 0xFF000000
								buffer1_bank1 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0x4)) >> 8) & 0xFFFFFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0x8)) << 24) & 0xFF000000
								buffer1_bank2 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0x8)) >> 8) & 0xFFFFFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0xC)) << 24) & 0xFF000000
								buffer1_bank3 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0xC)) >> 8) & 0xFFFFFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0x10)) << 24) & 0xFF000000
							if (veu.read_csr(veu.VEURADDR1) & 0x3) == 0x2: # discard low 16 bit
								buffer1_bank0 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR1)) >> 16) & 0xFFFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0x4)) << 16) & 0xFFFF0000
								buffer1_bank1 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0x4)) >> 16) & 0xFFFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0x8)) << 16) & 0xFFFF0000
								buffer1_bank2 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0x8)) >> 16) & 0xFFFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0xC)) << 16) & 0xFFFF0000
								buffer1_bank3 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0xC)) >> 16) & 0xFFFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0x10)) << 16) & 0xFFFF0000
							if (veu.read_csr(veu.VEURADDR1) & 0x3) == 0x3: # discard low 24 bit
								buffer1_bank0 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR1)) >> 24) & 0xFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0x4)) << 8) & 0xFFFFFF00
								buffer1_bank1 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0x4)) >> 24) & 0xFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0x8)) << 8) & 0xFFFFFF00
								buffer1_bank2 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0x8)) >> 24) & 0xFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0xC)) << 8) & 0xFFFFFF00
								buffer1_bank3 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0xC)) >> 24) & 0xFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR1 + 0x10)) << 8) & 0xFFFFFF00
						# get input_buffer2
						# vssrl, vssra
						# vnclip
						# vwredsum (disable getsram)
						# vredsum (disable getsram)
						# vmv
						if (((veu.read_csr(veu.VEUCFG) >> 11) & 0x1) or \
						((veu.read_csr(veu.VEUSTATUS) >> 13) & 0b1) or ((veu.read_csr(veu.VEUDEBUG) >> 13) & 0b1) or \
						((veu.read_csr(veu.VEUSTATUS) >> 14) & 0b1) or ((veu.read_csr(veu.VEUDEBUG) >> 14) & 0b1) or \
						((veu.read_csr(veu.VEUSTATUS) >> 15) & 0x1) or ((veu.read_csr(veu.VEUDEBUG) >> 15) & 0b1) or \
						((veu.read_csr(veu.VEUSTATUS) >> 16) & 0x1) or ((veu.read_csr(veu.VEUDEBUG) >> 16) & 0b1) or \
						((veu.read_csr(veu.VEUSTATUS) >> 17) & 0x1) or ((veu.read_csr(veu.VEUDEBUG) >> 17) & 0b1) or \
						((veu.read_csr(veu.VEUSTATUS) >> 11) & 0x1) or ((veu.read_csr(veu.VEUDEBUG) >> 11) & 0b1)):
							buffer2_bank0   =   veu.read_csr(veu.VEURADDR2)
							buffer2_bank1   =   veu.read_csr(veu.VEURADDR2)
							buffer2_bank2   =   veu.read_csr(veu.VEURADDR2)
							buffer2_bank3   =   veu.read_csr(veu.VEURADDR2)
						else:
							if (veu.read_csr(veu.VEURADDR2) & 0x3) == 0:
								buffer2_bank0 = spirit.get_sram(veu.read_csr(veu.VEURADDR2))
								buffer2_bank1 = spirit.get_sram(veu.read_csr(veu.VEURADDR2) + 0x4)
								buffer2_bank2 = spirit.get_sram(veu.read_csr(veu.VEURADDR2) + 0x8)
								buffer2_bank3 = spirit.get_sram(veu.read_csr(veu.VEURADDR2) + 0xC)
							if (veu.read_csr(veu.VEURADDR2) & 0x3) == 0x1: # discard low 8 bit
								buffer2_bank0 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR2)) >> 8) & 0xFFFFFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0x4)) << 24) & 0xFF000000
								buffer2_bank1 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0x4)) >> 8) & 0xFFFFFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0x8)) << 24) & 0xFF000000
								buffer2_bank2 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0x8)) >> 8) & 0xFFFFFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0xC)) << 24) & 0xFF000000
								buffer2_bank3 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0xC)) >> 8) & 0xFFFFFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0x10)) << 24) & 0xFF000000
							if (veu.read_csr(veu.VEURADDR2) & 0x3) == 0x2: # discard low 16 bit
								buffer2_bank0 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR2)) >> 16) & 0xFFFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0x4)) << 16) & 0xFFFF0000
								buffer2_bank1 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0x4)) >> 16) & 0xFFFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0x8)) << 16) & 0xFFFF0000
								buffer2_bank2 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0x8)) >> 16) & 0xFFFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0xC)) << 16) & 0xFFFF0000
								buffer2_bank3 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0xC)) >> 16) & 0xFFFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0x10)) << 16) & 0xFFFF0000
							if (veu.read_csr(veu.VEURADDR2) & 0x3) == 0x3: # discard low 24 bit
								buffer2_bank0 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR2)) >> 24) & 0xFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0x4)) << 8) & 0xFFFFFF00
								buffer2_bank1 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0x4)) >> 24) & 0xFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0x8)) << 8) & 0xFFFFFF00
								buffer2_bank2 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0x8)) >> 24) & 0xFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0xC)) << 8) & 0xFFFFFF00
								buffer2_bank3 = ((spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0xC)) >> 24) & 0xFF) | \
												(spirit.get_sram(veu.read_csr(veu.VEURADDR2 + 0x10)) << 8) & 0xFFFFFF00
						# get output_buffers
						# vmac
						# vnclip
						if (((veu.read_csr(veu.VEUCFG) >> 12) & 0b1) or \
						((veu.read_csr(veu.VEUSTATUS) >> 19) & 0b1) or ((veu.read_csr(veu.VEUDEBUG) >> 19) & 0b1) or \
						((veu.read_csr(veu.VEUSTATUS) >> 15) & 0x1) or ((veu.read_csr(veu.VEUDEBUG) >> 15) & 0b1)):
							if (veu.read_csr(veu.VEUWADDR) & 0x3) == 0x0:
								update_output_bank0 = spirit.get_sram(veu.read_csr(veu.VEUWADDR))
								update_output_bank1 = spirit.get_sram(veu.read_csr(veu.VEUWADDR) + 0x4)
								update_output_bank2 = spirit.get_sram(veu.read_csr(veu.VEUWADDR) + 0x8)
								update_output_bank3 = spirit.get_sram(veu.read_csr(veu.VEUWADDR) + 0xC)
							if (veu.read_csr(veu.VEUWADDR) & 0x3) == 0x1: # discard low 8 bit
								update_output_bank0 =   ((spirit.get_sram(veu.read_csr(veu.VEUWADDR)) >> 8) & 0xFFFFFF) | \
														(spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0x4)) << 24) & 0xFF000000
								update_output_bank1 =   ((spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0x4)) >> 8) & 0xFFFFFF) | \
														(spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0x8)) << 24) & 0xFF000000
								update_output_bank2 =   ((spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0x8)) >> 8) & 0xFFFFFF) | \
														(spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0xC)) << 24) & 0xFF000000
								update_output_bank3 =   ((spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0xC)) >> 8) & 0xFFFFFF) | \
														(spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0x10)) << 24) & 0xFF000000
							if (veu.read_csr(veu.VEUWADDR) & 0x3) == 0x2: # discard low 16 bit
								update_output_bank0 = ((spirit.get_sram(veu.read_csr(veu.VEUWADDR)) >> 16) & 0xFFFF) | \
													(spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0x4)) << 16) & 0xFFFF0000
								update_output_bank1 = ((spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0x4)) >> 16) & 0xFFFF) | \
													(spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0x8)) << 16) & 0xFFFF0000
								update_output_bank2 = ((spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0x8)) >> 16) & 0xFFFF) | \
													(spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0xC)) << 16) & 0xFFFF0000
								update_output_bank3 = ((spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0xC)) >> 16) & 0xFFFF) | \
													(spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0x10)) << 16) & 0xFFFF0000
							if (veu.read_csr(veu.VEUWADDR) & 0x3) == 0x3: # discard low 24 bit
								update_output_bank0 = ((spirit.get_sram(veu.read_csr(veu.VEUWADDR)) >> 24) & 0xFF) | \
													(spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0x4)) << 8) & 0xFFFFFF00
								update_output_bank1 = ((spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0x4)) >> 24) & 0xFF) | \
													(spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0x8)) << 8) & 0xFFFFFF00
								update_output_bank2 = ((spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0x8)) >> 24) & 0xFF) | \
													(spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0xC)) << 8) & 0xFFFFFF00
								update_output_bank3 = ((spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0xC)) >> 24) & 0xFF) | \
													(spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0x10)) << 8) & 0xFFFFFF00
							veu.update_output_buffer(update_output_bank0, update_output_bank1, update_output_bank2, update_output_bank3)
						veu.update_buffer1(buffer1_bank0, buffer1_bank1, buffer1_bank2, buffer1_bank3)
						veu.update_buffer2(buffer2_bank0, buffer2_bank1, buffer2_bank2, buffer2_bank3)
						veu.run()
						if (veu.read_csr(veu.VEUCFG) & 0x400) == 0x400:
							if (veu.read_csr(veu.VEUWADDR) & 0x3) == 0:
								spirit.set_sram(veu.read_csr(veu.VEUWADDR), get_sram_data(veu.waddr_wstrb & 0xF, veu.read_output_buffer(0), spirit.get_sram(veu.read_csr(veu.VEUWADDR))))
								spirit.set_sram(veu.read_csr(veu.VEUWADDR) + 0x4, get_sram_data((veu.waddr_wstrb >> 4) & 0xF, veu.read_output_buffer(1), spirit.get_sram(veu.read_csr(veu.VEUWADDR) + 0x4)))
								spirit.set_sram(veu.read_csr(veu.VEUWADDR) + 0x8, get_sram_data((veu.waddr_wstrb >> 8) & 0xF, veu.read_output_buffer(2), spirit.get_sram(veu.read_csr(veu.VEUWADDR) + 0x8)))
								spirit.set_sram(veu.read_csr(veu.VEUWADDR) + 0xC, get_sram_data((veu.waddr_wstrb >> 12) & 0xF, veu.read_output_buffer(3), spirit.get_sram(veu.read_csr(veu.VEUWADDR) + 0xC)))
							if (veu.read_csr(veu.VEUWADDR) & 0x3) == 0x1: # save low 8 bit
								spirit.set_sram(veu.read_csr(veu.VEUWADDR), (spirit.get_sram(veu.read_csr(veu.VEUWADDR)) & 0xFF) | \
																			((veu.read_output_buffer(0) & 0xFFFFFF) << 8))
								spirit.set_sram(veu.read_csr(veu.VEUWADDR) + 0x4,   ((veu.read_output_buffer(0) >> 24) & 0xFF) | \
																					((veu.read_output_buffer(1) >> 0) & 0xFFFFFF))
								spirit.set_sram(veu.read_csr(veu.VEUWADDR) + 0x8,   ((veu.read_output_buffer(1) >> 24) & 0xFF) | \
																					((veu.read_output_buffer(2) >> 0) & 0xFFFFFF))
								spirit.set_sram(veu.read_csr(veu.VEUWADDR) + 0xC,   ((veu.read_output_buffer(2) >> 24) & 0xFF) | \
																					((veu.read_output_buffer(3) >> 0) & 0xFFFFFF))
								spirit.set_sram(veu.read_csr(veu.VEUWADDR) + 0x10,  (spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0x10)) & 0xFFFFFF00) | \
																					((veu.read_output_buffer(3) >> 24) & 0xFF))
							if (veu.read_csr(veu.VEUWADDR) & 0x3) == 0x2: # save low 16 bit
								spirit.set_sram(veu.read_csr(veu.VEUWADDR), (spirit.get_sram(veu.read_csr(veu.VEUWADDR)) & 0xFFFF) | \
																			((veu.read_output_buffer(0) & 0xFFFF) << 16))
								spirit.set_sram(veu.read_csr(veu.VEUWADDR) + 0x4,   ((veu.read_output_buffer(0) >> 16) & 0xFFFF) | \
																					((veu.read_output_buffer(1) >> 0) & 0xFFFF))
								spirit.set_sram(veu.read_csr(veu.VEUWADDR) + 0x8,   ((veu.read_output_buffer(1) >> 16) & 0xFFFF) | \
																					((veu.read_output_buffer(2) >> 0) & 0xFFFF))
								spirit.set_sram(veu.read_csr(veu.VEUWADDR) + 0xC,   ((veu.read_output_buffer(2) >> 16) & 0xFFFF) | \
																					((veu.read_output_buffer(3) >> 0) & 0xFFFF))
								spirit.set_sram(veu.read_csr(veu.VEUWADDR) + 0x10,  (spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0x10)) & 0xFFFF0000) | \
																					((veu.read_output_buffer(3) >> 16) & 0xFFFF))
							if (veu.read_csr(veu.VEUWADDR) & 0x3) == 0x3: # save low 24 bit
								spirit.set_sram(veu.read_csr(veu.VEUWADDR), (spirit.get_sram(veu.read_csr(veu.VEUWADDR)) & 0xFFFFFF) | \
																			((veu.read_output_buffer(0) & 0xFF) << 24))
								spirit.set_sram(veu.read_csr(veu.VEUWADDR) + 0x4,   ((veu.read_output_buffer(0) >> 8) & 0xFFFFFF) | \
																					((veu.read_output_buffer(1) >> 0) & 0xFF))
								spirit.set_sram(veu.read_csr(veu.VEUWADDR) + 0x8,   ((veu.read_output_buffer(1) >> 8) & 0xFFFFFF) | \
																					((veu.read_output_buffer(2) >> 0) & 0xFF))
								spirit.set_sram(veu.read_csr(veu.VEUWADDR) + 0xC,   ((veu.read_output_buffer(2) >> 8) & 0xFFFFFF) | \
																					((veu.read_output_buffer(3) >> 0) & 0xFF))
								spirit.set_sram(veu.read_csr(veu.VEUWADDR) + 0x10,  (spirit.get_sram(veu.read_csr(veu.VEUWADDR + 0x10)) & 0xFFFFFF00) | \
																					((veu.read_output_buffer(3) >> 8) & 0xFF))
							# vmac
							# vwredsum
							# vredsum
							if (((veu.read_csr(veu.VEUSTATUS) >> 19) & 0x1) or ((veu.read_csr(veu.VEUDEBUG) >> 19) & 0x1) or \
							((veu.read_csr(veu.VEUSTATUS) >> 16) & 0x1) or ((veu.read_csr(veu.VEUDEBUG) >> 16) & 0b1) or \
							((veu.read_csr(veu.VEUSTATUS) >> 17) & 0x1) or ((veu.read_csr(veu.VEUDEBUG) >> 17) & 0b1)):
								veu.write_csr(veu.VEUWADDR, veu.read_csr(veu.VEUWADDR))
								# print(hex(spirit.get_sram(0x400020d0)))
							elif (((veu.read_csr(veu.VEUSTATUS) >> 18) & 0x1) or ((veu.read_csr(veu.VEUDEBUG) >> 18) & 0b1)):
								mask_cnt = one_cnt_16bit(veu.psccu_mask & 0xFFFF)
								veu.write_csr(veu.VEUWADDR, veu.read_csr(veu.VEUWADDR) + mask_cnt)
							else:
								veu.write_csr(veu.VEUWADDR, veu.read_csr(veu.VEUWADDR) + 0x10)
						running = veu.read_csr(veu.VEUSTATUS)

					# 累计计算运行时间
					print(vector_len)
					running_time = vector_len // 128 * 3
					run_time.accumulate_veu_time(running_time)

					spirit.set_csr_bus_rdata(veu.get_csr_bus_rdata())
				elif (spirit.get_csr_bus_addr() & 0xf00) == 0x200:
					# 1、获取csr接口数据
					sau_en = spirit.get_csr_bus_we() | spirit.get_csr_bus_re()
					sau_operation = spirit.get_csr_bus_write_type()
					sau_addr = spirit.get_csr_bus_addr() & 0x00f
					sau_wdata1 = spirit.get_csr_bus_wdata1()
					sau_wdata2 = spirit.get_csr_bus_wdata2()
					# 2、写入 csr 缓存区
					if (sau_en==1 & sau_operation==1):
						sau.set_csr_bus(sau_en, sau_operation, sau_addr, sau_wdata2, sau_wdata1)
						# print(sau_en)
						# print((f"{((sau_wdata2 << 32) | sau_wdata1):064b}"))
						# print("csr_addr:"+str(sau_addr))
						# print("csr_operation:"+str(sau_operation))
						# print(sau.ins_cache,"\n")
					# 3、在csr_addr = 3时配置指令项, 也就是第三条指令时解析指令
					if (sau_en==1 & (sau_addr//2 == 2)):
						sau.set_sau()
						# print("ins decoding")
					# 4、解析出指令项 start==1 启动SAU
					if sau.ins.start == 1:
						# TODO： sram的分块映射，考虑多核地址
						# 这里对 spirit.sram 做一个虚拟映射，处理完后再映射回去，read_sram和write_sram可能会需要循环
						# sram_arrayd的格式是list[int32, int32, int32, ...]
						if(SAU_debuf_flag == True):
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
			if (spirit.trap == 1 or spirit.trap == 2):
				while 1:
					command = input(REDC + "wait your command:" + RESETC)
					if (command == 's' or command == 'step'):
						DEBUG_PRINT("exec step command!", GREENC)
						pc_value = hex(spirit.get_current_pc())[2:]
						step_debug(asm_file_path, pc_value)
						spirit.trap = 2
						break
					elif (command == 'c' or command == 'continue'):
						DEBUG_PRINT("exec continue command!", GREENC)
						spirit.trap = 0
						break
					elif (command == 'b' or command == 'break out'):
						DEBUG_PRINT("exect and calculation time!", GREENC)
						spirit.trap = 1
						break
					elif (command == 'vt' or command == 'verbosetrue'):
						spirit.verbose = True
						DEBUG_PRINT("set verbose True!", GREENC)
					elif (command == 'vf' or command == 'verbosefalse'):
						spirit.verbose = False
						DEBUG_PRINT("set verbose False!", GREENC)
					elif (command == 'e' or command == 'exit' or command == 'q' or command == 'quit'):
						DEBUG_PRINT("exit by python!\n", GREENC)
						exit()
					elif (command == 'g' or command == 'getsram'):
						DEBUG_PRINT("exec getsram command!", GREENC)
						while 1:
							str_addr = input("please input addr:")
							decimal_addr = int(str_addr, 16)
							debug_sram_data = spirit.get_sram(decimal_addr)
							DEBUG_PRINT("sram["+hex(decimal_addr & 0xFFFFFFFC)+"]: "+hex(debug_sram_data), GREENC)
							whetherBreak = input("exit or continue:")
							if (whetherBreak == "e" or whetherBreak == "exit" or whetherBreak == 'q' or whetherBreak == 'quit'):
								break
					else:
						DEBUG_PRINT("get a error command!\n", YELLOWC)
		break
	# 5、打印运行时间
	cpu_runtime = spirit.get_csr(1)
	run_time.accumulate_cpu_time(cpu_runtime)
	run_time.print_run_time()

if __name__ == "__main__":
	main()