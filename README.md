# Used Resources

* [guide of setting up environment for risc-v + UART](https://twilco.github.io/riscv-from-scratch/2019/04/27/riscv-from-scratch-2.html)
* [simle lib in rust, implementing UART driver](https://github.com/diodesign/mmio_sifive_uart/blob/main/src/lib.rs)
* [RISC-V cross compiler (elf ubuntu gcc)](https://github.com/riscv-collab/riscv-gnu-toolchain/releases/tag/2023.07.07)
* [sifive official sdk (metal drivers)](https://github.com/sifive/freedom-metal/tree/1cec4a23a7ed7350db79a392be65acd51acd5412)

# Potential needed (area for usefull links)

https://www.kernelconfig.io/config_sf_pdma (dma driver imp for linux sifive)

https://starfivetech.com/uploads/fu540-c000-manual-v1p4.pdf (fu540-c000 specs -- section 12)

# Настройка проекта

Используется вот этот Компилятор:

[RISC-V cross compiler (elf ubuntu gcc)](https://github.com/riscv-collab/riscv-gnu-toolchain/releases/tag/2023.07.07)

Для сборки/запуска надо изменить переменную RISCV_CROSS_BIN в Makefile (указать путь установки кросскомпилятора)

```
make biuld # сборка
make build-debug # сборка с отладочной информацией
make qemu # запуск в qemu 
make qemu-debug # запуск в qemu в режиме отладки
make debug # запуск gdb для подключения к активной сессии qemu
```
