#include "uart0.h"
#include <stdint.h>

void uart_write_reg(uart_t *uart, size_t reg, uint32_t val) {
	*(volatile uint32_t *)(uart->base_addr + reg) = val;
}

uint32_t uart_read_reg(uart_t *uart, size_t reg) {
	return *(volatile unsigned int *)(uart->base_addr + reg);
}

void uart_init(uart_t *uart) {
	uart_write_reg(uart, REG_TXCTRL, REG_TXCTRL_TXCNT | REG_TXCTRL_TXEN);
	uart_write_reg(uart, REG_RXCTRL, REG_RXCTRL_RXCNT | REG_RXCTRL_RXEN);
}

char uart_is_transmit_full(uart_t *uart) {
	unsigned long val = uart_read_reg(uart, REG_TXDATA);
	return !!(val & REG_TXDATA_FULL);
}


void uart_send_byte(uart_t *uart, char to_send) {
	// TODO Fix 0 writing issue
	while (1) {
		if (!uart_is_transmit_full(uart)) {
			uart_write_reg(uart, REG_TXDATA, to_send);
			break;
		}
	}
}

		
void uart_puts(uart_t *uart, char *str) {
	char *s = str;
	while (*s) {
		uart_send_byte(uart, *s);
		++s;
	}
}
