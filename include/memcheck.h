#ifndef MEMCHECK_H
#define MEMCHECK_H

#include "base.h"
#include "bitarray.h"
#include "uart0.h"
#include <stddef.h>

typedef struct __pointer_range {
    void *from;
    void *to;
} poiter_range_t;

typedef struct __memcheck_metrics {
    poiter_range_t *ranges;
    size_t max_nranges;
    size_t nranges;
    size_t total_ranges_found;
} memcheck_metrics_t;



int fill_ram(
    void *start,
    size_t write_size,
    char *pattern,
    size_t chunk_size,
    int nchannels,
    memcheck_metrics_t *metrics
);
void check_ram(void* start, size_t nbytes, size_t chunk_size, uint64_t pattern, memcheck_metrics_t *metrics);

void memcheck_metrics_add_range(memcheck_metrics_t *metrics, poiter_range_t range);
void memcheck_metrics_report_print(uart_t *uart, char *report_name, memcheck_metrics_t *metrics, int ranges_per_line);

#define memcheck_metrics_last_range(metrics) metrics->ranges[metrics->nranges-1]

// need this to prevent from large data copy
// as we cant use heap, need to declare that way
#define memcheck_metrics_declare(name, nranges) \
	poiter_range_t __## name ##_ranges[nranges] = {}; \
	memcheck_metrics_t name = { \
		.ranges = __## name ##_ranges, \
		.max_nranges = nranges \
	};

#endif