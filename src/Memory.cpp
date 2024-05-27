#include <stdexcept>
#include <string>
#include "Memory.hpp"


void MemoryRange::ReadSubscriptions(uint32 address)
{
	for (const auto& subscription : subscriptions)
	{
		if (subscription.subscribeToReads && address >= subscription.startAddress && address < subscription.startAddress + subscription.size)
		{
			subscription.callback(address);
		}
	}
}

void MemoryRange::WriteSubscriptions(uint32 address)
{
	for (const auto& subscription : subscriptions)
	{
		if (subscription.subscribeToWrites && address >= subscription.startAddress && address < subscription.startAddress + subscription.size)
		{
			subscription.callback(address);
		}
	}
}

MemoryRange& Memory::GetRange(uint32 address)
{
	for (auto& currentRange : _memory)
	{
		if (address >= currentRange.startingAddress && address < currentRange.startingAddress + currentRange.size)
		{
			return currentRange;
		}
	}

	throw std::runtime_error("Invalid write! No range found for address " + std::to_string(address));
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

void Memory::SubscribeToRange(MemorySubscription subscription)
{
	MemoryRange& range = GetRange(subscription.startAddress);

	range.subscriptions.push_back(subscription);
}

uint8 Memory::ReadByte(uint32 address)
{
	MemoryRange& range = GetRange(address);
	range.ReadSubscriptions(address);

	return range.buffer[address - range.startingAddress];
}

uint16 Memory::ReadHalfWord(uint32 address)
{
	MemoryRange& range = GetRange(address);;
	range.ReadSubscriptions(address);

	uint16* buffer = (uint16*)&range.buffer[address - range.startingAddress];

	return *buffer;
}

uint32 Memory::ReadWord(uint32 address)
{
	MemoryRange& range = GetRange(address);
	range.ReadSubscriptions(address);

	uint32* buffer = (uint32*) &range.buffer[address - range.startingAddress];

	return *buffer;
}

void Memory::Write(uint32 address, uint32 length, uint8* data)
{
	MemoryRange& range = GetRange(address);
	range.WriteSubscriptions(address);

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
	MemoryRange& range = GetRange(address);
	range.WriteSubscriptions(address);

	uint8* dst = (uint8*)&range.buffer[address - range.startingAddress];
	*dst = data;
}

void Memory::WriteHalfword(uint32 address, uint16 data)
{
	MemoryRange& range = GetRange(address);
	range.WriteSubscriptions(address);

	uint16* dst = (uint16*)&range.buffer[address - range.startingAddress];
	*dst = data;
}

void Memory::WriteWord(uint32 address, uint32 data)
{
	MemoryRange& range = GetRange(address);
	range.WriteSubscriptions(address);

	uint32* dst = (uint32*) & range.buffer[address - range.startingAddress];
	*dst = data;
}

