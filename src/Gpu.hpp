#pragma once
#include "Types.hpp"
#include "Memory.hpp"
#include "Vertex.hpp"

enum class PrimitiveID : uint8
{
	FRAMEBUFFER_CLEAR = 0x02,

	MONOCHROME_TRIANGLE = 0x20,

	GOURAUD_TRIANGLE = 0x30,

	DRAWMODE_SETTINGS = 0xe1,
	TEXTURE_WINDOW_SETTINGS,
	SET_DRAW_TOPLEFT,
	SET_DRAW_BOTTOMRIGHT,
	SET_DRAWINGOFFSET,
	MASK_SETTINGS
};

enum class Width : uint8
{
	WIDTH_256,
	WIDTH_384,
	WIDTH_320,
	
	WIDTH_INVALID,
	WIDTH_512,

	WIDTH_INVALID2,
	WIDTH_640
};

enum class DisplayEnabled : uint8
{
	DISPLAY_ON,
	DISPLAY_OFF
};

struct GpuControlRegister
{
	uint8 texturePageX : 4;
	uint8 texturePageY : 1;
	uint8 semiTransparentState : 2;
	uint8 texturePageColorMode : 2;
	uint8 ditheringOn : 1;
	uint8 drawToDisplayAreaAllowed : 1;
	uint8 applyMask : 1;
	uint8 useMask : 1;
	
	uint8 padding : 2;

	Width width: 3;
	uint8 height : 1;
	uint8 videoRegion : 1;
	uint8 isrgb24 : 1;
	uint8 isInterlaced : 1;
	DisplayEnabled displayEnabled : 1;

	uint8 padding2 : 2;

	uint8 busyBit : 1;
	uint8 readyToSendImage : 1;
	uint8 readyToReceiveCommands : 1;
	uint8 dmaDirection : 2;
	uint8 interlaceLinesToDraw : 1;
};

class Gpu
{
private:
	bool _processingPrimitive;
	std::vector<uint32> _primitiveData;
	uint32 _currentPrimitiveSize;
	std::vector<Vertex>* _vertices;
	glm::vec4* _clearColor;

	uint32 _gpuData;
	GpuControlRegister _gpuControl;

public:
	Gpu(std::vector<Vertex>* vertices, glm::vec4* clearColor);

	void ProcessPrimitive();

	uint8* GetGpuDataAddress();
	void SetupCallback(Memory& memory);

};

