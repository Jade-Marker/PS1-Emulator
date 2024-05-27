#include "Memory.hpp"

bool Memory::GetRange(uint32 address, MemoryRange& range)
{
	for (const auto& currentRange : _memory)
	{
		if (address >= currentRange.startingAddress && address < currentRange.startingAddress + currentRange.size)
		{
			range = currentRange;
			return true;
		}
	}

	return false;
}

Memory::Memory()
{
}

Memory::~Memory()
{
    for(const auto& range : _memory)
    {
        if(range.ownsBuffer)
            delete [] range.buffer;
    }
}

void Memory::AddRange(MemoryRange range)
{
	if (range.ownsBuffer)
		range.buffer = new uint8[range.size];

	bool inserted = false;
	for (int i = 0; i < _memory.size(); i++)
	{
		const MemoryRange& currentRange = _memory[i];

		if (range.startingAddress + range.size < currentRange.startingAddress + currentRange.size)
		{
			auto iter = _memory.begin();
			std::advance(iter, i);
			_memory.insert(iter, range);
			inserted = true;
			break;
		}
	}

	if (!inserted)
		_memory.push_back(range);
}

uint8 Memory::ReadByte(uint32 address)
{
	MemoryRange range;
	GetRange(address, range);

	return range.buffer[address - range.startingAddress];
}

uint16 Memory::ReadHalfWord(uint32 address)
{
	MemoryRange range;
	GetRange(address, range);

	uint16* buffer = (uint16*)&range.buffer[address - range.startingAddress];

	return *buffer;
}

uint32 Memory::ReadWord(uint32 address)
{
	MemoryRange range;
	GetRange(address, range);

	uint32* buffer = (uint32*) &range.buffer[address - range.startingAddress];

	return *buffer;
}

void Memory::Write(uint32 address, uint32 length, uint8* data)
{
	MemoryRange range;
	GetRange(address, range);

	uint8* dst = &range.buffer[address - range.startingAddress];
	for (uint32 i = 0; i < length; i++)
	{
		*dst = *data;
		dst++;
		data++;
	}
}

void Memory::WriteByte(uint32 address, uint8 data)
{
	MemoryRange range;
	GetRange(address, range);

	uint8* dst = (uint8*)&range.buffer[address - range.startingAddress];
	*dst = data;
}

void Memory::WriteHalfword(uint32 address, uint16 data)
{
	MemoryRange range;
	GetRange(address, range);

	uint16* dst = (uint16*)&range.buffer[address - range.startingAddress];
	*dst = data;
}

void Memory::WriteWord(uint32 address, uint32 data)
{
	MemoryRange range;
	GetRange(address, range);

	uint32* dst = (uint32*) & range.buffer[address - range.startingAddress];
	*dst = data;
}
