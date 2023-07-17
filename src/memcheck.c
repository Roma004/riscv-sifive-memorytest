#include "memcheck.h"
#include "pdma.h"
#include "strutils.h"
#include <stddef.h>


void memcheck_metrics_add_range(memcheck_metrics_t *metrics, poiter_range_t range) {
    metrics->total_ranges_found += 1;
    
    if (metrics->nranges == metrics->max_nranges) return;

    metrics->ranges[metrics->nranges++] = range;
}


void memcheck_metrics_report_print(uart_t *uart, char *report_name, memcheck_metrics_t *metrics, int ranges_per_line) {
	#define print(str) uart_puts(uart, str)

    print("Result of ");
    print(report_name);
    print(":\n");

    print("  Total bad chunks: ");
    {
        char buf[16] = {};
        int idx = 0;
        uint_to_string(buf, &idx, metrics->total_ranges_found);
        
        print(buf);
    }
    print("\n");

    if (metrics->total_ranges_found != 0) {
        print("  list of error ranges:\n");
        for (size_t ranges_counter = 0; ranges_counter < metrics->nranges;) {
            char buf[256] = {' ', ' ', '|'};
            int idx = 3;

            for (int i = 0; i < ranges_per_line && ranges_counter < metrics->nranges; ++i) {
                buf[idx++] = ' ';
                ptr_to_string(buf, &idx, metrics->ranges[ranges_counter].from); 
                buf[idx++] = ' '; buf[idx++] = '-'; buf[idx++] = ' ';
                ptr_to_string(buf, &idx, metrics->ranges[ranges_counter].to);
                buf[idx++] = ' '; buf[idx++] = '|';
                ranges_counter++;
            }
            print(buf);
            print("\n");
        }
        if (metrics->total_ranges_found > metrics->nranges) {
            char buf[16] = {}; int idx = 0;
            uint_to_string(buf, &idx, metrics->total_ranges_found - metrics->nranges);
            print("  and "); print(buf); print(" more ranges.\n");
        }
    }

    #undef print
}



int fill_ram(void *start, size_t write_size, char *pattern, size_t chunk_size, int nchannels, memcheck_metrics_t *metrics) {
	if (nchannels > 4) nchannels = 4;
	if (nchannels < 1) nchannels = 1;

	size_t chunks_to_fill = write_size / chunk_size; 
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
		tmp->next_config.write_ptr = (uint64_t)start + i*channel_offset*chunk_size;

		chunks_for_channel[i] = channel_offset;
	}

	if (nchannels < 1) return -1;

	// if has 1-3 chunks of reminder, give it to the last channel
	if (chunks_to_fill % nchannels != 0) {
		chunks_for_channel[nchannels-1] += chunks_to_fill - (chunks_to_fill * nchannels);
	}

    char channels_tmp[4] = {};

    char asd = 0;

	while (chunks_for_channel[0] || chunks_for_channel[1] || chunks_for_channel[2] || chunks_for_channel[3]) {
		for (int i = 0; i < nchannels; ++i) {
			if (!chunks_for_channel[i]) continue;

			pdma_control_get(&channels[i]);
			if (channels[i].control.run) continue;

			// synthetic case of error. Uncomment to see some DMA write errors
			// if (i == 2) {
            //     if (asd % 128 < 28) {
            //         channels[i].control.error = 1;
            //     } else {
            //         channels[i].control.error = 0;
            //     }
            //     asd++;
            // } 

            if (channels[i].control.error == 1) {
                if (channels_tmp[i] == 0) {
                    memcheck_metrics_add_range(metrics, (poiter_range_t){
                        .from = (void *)channels[i].next_config.write_ptr - chunk_size,
                        .to = (void *)channels[i].next_config.write_ptr,
                    });
                    channels_tmp[i] = 1;
                } else {
                    memcheck_metrics_last_range(metrics).to = (void *)channels[i].next_config.write_ptr;
                }
            } else {
                channels_tmp[i] = 0;
            }

			channels[i].control.error = 0;

			channels[i].control.run = 1;
			pdma_config_write_next(&channels[i]);
			pdma_control_write(&channels[i]);

			chunks_for_channel[i]--;

			channels[i].next_config.write_ptr += chunk_size;
		}
	} 

	return 0;
}


void check_ram(void* start, size_t nbytes, size_t chunk_size, uint64_t pattern, memcheck_metrics_t *metrics) {
	nbytes -= nbytes % chunk_size;
	
	uint64_t *end = (uint64_t *)(start + nbytes);

    char is_err_range = 0;
	for (uint64_t *ptr = start; ptr != end; ++ptr) {
		if ((*ptr & ~pattern) == 0) {
            is_err_range = 0;
            continue;
        }

        if (!is_err_range) {
            memcheck_metrics_add_range(metrics, (poiter_range_t){
                .from = ptr,
                .to = ptr + 1,
            });
        } else {
            memcheck_metrics_last_range(metrics).to = ptr + 1;
        }
        is_err_range = 1;
	}
}
