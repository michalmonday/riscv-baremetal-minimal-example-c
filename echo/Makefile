PROG_NAME 	= echo

INCLUDE_DIR = ../include
ALGORITHMS_DIR = algorithms

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
			  -I$(ALGORITHMS_DIR)

LD_FLAGS=-T example_original.ld

.PHONY: all clean
all: postcompile

start.o: 
	$(GCC) $(GCC_FLAGS) -c startup.c -o start.o
#$(GCC) $(GCC_FLAGS) -c start_original.s -o start.o

includes: $(INCLUDE_DIR)/*.c $(INCLUDE_DIR)/*.h 
	$(GCC) $(GCC_FLAGS) -c $(INCLUDE_DIR)/*.c 

$(PROG_NAME).o: $(PROG_NAME).c 
	$(GCC) $(GCC_FLAGS) -c $(PROG_NAME).c

$(PROG_NAME): start.o $(PROG_NAME).o includes 
	# $(GCC) $(GCC_FLAGS) -o $@ start.o $(filter-out start.o, $(wildcard *.o)) $(LD_FLAGS)
	$(GCC) $(GCC_FLAGS) -o $@ start.o $(PROG_NAME).o $(shell find . ! -name "start.o" ! -name "$(PROG_NAME).o" -name "*.o") $(LD_FLAGS)

postcompile: $(PROG_NAME)
	$(OBDUMP) -sSD $(PROG_NAME) > $(PROG_NAME).dump
	$(OBJCOPY) -O binary $(PROG_NAME) $(PROG_NAME).bin
	cp $(PROG_NAME).bin /mnt/hgfs/shared\ ubuntu\ vm
	cp $(PROG_NAME).dump /mnt/hgfs/shared\ ubuntu\ vm
	rm *.o
	echo "Done!"

clean:
	rm *.o $(PROG_NAME) $(PROG_NAME).dump $(PROG_NAME).bin

