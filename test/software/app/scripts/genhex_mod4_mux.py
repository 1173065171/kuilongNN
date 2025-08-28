import os
import shutil
import time
import argparse

def convert_32bit_to_512bit(input_file, output_file, Mux=16, update = None):
	"""### convert 32bit hex-data to 512bit hex-data in one line, adapt to sram-mux-mem queue

	### Args:
		input_file (str): input file
		output_file (str): output file
		Mux (int, optional): mux of sram-mux-mem queue. Defaults to 16.
	"""
	with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
		# read input file
		lines = infile.readlines()
		num_lines = len(lines)

		# 初始化存储器
		mem = {}  # 模拟存储器
		writeEnable = [1] * 32  # 全部写使能

		for i in range(num_lines//Mux):
			row = int('0' * 512, 2)  # 初始化512位数据为全1
			for j in range(Mux):
				# ready
				row_address = i
				mux_address = j
				lines_str = lines[i*Mux+j]

				D_int = [int(bit) for bit in bin(int(lines_str, 16))[2:].zfill(32)]

				# 将 writeEnable 和 D_int 扩展为带 15'b0 填充的位串
				row_mask = 0
				new_data = 0
				for k in range(32):
					row_mask |= (writeEnable[k] << (k * 16 + mux_address))
					new_data |= (D_int[31-k] << (k * 16 + mux_address))
				
				# 更新行数据
				row = (row & ~row_mask) | (row_mask & (~row_mask | new_data))

				if update != None:
					# 更新存储器, 数据转换为16进制字符串
					mem[row_address] = f'{row:0128x}'

			outfile.write(f'{row:0128x}' + '\n')


def main():
	split_file = [f'memory_mod4_{i}.hex' for i in range(4)]
	output_file = [f'memory_{i}.hex' for i in range(4)]
	for i in range(4):
		convert_32bit_to_512bit(split_file[i], output_file[i], Mux=16) 


if __name__ == "__main__":

	main()
