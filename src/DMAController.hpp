#pragma once
#include <array>
#include "Types.hpp"
#include "Memory.hpp"

struct DMAChannel
{
	uint32 dmaMemoryAddressRegister;
	uint32 dmaBlockControlRegister;
	uint32 dmaChannelControlRegister;

	uint32 padding;
};

constexpr uint8 cDmaChannelEnabled = 8;

union DMAPrimaryControlRegister
{
	struct {
		uint8 dma0 : 4;
		uint8 dma1 : 4;
		uint8 dma2 : 4;
		uint8 dma3 : 4;
		uint8 dma4 : 4;
		uint8 dma5 : 4;
		uint8 dma6 : 4;
	};

	uint32 asInteger;
};

class DMAController
{
private:
	Memory* pMemory;

	std::array<DMAChannel, 7> dmaChannels;
	DMAPrimaryControlRegister dmaPrimaryControlRegister;

public:
	DMAController(Memory* pMemory);

	void SetupCallback(Memory& memory);
	uint8* GetMemory();
};

