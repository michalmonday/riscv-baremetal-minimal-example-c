# Switched gcc with clang, added -target, changed -march and -mabi, added -mno-relax because it was in:
# https://github.com/CTSRD-CHERI/cheri-exercises/blob/master/tools/ccc

PROG_NAME 	= riscv-example
# RISCV64_GCC = riscv64-unknown-elf-gcc
# RISCV64_GCC = /home/michal/cheri/output/sdk/bin/clang
RISCV64_GCC = /home/michal/cheri/output/sdk/bin/riscv64-unknown-freebsd-clang
GCC_FLAGS	= -target riscv64 \
		  -march=rv64gcxcheri \
		  -mabi=l64pc128d \
		  -mno-relax \
		  -ffreestanding -static -mcmodel=medany \
		  -fvisibility=hidden -fno-builtin \
		  -nostdlib -nostartfiles

LD_FLAGS=-T example.ld

.PHONY: all clean qemu
all: $(PROG_NAME)

start.o: 
	$(RISCV64_GCC) $(GCC_FLAGS) -c start.s

main.o: main.c
	$(RISCV64_GCC) $(GCC_FLAGS) -c main.c

$(PROG_NAME): start.o main.o
	$(RISCV64_GCC) $(GCC_FLAGS) -o $@ start.o main.o $(LD_FLAGS)

clean:
	rm *.o $(PROG_NAME)

qemu:
	qemu-system-riscv64 -nographic -machine sifive_u -kernel $(PROG_NAME) -bios none
