#include "bitarray.h"
#include <stddef.h>

#define struct_size (sizeof(struct __bitarr_chunk) * 8)

void bitarray_set(bitarray_t arr, size_t index, char val) {
    int bit_id = index % struct_size;
    int arr_index = index / struct_size;

    // So, not very elegant, but uses less arithmetics
    switch (bit_id) {
    case 0:
        BITARRAY_SET(arr, arr_index, 0, val); break;
    case 1:
        BITARRAY_SET(arr, arr_index, 1, val); break;
    case 2:
        BITARRAY_SET(arr, arr_index, 2, val); break;
    case 3:
        BITARRAY_SET(arr, arr_index, 3, val); break;
    case 4:
        BITARRAY_SET(arr, arr_index, 4, val); break;
    case 5:
        BITARRAY_SET(arr, arr_index, 5, val); break;
    case 6:
        BITARRAY_SET(arr, arr_index, 6, val); break;
    case 7:
        BITARRAY_SET(arr, arr_index, 7, val); break;
    }
}

char bitarray_get(bitarray_t arr, size_t index) {
    int bit_id = index % struct_size;
    int arr_index = index / struct_size;

    // So, not very elegant, but uses less arithmetics
    switch (bit_id) {
    case 0:
        return BITARRAY_GET(arr, arr_index, 0);
    case 1:
        return BITARRAY_GET(arr, arr_index, 1);
    case 2:
        return BITARRAY_GET(arr, arr_index, 2);
    case 3:
        return BITARRAY_GET(arr, arr_index, 3);
    case 4:
        return BITARRAY_GET(arr, arr_index, 4);
    case 5:
        return BITARRAY_GET(arr, arr_index, 5);
    case 6:
        return BITARRAY_GET(arr, arr_index, 6);
    case 7:
        return BITARRAY_GET(arr, arr_index, 7);
    default:
        return -1;
    }
}

void bitarray_clear(bitarray_t arr, size_t len) {
    size_t nbytes = (len + struct_size - 1) / struct_size; 
    for (int i = 0; i < nbytes; ++i) {
        *(uint8_t *)(arr + i) = 0;
    }
}

#undef struct_size