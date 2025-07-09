class SimulatorTime():
	"""
	A class to measure the execution time of a function.
	"""

	def __init__(self):
		self.cpu_running_time = 0
		self.veu_running_time = 0
		self.sau_running_time = 0

	def accumulate_cpu_time(self, time):
		"""
		Accumulate the CPU running time.
		"""
		self.cpu_running_time += time

	def accumulate_veu_time(self, time):
		"""
		Accumulate the VEU running time.
		"""
		self.veu_running_time += time
	
	def accumulate_sau_time(self, time):
		"""
		Accumulate the SAU running time.
		"""
		self.sau_running_time += time

	def print_run_time(self):
		"""
		Print the VEU and SAU running time.
		"""
		print("VEU running time: ", self.veu_running_time)
		print("SAU running time: ", self.sau_running_time)
		print("CPU running time: ", self.cpu_running_time)
		
		print("Total running time: ", self.cpu_running_time + self.veu_running_time + self.sau_running_time)