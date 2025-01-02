#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Types.hpp"
#include "Surface.hpp"
#include "PhysicalDevice.hpp"
#include "Device.hpp"

class SwapChain{
private:
    std::vector<VkFramebuffer> _swapChainFramebuffers;
    std::vector<VkImageView> _swapChainImageViews;
    std::vector<VkImage> _swapChainImages;
    VkFormat _swapChainImageFormat;
    VkExtent2D _swapChainExtent;
    VkSwapchainKHR _swapChain;
    
    Device _device;

private:
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* pWindow);
    
public:
    VkResult Create(Device device, Surface surface, PhysicalDevice physicalDevice, GLFWwindow* pWindow);
    void Destroy();
    
    void CreateImageViews();
    void CreateFramebuffers(VkRenderPass renderPass);
    
    VkSwapchainKHR GetSwapChain();
    VkExtent2D GetExtent();
    VkFramebuffer GetFrameBuffer(uint32 index);
    VkFormat GetImageFormat();
};
