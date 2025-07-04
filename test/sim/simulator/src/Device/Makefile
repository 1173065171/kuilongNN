swig_device:
	${SWIG} -c++ -python Device.i
	${CC} -c -fpic Device.cpp Device_wrap.cxx -I${PYTHONH}
	${CC} -shared ${MACOS_FLAG} Device.o Device_wrap.o -o _Device_Emulator.so -lstdc++

clean_device:
	rm -rf *.cxx *.py *.o *.so __pycache__

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
