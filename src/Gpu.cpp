#include "Gpu.hpp"
#include <iostream>

Gpu::Gpu()
{
}

uint8* Gpu::GetGpuDataAddress()
{
	return (uint8*) & _gpuData;
}

void Gpu::SetupCallback(Memory& memory)
{
	//writing to _gpuData is to draw a graphics primitive
	//writing to _gpuStatus is to change the state
	memory.SubscribeToRange(MemorySubscription(0x1f801810, sizeof(uint32) * 2, true, true, false,
	[](uint32 address, AccessType accessType, uint32 data)
	{
		std::cout << "access of type " << (int)accessType << " at vram i / o port 0x" << std::hex << address << std::endl;
	}));
}
