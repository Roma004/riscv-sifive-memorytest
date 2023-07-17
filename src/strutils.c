#include "strutils.h"
#include <stddef.h>
#include "stdint.h"


void general_uint_to_string(char *str, int *idx, size_t val, int base, const char prefix[2], const char *alphabet, int min_length) {
    // 64 is maximum length of 64-bit int in base-2  
	char buf[64] = {};

    // defaulting to hex alphabet
    if (alphabet == NULL) alphabet = alph_hex;

    // need to add prefix, like 0x or 0b
    if (prefix != NULL) {
        str[(*idx)++] = prefix[0];
        str[(*idx)++] = prefix[1];
    }

	int i = 0;
	while (val) {
		buf[i++] = val % base;
		val /= base;
	}
	char flag = 0;
	for (int i = base-1; i >= 0; --i) {
		if (!flag && i > min_length-1 && buf[i] == 0) continue;
		flag = 1;
		str[(*idx)++] = alphabet[buf[i]];
	}
}
