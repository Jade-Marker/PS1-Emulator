#pragma once

#include <vector>
#include "Types.hpp"

struct MemoryRange
{
	uint32 startingAddress;
	uint32 size;
	uint8* buffer;
	bool ownsBuffer;

	MemoryRange(uint32 startingAddress,	uint32 size, uint8* buffer):
		startingAddress(startingAddress), size(size), buffer(buffer), ownsBuffer(false)
	{}

	MemoryRange(uint32 startingAddress, uint32 size) :
		startingAddress(startingAddress), size(size), buffer(buffer), ownsBuffer(true)
	{}

	MemoryRange() :
		startingAddress(0), size(0), buffer(nullptr), ownsBuffer(false)
	{}
};

class Memory
{
private:
	//store vector that says for what range we should use what object
	std::vector<MemoryRange> _memory;

	bool GetRange(uint32 address, MemoryRange& range);

public:
	Memory();
    ~Memory();
    
	void AddRange(MemoryRange range);

	uint8 Read(uint32 address);
	uint32 ReadInt(uint32 address);

	void Write(uint32 address, uint32 length, uint8* data);
};

