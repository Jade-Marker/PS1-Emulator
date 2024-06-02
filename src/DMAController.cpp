#include "DMAController.hpp"
#include <iostream>

DMAController::DMAController(Memory* pMemory):
	_pMemory(pMemory)
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
							
							uint32 entry = _pMemory->ReadWord(_pMemory->ReadWord(_dmaChannels[2].dmaMemoryAddressRegister));
							uint32 currentWordAddress = _pMemory->ReadWord(_dmaChannels[2].dmaMemoryAddressRegister) + 4;
							while (entry != 0xFFFFFFFF)
							{
								uint32 nextAddress = entry & 0x00FFFFFF;
								uint8 sizeofEntry = (entry & 0xFF000000) >> 24;

								//Send data to gpu
								for (int i = 0; i < sizeofEntry; i++)
								{
									_pMemory->WriteWord(0x1f801810, _pMemory->ReadWord(currentWordAddress));
									currentWordAddress += 4;
								}

								currentWordAddress = nextAddress + 4;
								entry = _pMemory->ReadWord(nextAddress);
							}
						}
						break;

						case 6:
							{
								uint32 currentAddress = _dmaChannels[6].dmaMemoryAddressRegister;
								for (int i = 0; i < _dmaChannels[6].dmaBlockControlRegister; i++)
								{
									_pMemory->WriteWord(currentAddress, (currentAddress - 4) & 0x00FFFFFF);
									currentAddress -= 4;
								}
								_pMemory->WriteWord(currentAddress, 0xFFFFFFFF);
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
					_dmaChannels[dmaChannelSelect].dmaChannelControlRegister &= 0x00FFFFFF;
				}
			}

			//std::cout << "access of type " << (int)accessType << " for dma, at 0x" << std::hex << address << std::endl;
		}));
}

uint8* DMAController::GetMemory()
{
	return (uint8*) & _dmaChannels[0];
}
