swig_cpu:
	${SWIG} -c++ -python Spirit.i
	${CC} -c -fpic Spirit.cpp Spirit_wrap.cxx -I${PYTHONH}
	${CC} -shared ${MACOS_FLAG} Spirit.o Spirit_wrap.o -o _Spirit_Emulator.so -lstdc++

clean_cpu:
	rm -rf *.cxx *_Emulator.py *.o *.so __pycache__

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
