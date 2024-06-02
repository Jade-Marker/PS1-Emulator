#include "Buffer.hpp"

uint32 Buffer::FindMemoryType(PhysicalDevice physicalDevice, uint32 typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice.GetDevice(), &memProperties);

	for (uint32 i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	throw std::runtime_error("failed to find suitable memory type");
}

VkResult Buffer::Create(PhysicalDevice physicalDevice, Device device, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties, uint32 size)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateBuffer(device.GetDevice(), &bufferInfo, nullptr, &_buffer);
	if (result != VK_SUCCESS)
		throw std::runtime_error("failed to create buffer");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device.GetDevice(), _buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, memoryProperties);

	//Not actually supposed to call vkAllocate memory for each buffer, but this is fine for now
	result = vkAllocateMemory(device.GetDevice(), &allocInfo, nullptr, &_bufferMemory);
	if (result != VK_SUCCESS)
		throw std::runtime_error("failed to allocate buffer memory");

	vkBindBufferMemory(device.GetDevice(), _buffer, _bufferMemory, 0);

	return VK_SUCCESS;
}

void Buffer::Destroy(Device device)
{
	vkDestroyBuffer(device.GetDevice(), _buffer, nullptr);
	vkFreeMemory(device.GetDevice(), _bufferMemory, nullptr);
}

void Buffer::CopyData(Device device, uint32 size, void* dataSrc)
{
	void* dataDst;
	vkMapMemory(device.GetDevice(), _bufferMemory, 0, size, 0, &dataDst);
	std::memcpy(dataDst, dataSrc, size);
	vkUnmapMemory(device.GetDevice(), _bufferMemory);
}

void Buffer::CopyBuffer(Device device, VkCommandPool commandPool, Queue graphicsQueue, Buffer& dstBuffer, VkDeviceSize size)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device.GetDevice(), &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, _buffer, dstBuffer._buffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	graphicsQueue.SubmitQueue(submitInfo, Fence());
	graphicsQueue.WaitIdle();

	vkFreeCommandBuffers(device.GetDevice(), commandPool, 1, &commandBuffer);
}

void* Buffer::MapMemory(Device device, uint32 size)
{
	void* pMemory;
	vkMapMemory(device.GetDevice(), _bufferMemory, 0, size, 0, &pMemory);

	return pMemory;
}

VkBuffer Buffer::GetBuffer()
{
	return _buffer;
}
