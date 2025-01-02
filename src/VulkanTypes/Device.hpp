#pragma once

#include <vulkan/vulkan.h>
#include "PhysicalDevice.hpp"
#include "QueueFamilyIndices.hpp"

class Device
{
private:
	VkDevice _device;

public:
	Device();

	VkResult Create(PhysicalDevice physicalDevice, QueueFamilyIndices indices);
	void Destroy();

	VkResult WaitIdle();

	const VkDevice& GetDevice() const;
};

