#include "Gpu.hpp"
#include <map>
#include <iostream>

#define GET_BIT(n, value) ((0x1 << n) & value)

std::map<PrimitiveID, uint32> primitiveSizes
{
	{PrimitiveID::FRAMEBUFFER_CLEAR, 3},

	{PrimitiveID::MONOCHROME_TRIANGLE, 4},

	{PrimitiveID::DRAWMODE_SETTINGS, 1},
	{PrimitiveID::TEXTURE_WINDOW_SETTINGS, 1},
	{PrimitiveID::SET_DRAW_TOPLEFT, 1},
	{PrimitiveID::SET_DRAW_BOTTOMRIGHT, 1},
	{PrimitiveID::SET_DRAWINGOFFSET, 1},
	{PrimitiveID::MASK_SETTINGS, 1},
};

Gpu::Gpu()
{
	_gpuControl.busyBit = 1;
	_gpuControl.readyToReceiveCommands = 1;
}

void Gpu::ProcessPrimitive()
{
	PrimitiveID primitiveId = (PrimitiveID)(_primitiveData[0] >> 24);

	switch (primitiveId)
	{
		case PrimitiveID::FRAMEBUFFER_CLEAR:
			break;

		case PrimitiveID::MONOCHROME_TRIANGLE:
			{
				std::cout << "Drawing monochrome triangle of color: " << std::hex << (_primitiveData[0] & 0x00FFFFFF) << std::endl << std::dec;
				std::cout << "v0 = (" << (_primitiveData[1] & 0x0000FFFF) << ", " << ((_primitiveData[1] & 0xFFFF0000) >> 16) << ") " << std::endl;
				std::cout << "v1 = (" << (_primitiveData[2] & 0x0000FFFF) << ", " << ((_primitiveData[2] & 0xFFFF0000) >> 16) << ") " << std::endl;
				std::cout << "v2 = (" << (_primitiveData[3] & 0x0000FFFF) << ", " << ((_primitiveData[3] & 0xFFFF0000) >> 16) << ") " << std::endl;
			}
			break;

		case PrimitiveID::DRAWMODE_SETTINGS:
			break;

		case PrimitiveID::TEXTURE_WINDOW_SETTINGS:
			break;

		case PrimitiveID::SET_DRAW_TOPLEFT:
			break;

		case PrimitiveID::SET_DRAW_BOTTOMRIGHT:
			break;

		case PrimitiveID::SET_DRAWINGOFFSET:
			break;

		case PrimitiveID::MASK_SETTINGS:
			break;

		default:
			throw std::runtime_error("Primitive not yet supported");
			break;
	}
}

uint8* Gpu::GetGpuDataAddress()
{
	return (uint8*) & _gpuData;
}

void Gpu::SetupCallback(Memory& memory)
{
	//writing to _gpuData is to draw a graphics primitive
	//writing to _gpuStatus is to change the state
	memory.SubscribeToRange(MemorySubscription(0x1f801810, sizeof(uint32) * 2, true, true, false,
	[this](uint32 address, AccessType accessType, uint32 data)
	{
		switch (address)
		{
			if (accessType == AccessType::WRITE)
			{

			case 0x1f801810:
				//graphics primitive
				{
					if(!_processingPrimitive)
					{
						_processingPrimitive = true;
						_primitiveData.clear();

						PrimitiveID primitiveId = (PrimitiveID)(data >> 24);

						if (primitiveSizes.find(primitiveId) == primitiveSizes.end())
							throw std::runtime_error("Primitive not found");

						_currentPrimitiveSize = primitiveSizes[primitiveId];

						_primitiveData.push_back(data);
						_currentPrimitiveSize--;
					}
					else
					{
						_primitiveData.push_back(data);
						_currentPrimitiveSize--;
					}

					if (_currentPrimitiveSize == 0)
					{
						_processingPrimitive = false;
						//actually process primitive now and send to vulkan
						ProcessPrimitive();
					}
				}
				break;

			case 0x1f801814:
				//changing state
				{
					uint8 command = data >> 24;
					uint32 parameter = data & 0x00FFFFFF;

					switch (command)
					{
						case 0x03:
							_gpuControl.displayEnabled = (DisplayEnabled)parameter;
						break;

						case 0x08:
							_gpuControl.width = (Width)(GET_BIT(6, parameter) + ((parameter & 0b11) << 1));

							_gpuControl.height = GET_BIT(2, parameter);
							_gpuControl.videoRegion = GET_BIT(3, parameter);
							_gpuControl.isrgb24 = GET_BIT(4, parameter);
							_gpuControl.isInterlaced = GET_BIT(5, parameter);
							break;
					}
				}
				break;
			}
		}

		//std::cout << "access of type " << (int)accessType << " at vram i / o port 0x" << std::hex << address << std::endl;
	}));
}
