#include <iostream>
#include <stdexcept>
#include <fstream>
#include <vector>
#include "Memory.hpp"
#include "R3000A.hpp"
#include "Constants.hpp"

//todo
//Evaluate if MemoryRange structure is still good?
//Update Memory so that components can subscribe to changes in given range
//Get window drawing
//Get vulkan drawing triangle
//Get vulkan drawing constantly changing vertex list
//Get GPU component 
//Get program running via bios reset
//Update instructions and cpu to properly match pipelining
//Get componentmanager running at given tick rate per second
//Get CPU running subset using lookup method
//Compare switch/case and lookup methods

uint8* ReadBinaryFile(uint32& size, const std::string& filePath)
{
	std::ifstream file = std::ifstream(filePath, std::ios::binary | std::ios::in | std::ios::ate);
	if (!file.is_open())
		throw std::runtime_error("could not open file");
	size = file.tellg();
	file.seekg(std::ios::beg);

	uint8* buffer = new uint8[size];
	file.read((char*)buffer, size);

	return buffer;
}

int main()
{
	uint32 programSize;	uint32 biosSize;
	uint8* program = ReadBinaryFile(programSize, "Executables/BasicGraphics.ps-exe");
	uint8* bios = ReadBinaryFile(biosSize, "OpenBios/openbios.bin");

	if (biosSize != 512 * 1024)
		throw std::runtime_error("BIOS incorrect size");

	Memory memory = Memory();
	uint8* ram = new uint8[2 * 1024 * 1024];
	memory.AddRange(MemoryRange(0x80000000, 2 * 1024 * 1024, ram));
	memory.AddRange(MemoryRange(0x00000000, 2 * 1024 * 1024, ram));
	memory.AddRange(MemoryRange(0xa0000000, 2 * 1024 * 1024, ram));
	memory.AddRange(MemoryRange(0x1f800000, 1024));
	memory.AddRange(MemoryRange(0xbfc00000, 512 * 1024));
	//memory.AddRange(MemoryRange(0x1f000000, 64 * 1024));

	uint8* hardwareRegisters = new uint8[8 * 1024];
	memory.AddRange(MemoryRange(0x1f801000, 8 * 1024, hardwareRegisters));

	
	for (uint32 i = 0; i < 2 * 1024 * 1024; i++)
		ram[i] = 0;

	memory.Write(0x80010000, programSize - 2048, program + 2048);
	memory.Write(0xbfc00000, 512 * 1024, bios);

	//uint32 programCounter = 0xbfc00000;
	uint32 programCounter = *(uint32*)(program + 8 + 16);
    
    R3000A cpu = R3000A(&memory, programCounter, std::array<uint32, 32>
	{
		0x00000000, 0x801FFF0,	0x00000000,	0x8011734,	0x8011884,	0x00000000, 0x00000000, 0x00000000,
		0X00006454, 0x000000F4, 0x000000B0, 0x80011600, 0xA0010004, 0x00000001, 0xFFFFFFFF, 0x00000000,
		0x00004374, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000010, 0x00000008, 0x1FC09238, 0x00000000, 0x00000000, 0x801FFFC8, 0x801FFFF0, 0x80116B8
	});

	while (true)
	{
		cpu.RunNCycles(1000);
	}

    delete[] program;

	return 0;
}
