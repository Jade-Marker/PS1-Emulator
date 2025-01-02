#pragma once

#include "Buffer.hpp"

//Index and vertex buffer can actually be stored together in the same buffer
//This is more cache friendly
//But for now, keep them separate

class IndexBuffer : public Buffer
{
public:
    VkResult Create(PhysicalDevice physicalDevice, Device device, VkCommandPool commandPool, Queue graphicsQueue, uint16 numIndices, const uint16* indices);

    void Bind(VkCommandBuffer commandBuffer);
};