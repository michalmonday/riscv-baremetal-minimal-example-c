
INCLUDE_DIR = ../include
# ALGORITHMS_DIR = ./algorithms

PROG_NAME ?= flute_halting_test
PROG_FULL_PATH = $(PROG_NAME)
BIN_PATH = .
# BIN_PATH = $(ALGORITHMS_DIR)/bin
PROG_BIN_FULL_PATH = $(BIN_PATH)/$(PROG_NAME)

TOOLCHAIN = riscv64-unknown-elf

GCC = $(TOOLCHAIN)-gcc
OBDUMP = $(TOOLCHAIN)-objdump
OBJCOPY = $(TOOLCHAIN)-objcopy

POSTCOMPILE_DEST_DIR = /mnt/hgfs/shared\ ubuntu\ vm/FORENSIC

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
	$(GCC) $(GCC_FLAGS) -c $(PROG_NAME).c -o $(PROG_NAME).o
	$(GCC) $(GCC_FLAGS) -o $@ start.o $(PROG_NAME).o $(shell find . ! -name "start.o" ! -wholename "$(PROG_FULL_PATH).o" ! -name "$(PROG_NAME).o" -name "*.o") $(LD_FLAGS)
# $(GCC) $(GCC_FLAGS) -o $@ start.o $(filter-out start.o, $(wildcard *.o)) $(LD_FLAGS)

postcompile: $(PROG_BIN_FULL_PATH)
	$(OBDUMP) -sDz $(PROG_BIN_FULL_PATH) > $(PROG_BIN_FULL_PATH).dump
	$(OBJCOPY) -O binary $(PROG_BIN_FULL_PATH) $(PROG_BIN_FULL_PATH).bin
	python3 parse_objdump.py $(PROG_BIN_FULL_PATH).dump -o $(PROG_BIN_FULL_PATH)_dump.json --ignore-prefix __ --ignore-substring .
	echo "Done!"
# cp $(PROG_BIN_FULL_PATH).dump $(POSTCOMPILE_DEST_DIR)/objdump
# cp $(PROG_BIN_FULL_PATH).bin $(POSTCOMPILE_DEST_DIR)
# cp $(PROG_BIN_FULL_PATH)_dump.json $(POSTCOMPILE_DEST_DIR)

clean: clean_objects
	rm -f $(PROG_BIN_FULL_PATH) $(PROG_BIN_FULL_PATH).dump $(PROG_BIN_FULL_PATH).bin 2> /dev/null
	rm -f $(shell find . -name "*.dump") 2> /dev/null
	rm -f $(shell find . -name "*.bin") 2> /dev/null
	rm -f $(BIN_PATH)/*.o 2> /dev/null

clean_objects:
	rm -f $(shell find . -name "*.o") 2> /dev/null
