#include <set>
#include <string>
#include "PhysicalDevice.hpp"
#include "Types.hpp"
#include "QueueFamilyIndices.hpp"
#include "SwapChainSupportDetails.hpp"
#include "DeviceExtensions.hpp"

bool PhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device, Surface surface)
{
    PhysicalDevice physicalDevice;
    physicalDevice._device = device;
    
    QueueFamilyIndices indices = QueueFamilyIndices::FindQueueFamilies(physicalDevice, surface);

    bool extensionsSupported = CheckDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = SwapChainSupportDetails::QuerySwapChainSupport(device, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.IsComplete() && extensionsSupported && swapChainAdequate;
}

bool PhysicalDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32 extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
}

PhysicalDevice::PhysicalDevice():
    _device(VK_NULL_HANDLE)
{
}

VkResult PhysicalDevice::Create(Instance instance, Surface surface)
{
    uint32 deviceCount = 0;
    vkEnumeratePhysicalDevices(instance.GetInstance(), &deviceCount, nullptr);

    if (deviceCount == 0)
        return VK_ERROR_INITIALIZATION_FAILED;

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance.GetInstance(), &deviceCount, devices.data());

    for (const VkPhysicalDevice& device : devices)
    {
        if (IsDeviceSuitable(device, surface))
        {
            _device = device;
            break;
        }
    }

    if (_device == VK_NULL_HANDLE)
        return VK_ERROR_INITIALIZATION_FAILED;
    
    return VK_SUCCESS;
}

const VkPhysicalDevice& PhysicalDevice::GetDevice() const
{
    return _device;
}
