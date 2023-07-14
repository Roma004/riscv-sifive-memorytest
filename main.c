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

	// char asd[5] = {sizeof(char)+'0', sizeof(short)+'0', sizeof(int)+'0', sizeof(long)+'0', 0};
	// uart_puts(&uart, asd);

	pdma_conf_t next_conf = {0, 0, 6, 0};

	pdma_chain_t pdma = {
		/* base addr */ PDMA_BASE_ADDR,
		/* chain id  */ 1,
		/* next conf */ next_conf,
		/* nbytes    */ 36,
		/* dest      */ res,
		/* src       */ s
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
