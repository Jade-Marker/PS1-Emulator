#include "SwapChainSupportDetails.hpp"

SwapChainSupportDetails SwapChainSupportDetails::QuerySwapChainSupport(VkPhysicalDevice device, Surface surface)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface.GetSurface(), &details.capabilites);

    uint32 formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.GetSurface(), &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.GetSurface(), &formatCount, details.formats.data());
    }

    uint32 presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.GetSurface(), &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.GetSurface(), &presentModeCount, details.presentModes.data());
    }

    return details;
}
