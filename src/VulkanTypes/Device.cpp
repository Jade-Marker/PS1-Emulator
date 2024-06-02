#include <vector>
#include <set>
#include "Device.hpp"
#include "Types.hpp"
#include "ValidationLayers.hpp"
#include "DeviceExtensions.hpp"

Device::Device():
	_device(VK_NULL_HANDLE)
{
}

VkResult Device::Create(PhysicalDevice physicalDevice, QueueFamilyIndices indices)
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32 queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size());

    createInfo.pEnabledFeatures = &deviceFeatures;

    if (cEnableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    createInfo.enabledExtensionCount = static_cast<uint32>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    return vkCreateDevice(physicalDevice.GetDevice(), &createInfo, nullptr, &_device);
}

void Device::Destroy()
{
	vkDestroyDevice(_device, nullptr);
}

VkResult Device::WaitIdle()
{
	return vkDeviceWaitIdle(_device);
}

const VkDevice& Device::GetDevice() const
{
    return _device;
}
