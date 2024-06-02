#include "Queue.hpp"

Queue::Queue():
	_queue(VK_NULL_HANDLE)
{
}

void Queue::Create(Device device, uint32 queueFamilyIndex)
{
	vkGetDeviceQueue(device.GetDevice(), queueFamilyIndex, 0, &_queue);
}

VkResult Queue::SubmitQueue(const VkSubmitInfo& submitInfo, const Fence& fence)
{
	return vkQueueSubmit(_queue, 1, &submitInfo, fence.GetFence());
}

VkResult Queue::PresentQueue(const VkPresentInfoKHR& presentInfo)
{
	return vkQueuePresentKHR(_queue, &presentInfo);
}

void Queue::WaitIdle()
{
	vkQueueWaitIdle(_queue);
}
