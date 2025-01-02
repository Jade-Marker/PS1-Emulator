#pragma once

#include <vulkan/vulkan.h>
#include "Instance.hpp"

class DebugMessenger
{
private:
	VkDebugUtilsMessengerEXT _debugMessenger;

public:
	DebugMessenger();

	VkResult Create(const Instance& instance);
	void Destroy(const Instance& instance);

	static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
};

