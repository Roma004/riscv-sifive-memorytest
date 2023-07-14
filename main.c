#include "base.h"
#include "uart0.h"
#include "pdma.h"


int main() {
	uart_t uart = { (void*)UART0_BASE_ADDR };
	uart_init(&uart);

#define print(str) uart_puts(&uart, str "\n")

	char s[36] = "Awcrnf89ny4-2irmgh2-f7834sdasdasd!\n";
	char res[36] = {};

	pdma_conf_t next_conf = {0, 0, 6, 0};

	pdma_chain_t pdma = {
		/* base addr */ PDMA_BASE_ADDR,
		/* chain id  */ 1,
		/* next conf */ next_conf,
		/* nbytes    */ 36,
		/* dest      */ res,
		/* src       */ s
	};

	if (pdma_claim(&pdma)) {   // reserve pdma chain 0
		print("PDMA claimed");
	} else {
		print("Unable to claim PDMA. exiting");
		return 1;
	}

	
	pdma_run(&pdma);    // start bytes transfer

	print("PDMA transfer start");

	pdma_wait_transfer(&pdma);  // wait until transfer ends, to check the result

	print("PDMA transfer end");

	// just write string we copied in previous step to uart
	print("string:");
	uart_puts(&uart, res);

	print("end.");

	// uart_puts(&uart, s);

#undef print

	return 0;
}
