#include <iostream>
#include <stdexcept>
#include <fstream>
#include <vector>
#include "Memory.hpp"
#include "Parser.hpp"
#include "ComponentManager.hpp"
#include "R3000A.hpp"

int main()
{
	std::ifstream file = std::ifstream("Executables/Bloom.ps-exe", std::ios::binary | std::ios::in | std::ios::ate);
	if (!file.is_open())
		throw std::runtime_error("could not open file");
	int size = file.tellg();
	file.seekg(std::ios::beg);

	uint8* buffer = new uint8[size];
	file.read((char*)buffer, size);

	Memory memory = Memory();
	memory.AddRange(MemoryRange(0x80000000, 2 * 1024 * 1024));
	memory.AddRange(MemoryRange(0x1f800000, 1024));
	memory.AddRange(MemoryRange(0xbfc00000, 512 * 1024));
	memory.AddRange(MemoryRange(0x1f000000, 64 * 1024));

	memory.Write(0x80010000, size - 2048, buffer + 2048);

	uint32 programCounter = *(uint32*)(buffer + 8 + 16);

	Parser parser = Parser(&memory, programCounter);
    parser.Parse();
    
    ComponentManager componentManager;
    componentManager.AddComponent(new R3000A());
    componentManager.AddComponent(new Component(3));
    componentManager.AddComponent(new Component(4));
    componentManager.AddComponent(new Component(6));

    for(int i = 0; i < 24; i++)
        componentManager.Tick();
    
    delete[] buffer;

	return 0;
}
