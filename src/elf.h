#ifndef ELF_H
#define ELF_H

#include <stdint.h>

#include "dynamicArray.h"

void createElfFromCode(char* filePath, dynamicArray* code, dynamicArray* labels, dynamicArray* backpatches);

// only going to deal with 64 bit for now
// https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
struct elfHeaderStruct {
	uint8_t magic[4];
	uint8_t bits;
	uint8_t endian;
	uint8_t version;
	uint8_t abi;
	uint8_t abiVer;
	uint8_t padding[7];
	uint16_t type;
	uint16_t arch;
	uint32_t ver2;
	uint64_t entryPoint;
	uint64_t programHeaderOffset;
	uint64_t sectionHeaderOffset;
	uint32_t flags;
	uint16_t headerSize;
	uint16_t programHeaderSize;
	uint16_t programHeaderCount;
	uint16_t sectionHeaderSize;
	uint16_t sectionHeaderCount;
	uint16_t sectionNamesIndex;
};

struct programHeaderStruct {
	uint32_t type;
	uint32_t flags;
	uint64_t offset;
	uint64_t virtAddr;
	uint64_t physAddr;
	uint64_t fileSize;
	uint64_t memSize;
	uint64_t align;
};

struct sectionHeaderStruct {
	uint32_t nameOffset;
	uint32_t type;
	uint64_t flags;
	uint64_t virtAddr;
	uint64_t fileOffset;
	uint64_t size;
	uint32_t link;
	uint32_t info;
	uint64_t addrAlign;
	uint64_t entSize;
};

// program header types
enum {
	PT_NULL = 0,
	PT_LOAD = 1,
	PT_DYNAMIC = 2,
	PT_INTERP = 3,
	PT_NOTE = 4,
	PT_SHLIB = 5,
	PT_PHDR = 6,
	PT_TLS = 7,
	PT_LOOS = 0x60000000,
	PT_HIOS = 0x6FFFFFFF,
	PT_LOPROC = 0x70000000,
	PT_HIPROC = 0x7FFFFFFF,
};

// segment permissions
enum {
	PF_X = 1,
	PF_W = 2,
	PF_R = 4,
};

#endif // ELF_H
