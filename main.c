#include "base.h"
#include "uart0.h"
#include "pdma.h"
#include <stddef.h>
#include <stdint.h>

char g_uninitilised_chans = 0,
	 g_write_errs = 0;

int fill_ram(void *start, size_t write_size, char *pattern, size_t chunk_size, int nchannels) {
	if (nchannels > 4 || nchannels < 1) return -1; // TODO return code

	size_t chunks_to_fill = write_size / chunk_size; // if write_size % 256 != 0 ignore
	pdma_chann_t channels[4];
	size_t channel_offset = chunks_to_fill / nchannels;
	size_t chunks_for_channel[4] = {};

	for (int i = 0; i < nchannels; i++) {
		channels[i] = pdma_init(PDMA_BASE_ADDR, i);

		pdma_chann_t *tmp = &channels[i];
		if (!pdma_claim(tmp)) {
			--nchannels;
			g_uninitilised_chans = 1;
			continue;  // TODO warning
		}

		tmp->next_config.conf = PDMA_FULL_SPEED;
		tmp->next_config.nbytes = chunk_size;
		tmp->next_config.read_ptr = (uint64_t)pattern;
		tmp->next_config.write_ptr = (uint64_t)start + i*channel_offset;

		chunks_for_channel[i] = channel_offset;
	}

	if (nchannels < 1) return -1; // TODO return code

	// if has 1-3 chunks of reminder, give it to the last channel
	if (chunks_to_fill % nchannels != 0) {
		chunks_for_channel[nchannels-1] += chunks_to_fill - (chunks_to_fill * nchannels);
	}

	while (chunks_for_channel[0] || chunks_for_channel[1] || chunks_for_channel[2] || chunks_for_channel[3]) {
		for (int i = 0; i < nchannels; ++i) {
			if (!chunks_for_channel[i]) continue;

			pdma_control_get(&channels[i]);
			if (channels[i].control.run) continue;

			// TODO handle control.error
			if (channels[i].control.error) g_write_errs = 1;

			channels[i].control.run = 1;
			pdma_config_write_next(&channels[i]);
			pdma_control_write(&channels[i]);

			chunks_for_channel[i]--;

			channels[i].next_config.write_ptr += chunk_size;
		}
	} 

	return 0;
}

int g_has_errors = 0;

void check_ram(void* start, size_t nbytes, size_t chunk_size, uint64_t pattern) {
	nbytes -= nbytes % chunk_size;
	
	uint64_t *end = (uint64_t *)(start + nbytes);
	for (uint64_t *ptr = start; ptr != end; ++ptr) {
		if ((*ptr & ~pattern) != 0) {
			// TODO analyze this 8 bytes
			g_has_errors = 1;
		}
	}
}


int main() {
	uart_t uart = { (void*)UART0_BASE_ADDR };
	uart_init(&uart);

#define print(str) uart_puts(&uart, str)

	// assume filling RAM with 0b01010101 (0x55 char'U') bytes
	// char pattern[256] = "U1UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU"
	//                     "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU"
	// 				    "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU"
	// 				    "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU";
	char pattern[64] = "U1UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU";


	print("Programm start\n");
	int res = fill_ram(RAM_ORIGIN, 4*128, pattern, 64, 4);

	if (res != 0) {
		print("error running ram_fill function\n");
	} else {
		print("ram filled with pattern\n");
	}
	if (g_uninitilised_chans) print("some channels was unclaimed\n");
	if (g_write_errs) print("occured errors writing some chanks\n");
	
	check_ram(RAM_ORIGIN, 4*128, 64, 0x5555555555555555);
	print("check result is: ");

	if (g_has_errors) {
		print("'error'");
	} else {
		print("'ok'");
	}
	print("!\n");

#undef print

	return 0;
}
