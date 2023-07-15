#ifndef BASE_H
#define BASE_H

#include "stdint.h"
#include "stddef.h"
#include <stdlib.h>
#include <string.h>


#define GET_B(addr) (*(volatile uint8_t *)(addr))
#define GET_W(addr) (*(volatile uint32_t *)(addr))
#define GET_DW(addr) (*(volatile uint64_t *)(addr))

typedef enum fault_t {
	ok = 0,
	size_too_small,
	tx_not_empty,
	data_not_ready
} fault_t;

// defined in LD script
extern volatile unsigned char *__uart_base_addr;
extern volatile unsigned char *__pdma_base_addr;

#define UART0_BASE_ADDR &__uart_base_addr
#define PDMA_BASE_ADDR &__pdma_base_addr


#endif