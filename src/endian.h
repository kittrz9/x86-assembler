#ifndef ENDIAN_H
#define ENDIAN_H

#include <stdbool.h>
#include <stddef.h>

enum endianness {
	BIG_ENDIAN,
	LITTLE_ENDIAN,
};

bool endianDetect(void);
void endianSwap(void* data, size_t size, enum endianness dataEndianness);

#endif // ENDIAN_H
