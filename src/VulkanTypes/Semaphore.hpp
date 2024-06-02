#pragma once

#include <vulkan/vulkan.h>
#include "Device.hpp"

class Semaphore
{
private:
    VkSemaphore _semaphore;
    
public:
    Semaphore();
    
    VkResult Create(Device device);
    void Destroy(Device device);
    
    const VkSemaphore& GetSemaphore();
};
