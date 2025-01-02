#include "IndexBuffer.hpp"

VkResult IndexBuffer::Create(PhysicalDevice physicalDevice, Device device, VkCommandPool commandPool, Queue graphicsQueue, uint16 numIndices, const uint16* indices)
{
	Buffer stagingBuffer;
	uint32 size = numIndices * sizeof(uint16);
	VkResult result = stagingBuffer.Create(physicalDevice, device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, size);
	stagingBuffer.CopyData(device, size, (void*)indices);

	result = Buffer::Create(physicalDevice, device, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, size);

	stagingBuffer.CopyBuffer(device, commandPool, graphicsQueue, *this, size);

	stagingBuffer.Destroy(device);

	return result;
}

void IndexBuffer::Bind(VkCommandBuffer commandBuffer)
{
	vkCmdBindIndexBuffer(commandBuffer, _buffer, 0, VK_INDEX_TYPE_UINT16);
}
