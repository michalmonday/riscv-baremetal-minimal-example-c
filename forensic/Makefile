
INCLUDE_DIR = ../include
# ALGORITHMS_DIR = ./algorithms

PROG_NAME ?= forensic
PROG_FULL_PATH = $(PROG_NAME)
BIN_PATH = .
# BIN_PATH = $(ALGORITHMS_DIR)/bin
PROG_BIN_FULL_PATH = $(BIN_PATH)/$(PROG_NAME)

TOOLCHAIN = riscv64-unknown-elf

GCC = $(TOOLCHAIN)-gcc
OBDUMP = $(TOOLCHAIN)-objdump
OBJCOPY = $(TOOLCHAIN)-objcopy

# # GCC_FLAGS	= -ffreestanding -march=rv64gc -mabi=lp64 -static -mcmodel=medany
# GCC_FLAGS	= -ffreestanding -march=rv64g -mabi=lp64 -static -mcmodel=medany \
# 			  -fvisibility=hidden -fno-builtin \
# 			  -nostdlib -nostartfiles \
# 			  -I$(INCLUDE_DIR) \
# 			  -I$(ALGORITHMS_DIR)

# riscv-gnu-toolchain was compiled with the following flags (in "./configure" command):
# --with-arch=rv64g --with-arch=rv64gc
# --with-abi=lp64d
# --with-cmodel=medany
# Without medany model argument, the compilation fails with errors like:
#     "relocation truncated to fit: R_RISCV_HI20 against `.LC0'"
GCC_FLAGS	= -march=rv64g -mabi=lp64d -static \
			  -fvisibility=hidden \
			  -nostartfiles \
			  -mcmodel=medany \
			  -I$(INCLUDE_DIR) \
			  -lm \
			  -Os
# -lm is needed for math.h used in aiiftr.c
# -Os optimizes for size (it seems to not include ".o" object files or their contents that are not used in the final binary despite supplying them to the compiler/linker)

# # if SINGLE_ALGORITHM=true then add -DSINGLE_ALGORITHM to GCC_FLAGS
# SINGLE_ALGORITHM ?= false
# ifeq ($(SINGLE_ALGORITHM), true)
# 	GCC_FLAGS += -DSINGLE_ALGORITHM=true
# endif 

LD_FLAGS=-T example_original.ld \
	-lm # needed for math.h used in aiiftr.c

.PHONY: all clean
all: postcompile clean_objects

start.o: 
	$(GCC) $(GCC_FLAGS) -c startup.c -o start.o
#$(GCC) $(GCC_FLAGS) -c start_original.s -o start.o

includes: $(INCLUDE_DIR)/*.c $(INCLUDE_DIR)/*.h 
	$(GCC) $(GCC_FLAGS) -c $(INCLUDE_DIR)/*.c

# algorithms: $(ALGORITHMS_DIR)/*.c $(ALGORITHMS_DIR)/*.h
# 	$(GCC) $(GCC_FLAGS) -c $(ALGORITHMS_DIR)/*.c

# $(PROG_FULL_PATH).o: $(PROG_FULL_PATH).c $(PROG_FULL_PATH).h 
# 	$(GCC) $(GCC_FLAGS) -c $(PROG_FULL_PATH).c -o $@

$(PROG_BIN_FULL_PATH): includes start.o 
	$(GCC) $(GCC_FLAGS) -c main.c -o main.o
	$(GCC) $(GCC_FLAGS) -o $@ start.o main.o $(shell find . ! -name "start.o" ! -wholename "$(PROG_FULL_PATH).o" ! -name "main.o" -name "*.o") $(LD_FLAGS)
# $(GCC) $(GCC_FLAGS) -o $@ start.o $(filter-out start.o, $(wildcard *.o)) $(LD_FLAGS)

postcompile: $(PROG_BIN_FULL_PATH)
	$(OBDUMP) -sDz $(PROG_BIN_FULL_PATH) > $(PROG_BIN_FULL_PATH).dump
	$(OBJCOPY) -O binary $(PROG_BIN_FULL_PATH) $(PROG_BIN_FULL_PATH).bin
	python3 parse_objdump.py $(PROG_BIN_FULL_PATH).dump -o $(PROG_BIN_FULL_PATH)_dump.json --ignore-prefix __ --ignore-substring .
	cp $(PROG_BIN_FULL_PATH).dump /mnt/hgfs/shared\ ubuntu\ vm/benchmark_programs/objdump
	cp $(PROG_BIN_FULL_PATH).bin /mnt/hgfs/shared\ ubuntu\ vm/benchmark_programs
	cp $(PROG_BIN_FULL_PATH)_dump.json /mnt/hgfs/shared\ ubuntu\ vm/benchmark_programs
	echo "Done!"

clean: clean_objects
	rm -f $(PROG_BIN_FULL_PATH) $(PROG_BIN_FULL_PATH).dump $(PROG_BIN_FULL_PATH).bin 2> /dev/null
	rm -f $(shell find . -name "*.dump") 2> /dev/null
	rm -f $(shell find . -name "*.bin") 2> /dev/null
	rm -f $(BIN_PATH)/*.o 2> /dev/null

clean_objects:
	rm -f $(shell find . -name "*.o") 2> /dev/null

# make clean && make multiple_algorithms MULTIPLE_ALGORITHMS=true PROG_NAME=main --always-make
multiple_algorithms: includes start.o 
	$(GCC) $(GCC_FLAGS) -c main.c -o multiple_algorithms.o
	$(GCC) $(GCC_FLAGS) -o multiple_algorithms start.o multiple_algorithms.o  $(shell find . ! -name "main.o" ! -name "multiple_algorithms.o" ! -name "start.o" -name "*.o") $(LD_FLAGS)
#riscv64-unknown-elf-gcc -march=rv64g -mabi=lp64d -static -fvisibility=hidden -nostartfiles -mcmodel=medany -I../include -I./algorithms -lm -DRANDOM_FUNCTION_GENERATOR -o main main.o ./utils_flute.o ./eembc_adaptation.o ./bitmnp.o ./uart_gpio.o ./rspeed.o ./start.o ./sensors.o ./puwmod.o ./uart_pynq.o ./a2time.o ./ttsprk.o ./syscalls.o -T example_original.ld -lm 
	$(OBDUMP) -sSD multiple_algorithms > multiple_algorithms.dump
	$(OBJCOPY) -O binary multiple_algorithms multiple_algorithms.bin
	cp multiple_algorithms.dump /mnt/hgfs/shared\ ubuntu\ vm/benchmark_programs/objdump
	cp multiple_algorithms.bin /mnt/hgfs/shared\ ubuntu\ vm/benchmark_programs
	echo "Done!"
