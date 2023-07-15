#include "base.h"
#include "uart0.h"
#include "pdma.h"
#include <stdint.h>


int main() {
	uart_t uart = { (void*)UART0_BASE_ADDR };
	uart_init(&uart);

#define print(str) uart_puts(&uart, str)

	// assume filling RAM with 0b01010101 (0x55 char'U') bytes
	// will copy 64 bytes per transfer.
	char pattern[65] = "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU";
	char buffer[65] = {};

	pdma_conf_t next_config = {
		.conf      = PDMA_FULL_SPEED,
		.nbytes    = 64,
		.read_ptr  = (uint64_t)pattern,
		.write_ptr = (uint64_t)buffer
	};

	pdma_chain_t pdma;

	char success = 0;
	for (int chain_id = 0; chain_id < 4; ++chain_id) {
		pdma = pdma_init(PDMA_BASE_ADDR, chain_id);
		if (!pdma_claim(&pdma)) {
			print("Unable to claim pdma, continue with next channel\n");
			continue;
		}
		success = 1;
		break;
	}
	if (!success) {
		print("Unable to claim pdma (all channels in use)\n");
		return 1;
	}

	print("Channel claimed. Configuring transfer\n");

	pdma.next_config = next_config;

	pdma_run(&pdma);    // start bytes transfer

	print("PDMA transfer start\n");

	pdma_unclaim(&pdma);  // wait until transfer ends, and unclaim channel

	print("PDMA transfer end. result of copy:\n");

	uart_puts(&uart, buffer);

	print("end.\n");

#undef print

	return 0;
}
