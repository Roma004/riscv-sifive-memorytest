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

typedef struct __bitarray {
    struct __bitarr_chunk *arr;
    size_t len;
    size_t nbytes;
} bitarray_t;

#define bitarr_chunk_size (sizeof(struct __bitarr_chunk) * 8)

// need this to prevent from large data copy
// as we cant use heap, need to declare that way
#define DECLARE_BIT_ARRAY(name, nels) \
struct __bitarr_chunk __bitarr##name = {}; \
bitarray_t name = { \
    .arr = &__bitarr##name, \
    .len = nels, \
    .nbytes = (nels + bitarr_chunk_size - 1) / bitarr_chunk_size \
};

void bitarray_set(bitarray_t *arr, size_t index, char val);
int bitarray_get(bitarray_t *arr, size_t index);

#endif