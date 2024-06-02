#pragma once

#include "vulkan/vulkan.hpp"
#include "types.hpp"
#include "Device.hpp"
#include "PhysicalDevice.hpp"
#include "Queue.hpp"

class Buffer
{
protected:
	VkBuffer _buffer;
	VkDeviceMemory _bufferMemory;

	uint32 FindMemoryType(PhysicalDevice physicalDevice, uint32 typeFilter, VkMemoryPropertyFlags properties);

public:
	VkResult Create(PhysicalDevice physicalDevice, Device device, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties, uint32 size);
	void Destroy(Device device);

	void CopyData(Device device, uint32 size, void* dataSrc);
	void CopyBuffer(Device device, VkCommandPool commandPool, Queue graphicsQueue, Buffer& dstBuffer, VkDeviceSize size);
	void* MapMemory(Device device, uint32 size);

	VkBuffer GetBuffer();
};

