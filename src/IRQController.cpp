#include "IRQController.hpp"
#include <iostream>

uint8* IRQController::GetMemory()
{
	return (uint8*)&_irqStat;
}

void IRQController::SetupCallback(Memory& memory)
{
	memory.SubscribeToRange(MemorySubscription(0xbf801070, sizeof(uint16), true, true, true,
		[this](uint32 address, AccessType accessType, uint32 data)
		{
			if (accessType == AccessType::READ)
				_irqStat = 0xFFFF;
		}));
}
