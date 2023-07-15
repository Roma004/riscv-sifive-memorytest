#include "base.h"
#include "uart0.h"
#include "pdma.h"
#include <stdint.h>


int main() {
	uart_t uart = { (void*)UART0_BASE_ADDR };
	uart_init(&uart);

#define print(str) uart_puts(&uart, str)

	char s[36] = "Awcrnf89ny4-2irmgh2-f7834sdasdasd!\n";
	char res[36] = {};

	uint32_t next_conf = 0xFF000008;

	pdma_chain_t pdma = {
		.base_addr      = PDMA_BASE_ADDR,
		.chan_id        = 1,
		.next_conf      = next_conf,
		.next_nbytes    = 36,
		.next_write_ptr = res,
		.next_read_ptr  = s
	};

	print(s);

	if (pdma_claim(&pdma)) {   // reserve pdma chain 0
		print("PDMA claimed\n");
	} else {
		print("Unable to claim PDMA. exiting\n");
		return 1;
	}

	
	pdma_run(&pdma);    // start bytes transfer

	print("PDMA transfer start\n");

	pdma_wait_transfer(&pdma);  // wait until transfer ends, to check the result

	print("PDMA transfer end\n");

	// just write string we copied in previous step to uart
	print("string:\n");
	uart_puts(&uart, res);

	print("end.\n");

	// uart_puts(&uart, s);

#undef print

	return 0;
}
