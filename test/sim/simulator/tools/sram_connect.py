

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