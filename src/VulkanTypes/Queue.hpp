#pragma once

#include <vulkan/vulkan.h>
#include "Device.hpp"
#include "Types.hpp"
#include "Fence.hpp"

class Queue
{
private:
	VkQueue _queue;

public:
	Queue();

	void Create(Device device, uint32 queueFamilyIndex);

	VkResult SubmitQueue(const VkSubmitInfo& submitInfo, const Fence& fence);
	VkResult PresentQueue(const VkPresentInfoKHR& presentInfo);

	void WaitIdle();
};

