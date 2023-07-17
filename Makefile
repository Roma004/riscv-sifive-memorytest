RISCV_CROSS_BIN = ../riscv-gcc-crosscompiler64/bin

CC = $(RISCV_CROSS_BIN)/riscv64-unknown-elf-gcc
GDB = $(RISCV_CROSS_BIN)/riscv64-unknown-elf-gdb


INCLUDE_DIR = ./include
SRC_DIR = ./src
OUTPUT = ./output/asd

LD_FILE = 64bit-main.ld
ASM_FILES = crt0.s
C_FILES = main.c $(SRC_DIR)/*.c

C_FALGS = -ffreestanding -nostartfiles -mcmodel=medany -march=rv64ima -mabi=lp64

LD_FLAGS = -Wl,-T,$(LD_FILE) -Wl,--gc-sections

DEBUG_PORT = 1234

QEMU = qemu-system-riscv64
QEMU_DEBUG_OPTIONS = -gdb tcp::$(DEBUG_PORT) -S
QEMU_FLAGS = -nographic -machine sifive_u -bios none -kernel $(OUTPUT) -m 128M


.PHONY: build
build:
	$(CC) $(C_FALGS) $(LD_FLAGS) -I$(INCLUDE_DIR) $(ASM_FILES) $(C_FILES) -o $(OUTPUT) -O2


.PHONY: build-debug
build-debug:
	$(CC) $(C_FALGS) $(LD_FLAGS) -I$(INCLUDE_DIR) $(ASM_FILES) $(C_FILES) -o $(OUTPUT) -g3 -O2


.PHONY: qemu
qemu:
	$(QEMU) $(QEMU_FLAGS)


.PHONY: qemu-debug
qemu-debug:
	$(QEMU) $(QEMU_FLAGS) $(QEMU_DEBUG_OPTIONS) 


.PHONY: debug
debug:
	$(GDB) $(OUTPUT) -ex "target remote localhost:$(DEBUG_PORT)" 