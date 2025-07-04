swig_uart:
	${SWIG} -c++ -python UART.i
	${CC} -c -fpic UART.cpp UART_wrap.cxx -I${PYTHONH}
	${CC} -shared ${MACOS_FLAG} ../Device/Device.o UART.o UART_wrap.o -o _UART_Emulator.so -lstdc++

clean_uart:
	rm -rf *.cxx *.py *.o *.so __pycache__

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
