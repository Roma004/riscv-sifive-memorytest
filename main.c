#include "base.h"
#include "bitarray.h"
#include "memcheck.h"
#include "pdma.h"
#include "uart0.h"
#include <stddef.h>
#include <stdint.h>

#define KBYTE 1024U
#define MBYTE KBYTE *KBYTE

// #define WRITE_CHUNK_SIZE 512
#define WRITE_CHUNK_SIZE 1 * KBYTE

// 8M reserved for ELF
#define RAM_LENGTH 120 * MBYTE

#define RANGES_PER_LINE   6
#define MAX_RANGES_NUMBER 128

#define MEMCHECK_PATTERN 0x5555555555555555

int main() {
    // change some U characters to what ever you want to see `check_ram` logs
    char write_buffer[WRITE_CHUNK_SIZE] =
        "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU"
        "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU"
        "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU"
        "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU"
        "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU"
        "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU"
        "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU"
        "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU"
        "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU"
        "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU"
        "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU"
        "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU"
        "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU"
        "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU"
        "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU"
        "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU";

    memcheck_metrics_declare(write_metrics, MAX_RANGES_NUMBER)
        memcheck_metrics_declare(read_metrics, MAX_RANGES_NUMBER)

            uart_t uart = uart_init(UART0_BASE_ADDR);
#define print(str) uart_puts(&uart, str)

    print("Program MEMCHECK start\n");

    print("Start filling RAM with predefined pattern\n\n");

    if (fill_ram(
            RAM_ORIGIN,
            RAM_LENGTH,
            write_buffer,
            WRITE_CHUNK_SIZE,
            4,
            &write_metrics
        )
        != 0) {
        print("ERORR. Can't claim any DMA channel\n");
        return 1;
    } else {
        print("Ram filled with pattern\n\n");
        memcheck_metrics_report_print(
            &uart, "DMA RAM filling", &write_metrics, RANGES_PER_LINE
        );
    }

    print("\nRun post-write errors checking\n");
    check_ram(
        RAM_ORIGIN,
        RAM_LENGTH,
        WRITE_CHUNK_SIZE,
        MEMCHECK_PATTERN,
        &read_metrics
    );

    memcheck_metrics_report_print(
        &uart,
        "Post-write Memory check with CPU",
        &read_metrics,
        RANGES_PER_LINE
    );
    print("\n");

    if (read_metrics.total_ranges_found == 0
        && write_metrics.total_ranges_found == 0) {
        print("All test passed! No r/w RAM erorrs found.\n");

    } else {
        if (write_metrics.total_ranges_found != 0) {
            print("Some DMA writes failes (see logs abow)!\n");
        }
        if (read_metrics.total_ranges_found != 0) {
            print("Some bytes've been written incorrectly (see logs abow)!\n");
        }
    }

    print("Program MEMCHECK exit.\n");

#undef print

    return 0;
}
