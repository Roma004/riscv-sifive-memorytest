#include "uart0.h"
#include <stdint.h>

#pragma GCC push_options
#pragma GCC optimize("O0")

void uart_write_reg(uart_t *uart, size_t reg, uint32_t val) {
    volatile uint32_t *reg_ptr = uart->base_addr + reg;
    *reg_ptr = val & 0x000000FF;
}

uint32_t uart_read_reg(uart_t *uart, size_t reg) {
    return *(uint32_t *)(uart->base_addr + reg);
}

char uart_is_transmit_full(uart_t *uart) {
    volatile uint32_t val = uart_read_reg(uart, REG_TXDATA);
    volatile char res = (val & REG_TXDATA_FULL) != 0;
    return res;
}

void uart_putc(uart_t *uart, char to_send) {
    while (uart_is_transmit_full(uart)) { asm volatile("nop;"); }
    uart_write_reg(uart, REG_TXDATA, to_send);
}

#pragma GCC pop_options

uart_t uart_init(void *base_addr) {
    uart_t res = {.base_addr = base_addr};

    uart_write_reg(&res, REG_TXCTRL, REG_TXCTRL_TXCNT | REG_TXCTRL_TXEN);
    uart_write_reg(&res, REG_RXCTRL, REG_RXCTRL_RXCNT | REG_RXCTRL_RXEN);

    // crutch for fixing first printig symbol issues
    uart_putc(&res, 0);

    return res;
}

void uart_puts(uart_t *uart, char *str) {
    for (char *s = str; *s != 0; ++s) { uart_putc(uart, *s); }
}
