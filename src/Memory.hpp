#pragma once

#include <vector>
#include <functional>
#include "Types.hpp"

enum class AccessType
{
	UKNOWN,

	READ,
	WRITE
};

struct MemorySubscription
{
	uint32 startAddress;
	uint32 size;
	bool subscribeToReads;
	bool subscribeToWrites;
	bool allowsMemoryWrite;
	std::function<void(uint32, AccessType, uint32)> callback;

	MemorySubscription(uint32 startAddress, uint32 size, bool subscribeToReads, bool subscribeToWrites, bool allowsMemoryWrite, std::function<void(uint32, AccessType, uint32)> callback) :
		startAddress(startAddress), size(size), subscribeToReads(subscribeToReads), subscribeToWrites(subscribeToWrites), allowsMemoryWrite(allowsMemoryWrite), callback(callback)
	{}
};

struct MemoryRange
{
	uint32 startingAddress;
	uint32 size;
	uint8* buffer;
	bool ownsBuffer;
	std::vector<MemorySubscription> subscriptions;

	MemoryRange(uint32 startingAddress,	uint32 size, uint8* buffer):
		startingAddress(startingAddress), size(size), buffer(buffer), ownsBuffer(false)
	{}

	MemoryRange(uint32 startingAddress, uint32 size) :
		startingAddress(startingAddress), size(size), buffer(buffer), ownsBuffer(true)
	{}

	MemoryRange() :
		startingAddress(0), size(0), buffer(nullptr), ownsBuffer(false)
	{}

	void inline ReadSubscriptions(uint32 address);
	bool inline WriteSubscriptions(uint32 address, uint32 data);
};

class Memory
{
private:
	//store vector that says for what range we should use what object
	std::vector<MemoryRange> _memory;

	MemoryRange& GetRange(uint32 address);

public:
	Memory();
    ~Memory();
    
	void AddRange(MemoryRange range);
	void SubscribeToRange(MemorySubscription subscription);

	uint8 ReadByte(uint32 address);
	uint16 ReadHalfWord(uint32 address);
	uint32 ReadWord(uint32 address);

	void Write(uint32 address, uint32 length, uint8* data);
	void WriteByte(uint32 address, uint8 data);
	void WriteHalfword(uint32 address, uint16 data);
	void WriteWord(uint32 address, uint32 data);
};

