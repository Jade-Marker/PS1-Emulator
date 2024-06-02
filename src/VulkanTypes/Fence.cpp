#include "Fence.hpp"

Fence::Fence():
_fence(VK_NULL_HANDLE)
{
    
}

VkResult Fence::Create(Device device, VkFenceCreateFlags flags)
{
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = flags;
    
    return vkCreateFence(device.GetDevice(), &fenceInfo, nullptr, &_fence);
}

void Fence::Destroy(Device device)
{
    vkDestroyFence(device.GetDevice(), _fence, nullptr);
}

void Fence::WaitForFence(Device device, uint64 timeout)
{
    vkWaitForFences(device.GetDevice(), 1, &_fence, VK_TRUE, timeout);
}

void Fence::ResetFence(Device device)
{
    vkResetFences(device.GetDevice(), 1, &_fence);
}

const VkFence& Fence::GetFence() const
{
    return _fence;
}
