#include <iostream>
#include <fstream>
#include <vector>
#include "Memory.hpp"
#include "Parser.hpp"

int main()
{
	std::ifstream file = std::ifstream("Executables/Bloom.ps-exe", std::ios::binary | std::ios::in | std::ios::ate);
	if (!file.is_open())
		throw std::exception("could not open file");
	int size = file.tellg();
	file.seekg(std::ios::beg);

	uint8* buffer = new uint8[size];
	file.read((char*)buffer, size);

	Memory memory = Memory();
	memory.AddRange(MemoryRange(0x80000000, size));
	memory.AddRange(MemoryRange(0x1f800000, 1024));
	memory.AddRange(MemoryRange(0xbfc00000, 512 * 1024));
	memory.AddRange(MemoryRange(0x1f000000, 64 * 1024));

	memory.Write(0x80010000, size - 2048, buffer + 2048);

	uint32 programCounter = *(uint32*)(buffer + 8 + 16);

	Parser parser = Parser(&memory, programCounter);
	while (1)
		parser.Parse();

	return 0;
}