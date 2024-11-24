#include "elf.h"

#include <stdio.h>

#define SECTION_ADDR 0x400000

char sectionNames[] = "\0.text\0.shstrtab\0";

void createElfFromCode(char* filePath, uint8_t* code, uint64_t codeSize) {
	uint64_t codeOffset = sizeof(struct elfHeaderStruct) + sizeof(struct programHeaderStruct);
	struct programHeaderStruct pHeaders[] = {
		{
			.type = PT_LOAD,
			.flags = PF_R | PF_W | PF_X,
			.offset = codeOffset,
			.virtAddr = SECTION_ADDR + codeOffset,
			.physAddr = SECTION_ADDR + codeOffset,
			.fileSize = codeSize,
			.memSize = codeSize
		}
	};

	struct sectionHeaderStruct sHeaders[] = {
		// null section
		{
			0,
		},
		// program section
		{
			.nameOffset = 1,
			.type = 1, // SHT_PROGBITS
			.flags = 2 | 4, // SHF_ALLOC | SHF_EXECINSTR
			.fileOffset = codeOffset,
			.virtAddr = SECTION_ADDR + codeOffset,
			.size = codeSize,
			.link = 0,
			.info = 0,
			.addrAlign = 0,
			.entSize = codeSize,
		},
		// section name section
		{
			.nameOffset = 7,
			.type = 3, // SHT_STRTAB
			.flags = 0,
			.fileOffset = codeOffset + codeSize,
			.virtAddr = 0,
			.size = sizeof(sectionNames),
			.link = 0,
			.info = 0,
			.addrAlign = 0,
			.entSize = sizeof(sectionNames),
		},
	};

	struct elfHeaderStruct elfHeader = {
		.magic = "\x7F""ELF", // have to concatenate like this because of funny hex string shenanigans
		.bits = 2, // 64 bit
		.endian = 1, // little endian
		.version = 1,
		.abi = 0, // sys-v
		.abiVer = 0,
		.padding = {0},
		.type = 2, // executable
		.arch = 0x3E, // x86-64
		.ver2 = 1,
		.entryPoint = sHeaders[1].virtAddr,
		.programHeaderOffset = sizeof(struct elfHeaderStruct),
		.sectionHeaderOffset = sizeof(struct elfHeaderStruct) + sizeof(pHeaders) + codeSize + sizeof(sectionNames),
		.flags = 0,
		.headerSize = sizeof(struct elfHeaderStruct),
		.programHeaderSize = sizeof(struct programHeaderStruct),
		.programHeaderCount = sizeof(pHeaders)/sizeof(struct programHeaderStruct),
		.sectionHeaderSize = sizeof(struct sectionHeaderStruct),
		.sectionHeaderCount = sizeof(sHeaders)/sizeof(struct sectionHeaderStruct),
		.sectionNamesIndex = 2,
	};

	FILE* f = fopen(filePath, "wb");

	fwrite(&elfHeader, sizeof(elfHeader), 1, f);
	fwrite(&pHeaders, sizeof(pHeaders), 1, f);
	fwrite(code, codeSize, 1, f);
	fwrite(&sectionNames, sizeof(sectionNames), 1, f);
	fwrite(&sHeaders, sizeof(sHeaders), 1, f);


	fclose(f);
}
