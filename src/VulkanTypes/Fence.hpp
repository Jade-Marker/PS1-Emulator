#pragma once

#include <vulkan/vulkan.h>
#include "Types.hpp"
#include "Device.hpp"

class Fence{
private:
    VkFence _fence;
    
public:
    Fence();
    
    VkResult Create(Device device, VkFenceCreateFlags flags);
    void Destroy(Device device);
    
    void WaitForFence(Device device, uint64 timeout);
    void ResetFence(Device device);
    
    const VkFence& GetFence() const;
};
