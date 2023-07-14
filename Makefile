NBIT = 64

RISCV_CROSS_BIN = ../riscv-compiler$(NBIT)/bin

CC = $(RISCV_CROSS_BIN)/riscv$(NBIT)-unknown-elf-gcc
GDB = $(RISCV_CROSS_BIN)/riscv$(NBIT)-unknown-elf-gdb


INCLUDE_DIR = ./include
SRC_DIR = ./src
OUTPUT = ./output/asd

LD_FILE = $(NBIT)bit-main.ld
ASM_FILES = crt0.s
C_FILES = main.c $(SRC_DIR)/*.c

C_FALGS = -ffreestanding -nostartfiles -nodefaultlibs -mcmodel=medany -O0

ifeq ($(NBIT),32)
	C_FALGS += -mabi=ilp32
else
	C_FALGS += -mabi=lp64
endif

LD_FLAGS = -Wl,-T,$(LD_FILE) -Wl,--gc-sections

DEBUG_PORT = 1234

QEMU = qemu-system-riscv$(NBIT)
QEMU_DEBUG_OPTIONS = -gdb tcp::$(DEBUG_PORT) -S
QEMU_FLAGS = -nographic -machine sifive_u -bios none -kernel $(OUTPUT) -m 128M


.PHONY: build
build:
	$(CC) $(C_FALGS) $(LD_FLAGS) -I$(INCLUDE_DIR) $(ASM_FILES) $(C_FILES) -o $(OUTPUT)


.PHONY: build-debug
build-debug:
	$(CC) $(C_FALGS) $(LD_FLAGS) -I$(INCLUDE_DIR) $(ASM_FILES) $(C_FILES) -o $(OUTPUT) -g3


.PHONY: qemu
qemu:
	$(QEMU) $(QEMU_FLAGS)


.PHONY: qemu-debug
qemu-debug:
	$(QEMU) $(QEMU_FLAGS) $(QEMU_DEBUG_OPTIONS) 


.PHONY: debug
debug:
	$(GDB) $(OUTPUT) -ex "target remote localhost:$(DEBUG_PORT)" 