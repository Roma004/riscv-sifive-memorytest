#ifndef STRUTILS_H
#define STRUTILS_H

#include "stddef.h"

// convert 8-64 bit integer to string.
// *str -- buffer, where to write.
// *idx -- index, since where to write to buffer. will be incremented.
// val -- inteher to write.
// base -- numerical system base (usually 2, 10 or 16, but could be whatever you want).
// prefix[2] -- numerical system prefix (0b, 0x and others).
// *alphabet -- string, containig all numbers of numeric system. if NULL uses "0123456789abcdef" (hex alphabet).
// min_length -- minimal amount of characters to write. (eg. val = 0x00023 and min_length = 3 will write 0x023).
void general_uint_to_string(char *str, int *idx, size_t val, int base, const char prefix[2], const char *alphabet, int min_length);

#define alph_hex   "0123456789abcdef"
#define alph_dec   "0123456789"
#define alph_bin   "01"
#define hex_prefix "0x"
#define bin_prefix "0b"

#define ptr_to_string(str, idx, ptr) general_uint_to_string(str, idx, (size_t)ptr, 16, hex_prefix, alph_hex, 8)
#define uint_to_string(str, idx, val) general_uint_to_string(str, idx, val, 10, NULL, alph_dec, 1)
#define hex_to_string(str, idx, val) general_uint_to_string(str, idx, val, 16, hex_prefix, alph_hex, 1)
#define bin_to_string(str, idx, val) general_uint_to_string(str, idx, val, 2, bin_prefix, alph_bin, 1)

#endif