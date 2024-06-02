#include "Semaphore.hpp"

Semaphore::Semaphore(): _semaphore(VK_NULL_HANDLE)
{
    
}

VkResult Semaphore::Create(Device device)
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    return vkCreateSemaphore(device.GetDevice(), &semaphoreInfo, nullptr, &_semaphore);
}

void Semaphore::Destroy(Device device)
{
    vkDestroySemaphore(device.GetDevice(), _semaphore, nullptr);
}

const VkSemaphore& Semaphore::GetSemaphore()
{
    return _semaphore;
}
