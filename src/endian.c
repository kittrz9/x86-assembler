#include "endian.h"

#include <stdint.h>
#include <stdio.h>

enum endianness detectedEndianness;

bool endianDetect(void) {
	uint16_t test = 0xaabb;
	uint8_t* byte = (uint8_t*)&test;
	if(*byte == 0xaa) {
		detectedEndianness = BIG_ENDIAN;
	} else {
		detectedEndianness = LITTLE_ENDIAN;
	}
	return detectedEndianness;
}

// could just assume that the endianness it's going to convert to is little endian
// since this currently only targets x86
// but maybe I'll try changing this to support some other (more sane) architecture
void endianSwap(void* data, size_t size, enum endianness dataEndianness) {
	uint8_t* dataBytes = data;
	if(detectedEndianness != dataEndianness) {
		for(size_t i = 0; i < size/2; ++i) {
			uint8_t tmp = dataBytes[i];
			dataBytes[i] = dataBytes[size-i-1];
			dataBytes[size-i-1] = tmp;
		}
	}
}
