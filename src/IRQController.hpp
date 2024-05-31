#pragma once

#include "Types.hpp"
#include "Memory.hpp"

class IRQController
{
private:
	uint16 _irqStat;

public:
	uint8* GetMemory();
	void SetupCallback(Memory& memory);
};

