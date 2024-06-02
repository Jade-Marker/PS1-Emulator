#pragma once

#include <vulkan/vulkan.h>
#include "Instance.hpp"
#include "Surface.hpp"

class PhysicalDevice
{
private:
	VkPhysicalDevice _device;

private:
	bool IsDeviceSuitable(VkPhysicalDevice device, Surface surface);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

public:
	PhysicalDevice();

	VkResult Create(Instance instance, Surface surface);

	const VkPhysicalDevice& GetDevice() const;
};

