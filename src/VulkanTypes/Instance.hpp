#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Types.hpp"

class Instance
{
private:
	VkInstance _instance;

private:
	std::vector<const char*> GetRequiredExtensions(bool enableValidationLayers);

public:
	Instance();

	VkResult Create(const char* appName, uint32 appVersion, const char* engineName, uint32 engineVersion, uint32 apiVersion,
		bool enableValidationLayers, const std::vector<const char*>& validationLayers);
	void Destroy();

	const VkInstance& GetInstance() const;
};

