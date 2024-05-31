#include "DMAController.hpp"
#include <iostream>

DMAController::DMAController(Memory* pMemory):
	pMemory(pMemory)
{
}

void DMAController::SetupCallback(Memory& memory)
{
	memory.SubscribeToRange(MemorySubscription(0x1f801080, sizeof(DMAController), true, true, true,
		[this](uint32 address, AccessType accessType, uint32 data)
		{
			uint8 lsbOfAddress = address;
			lsbOfAddress &= 0x0F;
			
			uint8 dmaChannelSelect = address;
			dmaChannelSelect &= 0xF0;
			dmaChannelSelect = dmaChannelSelect >> 4;
			dmaChannelSelect -= 8;

			if (accessType == AccessType::WRITE)
			{
				//should check if channel is enabled first, but fine for now
				if (lsbOfAddress == 0x08)
				{
					//writing to control register, so we should actually do something
					switch (dmaChannelSelect)
					{
						case 2:
						{
							//send linked list to gpu
							
							uint32 entry = pMemory->ReadWord(pMemory->ReadWord(dmaChannels[2].dmaMemoryAddressRegister));
							while (entry != 0xFFFFFFFF)
							{
								uint32 nextAddress = entry & 0x00FFFFFF;
								uint8 sizeofEntry = (entry & 0xFF000000) >> 24;



								entry = pMemory->ReadWord(nextAddress);
							}
						}
						break;

						case 6:
							{
								uint32 currentAddress = dmaChannels[6].dmaMemoryAddressRegister;
								for (int i = 0; i < dmaChannels[6].dmaBlockControlRegister; i++)
								{
									pMemory->WriteWord(currentAddress, (currentAddress - 4) & 0x00FFFFFF);
									currentAddress -= 4;
								}
								pMemory->WriteWord(currentAddress, 0xFFFFFFFF);
							}
							break;

						default:
							throw std::runtime_error("DMA access out of range");
							break;
					}
				}
			}
			else if (accessType == AccessType::READ)
			{
				if (lsbOfAddress == 0x08)
				{
					dmaChannels[dmaChannelSelect].dmaChannelControlRegister &= 0x00FFFFFF;
				}
			}

			std::cout << "access of type " << (int)accessType << " for dma, at 0x" << std::hex << address << std::endl;
		}));
}

uint8* DMAController::GetMemory()
{
	return (uint8*) & dmaChannels[0];
}
