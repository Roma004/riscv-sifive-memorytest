#ifndef BITARRAY_H
#define BITARRAY_H

#include "stdint.h"
#include "stddef.h"

struct __bitarr_chunk {
    uint8_t bit0 : 1;
    uint8_t bit1 : 1;
    uint8_t bit2 : 1;
    uint8_t bit3 : 1;
    uint8_t bit4 : 1;
    uint8_t bit5 : 1;
    uint8_t bit6 : 1;
    uint8_t bit7 : 1;
};

typedef struct __bitarr_chunk *bitarray_t;

#define DECLARE_BIT_ARRAY(name, nels) struct __bitarr_chunk name[(nels + 7) / 8]
#define BITARRAY_SET(arr, index, bit_id, val) arr[index].bit##bit_id = val & 0x1
#define BITARRAY_GET(arr, index, bit_id) (arr[index].bit##bit_id)

void bitarray_set(bitarray_t arr, size_t index, char val);
char bitarray_get(bitarray_t arr, size_t index);
void bitarray_clear(bitarray_t arr, size_t len);

#endif