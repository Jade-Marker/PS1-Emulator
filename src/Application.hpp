#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

#include "Types.hpp"
#include "Fence.hpp"
#include "Semaphore.hpp"
#include "Instance.hpp"
#include "DebugMessenger.hpp"
#include "Queue.hpp"
#include "SwapChain.hpp"
#include "IndexBuffer.hpp"

class Application{
public:
    void Run();
    
private:
    static void FramebufferResizeCallback(GLFWwindow* pWindow, int width, int height);
    
    void InitWindow();
    void InitVulkan();
    void MainLoop();
    void Cleanup();

    void CreateInstance();
    void CreateLogicalDevice();
    bool CheckValidationLayerSupport();
    void SetupDebugMessenger();
    void PickPhysicalDevice();
    void CreateSurface();
    void CreateDescriptorSetLayout();
    void RecreateSwapChainAndGraphicsPipeline();
    void CreateGraphicsPipeline();
    void CleanupGraphicsPipeline();
    VkShaderModule CreateShaderModule(const std::vector<char>& code);
    void CreateRenderPass();
    void CreateCommandPool();
    void CreateCommandBuffer();
    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32 imageIndex);
    void DrawFrame();
    void UpdateUniformBuffer();
    void CreateSyncObjects();
    void CreateDescriptorPool();
    void CreateDescriptorSets();

private:
    GLFWwindow* _pWindow;
    Instance _instance;
    DebugMessenger _debugMessenger;
    PhysicalDevice _physicalDevice;
    Device _device;

    Queue _graphicsQueue;
    Surface _surface;
    Queue _presentQueue;
    SwapChain _swapChain;

    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;

    bool _frameBufferResized;
    Semaphore _imageAvailableSemaphore;
    Semaphore _renderFinishedSemaphore;
    Fence _inFlightFence;

    Buffer _vertexBuffer;
    Buffer _uniformBuffer;
    void* _uniformBufferMapped;
};
