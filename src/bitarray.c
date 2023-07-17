#include "bitarray.h"
#include <stddef.h>


void bitarray_set(bitarray_t *arr, size_t index, char val) {
    size_t bit_id = index % bitarr_chunk_size;
    size_t arr_index = index / bitarr_chunk_size;

    if (index >= arr->len) return;

    // So, not very elegant, but uses less arithmetics
    switch (bit_id) {
    case 0:
        arr->arr[arr_index].bit0 = val; break;
    case 1:
        arr->arr[arr_index].bit1 = val; break;
    case 2:
        arr->arr[arr_index].bit2 = val; break;
    case 3:
        arr->arr[arr_index].bit3 = val; break;
    case 4:
        arr->arr[arr_index].bit4 = val; break;
    case 5:
        arr->arr[arr_index].bit5 = val; break;
    case 6:
        arr->arr[arr_index].bit6 = val; break;
    case 7:
        arr->arr[arr_index].bit7 = val; break;
    }
}

int bitarray_get(bitarray_t *arr, size_t index) {
    size_t bit_id = index % bitarr_chunk_size;
    size_t arr_index = index / bitarr_chunk_size;

    if (index >= arr->len) return -1;

    // So, not very elegant, but uses less arithmetics
    switch (bit_id) {
    case 0:
        return arr->arr[arr_index].bit0;
    case 1:
        return arr->arr[arr_index].bit1;
    case 2:
        return arr->arr[arr_index].bit2;
    case 3:
        return arr->arr[arr_index].bit3;
    case 4:
        return arr->arr[arr_index].bit4;
    case 5:
        return arr->arr[arr_index].bit5;
    case 6:
        return arr->arr[arr_index].bit6;
    case 7:
        return arr->arr[arr_index].bit7;
    default:
        return -1;
    }
}