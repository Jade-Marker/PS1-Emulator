#pragma once

#include "Types.hpp"
#include "Memory.hpp"
#include "R3000A.hpp"

class IRQController
{
private:
	uint16 _irqStat;
	R3000A* _cpu;

public:
	IRQController(R3000A* cpu);

	uint8* GetMemory();
	void SetupCallback(Memory& memory);
};

