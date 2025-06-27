
all:
	mkdir -p out
	python3 main.py 2>&1 | tee record.log

build:
	cd ./src/Spirit && make all -B
	cd ./src/Device && make all -B
	cd ./src/UART && make all -B
	cd ./src/VEU && make all -B

# remove \x1b
r1b:
	@python3 remove_ansi_escapes.py

clean_log:
	rm -vrf *.log


clean_out:
	rm -rf ./out/
	mkdir out


clean:
	@rm -rf *.log *_output.txt
	@rm -rf ./out/
