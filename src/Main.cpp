#include <iostream>
#include <stdexcept>
#include <fstream>
#include <vector>
#include "Memory.hpp"
#include "ComponentManager.hpp"
#include "R3000A.hpp"
#include "Constants.hpp"

//todo
//Set CPU to match whatever state actual one does on program start
//Get all instructions from BasicGraphics working
//Evaluate if component/componentmanager structure is still good?
//Evaluate if MemoryRange structure is still good?
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
	uint8* program = ReadBinaryFile(programSize, "Executables/BasicAsm.ps-exe");
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
	memory.AddRange(MemoryRange(0x1f000000, 64 * 1024));

	memory.Write(0x80010000, programSize - 2048, program + 2048);
	memory.Write(0xbfc00000, 512 * 1024, bios);

	//uint32 programCounter = 0xbfc00000;
	uint32 programCounter = *(uint32*)(program + 8 + 16);
    
    ComponentManager componentManager;
    componentManager.AddComponent(new R3000A(&memory, programCounter));

	while (true)
		componentManager.RunNTicks(100);
	//componentManager.RunNTicks(1000);

    delete[] program;

	return 0;
}
