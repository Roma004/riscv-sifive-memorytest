#include "base.h"
#include "uart0.h"
#include "pdma.h"
#include <stddef.h>
#include <stdint.h>
#include "bitarray.h"

void addr_to_string(char *str, size_t addr) {
	char *s = str;
	char buf[16] = {};
	*s++ = '0'; *s++ = 'x';

	size_t val = addr;
	int i = 0;
	while (val) {
		buf[i++] = val % 16;
		val /= 16;
	}
	for (int i = 15; i > 0; --i) s[15-i] = buf[i];
}

void print_ram_info(
	uart_t *uart,
	bitarray_t arr,
	size_t nchunks,
	unsigned int chunks_per_line,
	void *start_addres,
	size_t chunk_size
) {
	if (chunks_per_line > 128) chunks_per_line = 128;
	if (chunks_per_line < 32) chunks_per_line = 32;

	int lines_total = (nchunks + chunks_per_line - 1) / chunks_per_line;

	for (int line_n = 0; line_n < lines_total; line_n++) {
		char line_buffer[151];
		int i = 0;
		line_buffer[i++] = '|';
		for (int k = 0; k < chunks_per_line; ++k) {
			char val = bitarray_get(arr, k);

			if (val < 0)       line_buffer[i++] = ' ';
			else if (val == 0) line_buffer[i++] = '-';
			else               line_buffer[i++] = '+';
		}
		line_buffer[i++] = '|'; line_buffer[i++] = ' ';
		addr_to_string(line_buffer + i, (size_t)start_addres + chunks_per_line*chunk_size);
		line_buffer[i++] = '\n';
		line_buffer[i++] = '\0';
		uart_puts(uart, line_buffer);
	}
}


int fill_ram(void *start, size_t write_size, char *pattern, size_t chunk_size, int nchannels, bitarray_t chunks_map) {
	if (nchannels > 4) nchannels = 4;
	if (nchannels < 1) nchannels = 1;

	size_t chunks_to_fill = write_size / chunk_size; // if write_size % 256 != 0 ignore
	pdma_chann_t channels[4];
	size_t channel_offset = chunks_to_fill / nchannels;
	size_t chunks_for_channel[4] = {};

	for (int i = 0; i < nchannels; i++) {
		channels[i] = pdma_init(PDMA_BASE_ADDR, i);

		pdma_chann_t *tmp = &channels[i];
		if (!pdma_claim(tmp)) {
			--nchannels;
			continue;
		}

		tmp->next_config.conf = PDMA_FULL_SPEED;
		tmp->next_config.nbytes = chunk_size;
		tmp->next_config.read_ptr = (uint64_t)pattern;
		tmp->next_config.write_ptr = (uint64_t)start + i*channel_offset;

		chunks_for_channel[i] = channel_offset;
	}

	if (nchannels < 1) return -1;

	// if has 1-3 chunks of reminder, give it to the last channel
	if (chunks_to_fill % nchannels != 0) {
		chunks_for_channel[nchannels-1] += chunks_to_fill - (chunks_to_fill * nchannels);
	}

	while (chunks_for_channel[0] || chunks_for_channel[1] || chunks_for_channel[2] || chunks_for_channel[3]) {
		for (int i = 0; i < nchannels; ++i) {
			if (!chunks_for_channel[i]) continue;

			pdma_control_get(&channels[i]);
			if (channels[i].control.run) continue;

			bitarray_set(chunks_map, channel_offset - chunks_for_channel[i], !channels[i].control.error);

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

#define KBYTE 1024
#define MBYTE KBYTE*KBYTE


#define WRITE_CHUNK_SIZE 64
#define READ_CHUNK_SIZE 8

// 1M reserved for code stack
#define RAM_LENGTH 127*MBYTE
#define N_WRITE_CHUNKS (RAM_LENGTH - 4*1024) / WRITE_CHUNK_SIZE
#define N_READ_CHUNKS  (RAM_LENGTH - 4*1024) / READ_CHUNK_SIZE


int main() {
	char write_buffer[WRITE_CHUNK_SIZE] = "U1UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU";

	DECLARE_BIT_ARRAY(chunks_write_info, N_WRITE_CHUNKS);
	DECLARE_BIT_ARRAY(chunks_read_info, N_READ_CHUNKS);
	bitarray_clear(chunks_write_info, N_WRITE_CHUNKS);
	bitarray_clear(chunks_read_info, N_READ_CHUNKS);

	uart_t uart = uart_init(UART0_BASE_ADDR);
	#define print(str) uart_puts(&uart, str)

	print("Start filling RAM\n");
	
	if (fill_ram(
		RAM_ORIGIN,
		RAM_LENGTH, 
		write_buffer,
		WRITE_CHUNK_SIZE,
		4,
		chunks_write_info
	) != 0) {
		print("ERORR. Can't claim any DMA channel\n");
		return 1;
	} else {
		print("Ram filled with pattern\n");
		print("Chanks map of ram filled (write errors marked as '-')\n");
		print_ram_info(&uart, chunks_write_info, N_WRITE_CHUNKS, 64, RAM_ORIGIN, WRITE_CHUNK_SIZE);
	}
	
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
