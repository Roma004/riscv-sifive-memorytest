#ifndef UART0_H
#define UART0_H

/* registers in the controller and their offsets */
#define REG_TXDATA (0 * 4) /* transmit data register */ 
#define REG_RXDATA (1 * 4) /* receive data register */
#define REG_TXCTRL (2 * 4) /* transmit control register */
#define REG_RXCTRL (3 * 4) /* receive control register */
#define REG_IE     (4 * 4) /* UART interrupt enable */
#define REG_IP     (5 * 4) /* UART interrupt pending */
#define REG_DIV    (6 * 4) /* baud rate divisor */

/* individual control bits */
#define REG_IE_TXWM      (1 << 0)  /* transmit watermark interrupt enable */
#define REG_IE_RXWM      (1 << 1)  /* receive watermark interrupt enable */

#define REG_TXCTRL_TXEN  (1 << 0)  /* transmit enable */
#define REG_RXCTRL_RXEN  (1 << 0)  /* receive enable */

#define REG_TXCTRL_TXCNT (1 << 16) /* tx FIFO irq watermark level of 1 */
#define REG_RXCTRL_RXCNT (6 << 16) /* rx FIFO irq watermark level of 6 */

#define REG_TXDATA_FULL  (1 << 31)
#define REG_RXDATA_EMPTY (1 << 31)


// max tries of checking for a byte to arrive
#define LOOP_MAX 1000
#define REG_TOTAL_SIZE (7 * 4) // 7 x 32-bit registers

#include "base.h"

typedef struct UART {
	void *base_addr;
} uart_t;


void uart_init(uart_t *uart);
void uart_write_reg(uart_t *uart, size_t reg, unsigned long val);
unsigned long uart_read_reg(uart_t *uart, size_t reg);
char uart_is_transmit_full(uart_t *uart);
void uart_send_byte(uart_t *uart, char to_send);
void uart_puts(uart_t *uart, char *str);


#endif // UART0_H