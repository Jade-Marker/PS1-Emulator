#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Surface.hpp"

struct SwapChainSupportDetails
{
public:
    VkSurfaceCapabilitiesKHR capabilites;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, Surface surface);
};

