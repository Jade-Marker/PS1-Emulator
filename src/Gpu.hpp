#pragma once
#include "Types.hpp"
#include "Memory.hpp"

class Gpu
{
private:
	uint32 _gpuData;
	uint32 _gpuStatus;

public:
	Gpu();

	uint8* GetGpuDataAddress();
	void SetupCallback(Memory& memory);

};

