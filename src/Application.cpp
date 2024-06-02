#include "Application.hpp"

#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <fstream>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <vector>

#include "Memory.hpp"
#include "R3000A.hpp"
#include "Gpu.hpp"
#include "DMAController.hpp"
#include "IRQController.hpp"
#include "Constants.hpp"

#include "Device.hpp"
#include "PhysicalDevice.hpp"
#include "QueueFamilyIndices.hpp"
#include "SwapChainSupportDetails.hpp"
#include "Surface.hpp"
#include "ValidationLayers.hpp"
#include "DeviceExtensions.hpp"

#include "Vertex.hpp"

//todo
//Cleanup irq/dma/gpu into base memorymappped component class
//Optimise Memory so that getting a range is a lookup, not a search
//Get program running via bios reset
//Update instructions and cpu to properly match pipelining
//Get componentmanager running at given tick rate per second
//Get CPU running subset using lookup method
//Compare switch/case and lookup methods

uint8* ReadBinaryFile(uint32& size, const std::string& filePath)
{
    std::ifstream file = std::ifstream(filePath, std::ios::binary | std::ios::in | std::ios::ate);
    if (!file.is_open())
        throw std::runtime_error("could not open file");
    size = file.tellg();
    file.seekg(std::ios::beg);

    uint8* buffer = new uint8[size];
    file.read((char*)buffer, size);

    return buffer;
}

constexpr uint32 cPS1WIDTH = 320;
constexpr uint32 cPS1HEIGHT = 256;
constexpr uint32 cMULTIPLIER = 3;
constexpr uint32 cWIDTH = cPS1WIDTH * cMULTIPLIER;
constexpr uint32 cHEIGHT = cPS1HEIGHT * cMULTIPLIER;

std::vector<Vertex> vertices;
glm::vec4 renderClearColor = { 0,0,0,1 };
constexpr uint32 cMaxVertices = 2048;

struct UniformBufferObject
{
    alignas(16) glm::mat4 proj;
};

static std::vector<char> ReadFile(const std::string& fileName)
{
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("failed to open file!");

    uint32 fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

void Application::Run()
{
    InitWindow();
    InitVulkan();
    MainLoop();
    Cleanup();
}

void Application::FramebufferResizeCallback(GLFWwindow *pWindow, int width, int height)
{
    reinterpret_cast<Application*>(glfwGetWindowUserPointer(pWindow))->_frameBufferResized = true;
}

void Application::InitWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    
    _pWindow = glfwCreateWindow(cWIDTH, cHEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(_pWindow, this);
    glfwSetFramebufferSizeCallback(_pWindow, FramebufferResizeCallback);
}

void Application::InitVulkan()
{
    CreateInstance();
    SetupDebugMessenger();
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();
    _swapChain.Create(_device, _surface, _physicalDevice, _pWindow);
    _swapChain.CreateImageViews();
    CreateRenderPass();
    CreateDescriptorSetLayout();
    CreateGraphicsPipeline();
    _swapChain.CreateFramebuffers(renderPass);
    CreateCommandPool();
    _vertexBuffer.Create(_physicalDevice, _device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(Vertex) * cMaxVertices);
    _uniformBuffer.Create(_physicalDevice, _device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(UniformBufferObject));
    _uniformBufferMapped = _uniformBuffer.MapMemory(_device, sizeof(UniformBufferObject));
    CreateDescriptorPool();
    CreateDescriptorSets();
    CreateCommandBuffer();
    CreateSyncObjects();
}

void Application::MainLoop()
{
    uint32 programSize;	uint32 biosSize;
    uint8* program = ReadBinaryFile(programSize, "Executables/BasicGraphics.ps-exe");
    uint8* bios = ReadBinaryFile(biosSize, "OpenBios/openbios.bin");

    if (biosSize != 512 * 1024)
        throw std::runtime_error("BIOS incorrect size");

    Memory memory = Memory();
    uint8* ram = new uint8[2 * 1024 * 1024];
    memory.AddRange(MemoryRange(0x80000000, 2 * 1024 * 1024, ram));
    memory.AddRange(MemoryRange(0x00000000, 2 * 1024 * 1024, ram));
    memory.AddRange(MemoryRange(0xa0000000, 2 * 1024 * 1024, ram));
    memory.AddRange(MemoryRange(0x1f800000, 1024));
    memory.AddRange(MemoryRange(0xbfc00000, 512 * 1024));
    //memory.AddRange(MemoryRange(0x1f000000, 64 * 1024));

    uint32 programCounter = *(uint32*)(program + 8 + 16);

    R3000A cpu = R3000A(&memory, programCounter, std::array<uint32, 32>
    {
        0x00000000, 0x801FFF0, 0x00000000, 0x8011734, 0x8011884, 0x00000000, 0x00000000, 0x00000000,
            0X00006454, 0x000000F4, 0x000000B0, 0x80011600, 0xA0010004, 0x00000001, 0xFFFFFFFF, 0x00000000,
            0x00004374, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000010, 0x00000008, 0x1FC09238, 0x00000000, 0x00000000, 0x801FFFC8, 0x801FFFF0, 0x80116B8
    });

    Gpu gpu = Gpu(&vertices, &renderClearColor);
    DMAController dmaController = DMAController(&memory);
    IRQController irq = IRQController(&cpu);

    memory.AddRange(MemoryRange(0x1f801810, 2 * sizeof(uint32), gpu.GetGpuDataAddress()));
    memory.AddRange(MemoryRange(0x1f801080, sizeof(DMAController), dmaController.GetMemory()));
    memory.AddRange(MemoryRange(0xbf801070, 2, irq.GetMemory()));

    gpu.SetupCallback(memory);
    dmaController.SetupCallback(memory);
    irq.SetupCallback(memory);

    for (uint32 i = 0; i < 2 * 1024 * 1024; i++)
        ram[i] = 0;

    memory.Write(0x80010000, programSize - 2048, program + 2048);
    memory.Write(0xbfc00000, 512 * 1024, bios);


    while (!glfwWindowShouldClose(_pWindow))
    {
        glfwPollEvents();
        
        vertices.clear();

        cpu.Run();

        if(vertices.size() > 0)
            _vertexBuffer.CopyData(_device, sizeof(Vertex) * vertices.size(), vertices.data());

        DrawFrame();
    }

    _device.WaitIdle();
    delete[] program;
}

void Application::Cleanup()
{
    _imageAvailableSemaphore.Destroy(_device);
    _renderFinishedSemaphore.Destroy(_device);
    _inFlightFence.Destroy(_device);

    vkDestroyCommandPool(_device.GetDevice(), commandPool, nullptr);
    
    _swapChain.Destroy();
    CleanupGraphicsPipeline();
    vkDestroyDescriptorSetLayout(_device.GetDevice(), descriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(_device.GetDevice(), descriptorPool, nullptr);
    vkDestroyRenderPass(_device.GetDevice(), renderPass, nullptr);

    _surface.Destroy(_instance);
    _vertexBuffer.Destroy(_device);
    _uniformBuffer.Destroy(_device);

    if (cEnableValidationLayers)
        _debugMessenger.Destroy(_instance);

    _device.Destroy();
    _instance.Destroy();
    
    glfwDestroyWindow(_pWindow);
    glfwTerminate();
}

void Application::CreateInstance()
{
    if(cEnableValidationLayers && !CheckValidationLayerSupport())
        throw std::runtime_error("validation layers requested, but not available");

    if (_instance.Create("Hello triangle", VK_MAKE_VERSION(1, 0, 0), "No engine", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0,
        cEnableValidationLayers, validationLayers))
        throw std::runtime_error("unable to create instance");
}

void Application::CreateLogicalDevice()
{
    QueueFamilyIndices indices = QueueFamilyIndices::FindQueueFamilies(_physicalDevice, _surface);

    if (_device.Create(_physicalDevice, indices) != VK_SUCCESS)
        throw std::runtime_error("failed to create logical device!");

    _graphicsQueue.Create(_device, indices.graphicsFamily.value());
    _presentQueue.Create(_device, indices.presentFamily.value());
}

bool Application::CheckValidationLayerSupport()
{
    uint32 layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    
    for(const char* layerName : validationLayers)
    {
        bool layerFound = false;
        
        for(const auto& layerProperties : availableLayers)
        {
            if(strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }
        
        if(!layerFound)
            return false;
    }
    
    return true;
}

void Application::SetupDebugMessenger()
{
    if (!cEnableValidationLayers) return;

    if(_debugMessenger.Create(_instance) != VK_SUCCESS)
        throw std::runtime_error("failed to setup debug messenger!");
}

void Application::PickPhysicalDevice()
{
    if (_physicalDevice.Create(_instance, _surface) == VK_ERROR_INITIALIZATION_FAILED)
        throw std::runtime_error("failed to find a suitable GPU");
}

void Application::CreateSurface()
{
    if(_surface.Create(_instance, _pWindow) != VK_SUCCESS)
        throw std::runtime_error("failed to create window surface");
}

void Application::CreateDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;

    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(_device.GetDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
        throw std::runtime_error("failed to create descriptor layout");
}

void Application::RecreateSwapChainAndGraphicsPipeline()
{
    int width = 0; int height  = 0;
    glfwGetFramebufferSize(_pWindow, &width, &height);
    while (width == 0 && height == 0)
    {
        glfwGetFramebufferSize(_pWindow, &width, &height);
        glfwWaitEvents();
    }
    
    _device.WaitIdle();
    
    _swapChain.Destroy();
    
    _swapChain.Create(_device, _surface, _physicalDevice, _pWindow);
    _swapChain.CreateImageViews();
    _swapChain.CreateFramebuffers(renderPass);
    
    CleanupGraphicsPipeline();
    CreateGraphicsPipeline();
}

void Application::CreateGraphicsPipeline()
{
    auto vertShaderCode = ReadFile("resources/shaders/vert.spv");
    auto fragShaderCode = ReadFile("resources/shaders/frag.spv");

    VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    auto bindingDescription = Vertex::GetBindingDescription();
    auto attributeDescriptions = Vertex::GetAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    
    VkExtent2D swapChainExtent = _swapChain.GetExtent();
    
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainExtent.width;
    viewport.height = (float)swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0,0 };
    scissor.extent = swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};

    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(_device.GetDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        throw std::runtime_error("failed to create pipeline layout!");

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;

    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr;

    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(_device.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
        throw std::runtime_error("failed to create graphics pipeline");

    vkDestroyShaderModule(_device.GetDevice(), fragShaderModule, nullptr);
    vkDestroyShaderModule(_device.GetDevice(), vertShaderModule, nullptr);
}

void Application::CleanupGraphicsPipeline()
{
    vkDestroyPipeline(_device.GetDevice(), graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(_device.GetDevice(), pipelineLayout, nullptr);
}

VkShaderModule Application::CreateShaderModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(_device.GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        throw std::runtime_error("failed to create shader module!");

    return shaderModule;
}

void Application::CreateRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = _swapChain.GetImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;

    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;

    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(_device.GetDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        throw std::runtime_error("failed to create render pass!");
}

void Application::CreateCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = QueueFamilyIndices::FindQueueFamilies(_physicalDevice, _surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(_device.GetDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
        throw std::runtime_error("failed to create command pool!");
}

void Application::CreateCommandBuffer()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(_device.GetDevice(), &allocInfo, &commandBuffer) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate command buffers");
}

void Application::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32 imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("failed to begin recording command buffer!");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = _swapChain.GetFrameBuffer(imageIndex);

    renderPassInfo.renderArea.offset = { 0,0 };
    renderPassInfo.renderArea.extent = _swapChain.GetExtent();

    VkClearValue clearColor = { {{renderClearColor[0], renderClearColor[1], renderClearColor[2], renderClearColor[3]}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkBuffer vertexBuffers[] = { _vertexBuffer.GetBuffer()};
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    vkCmdDraw(commandBuffer, vertices.size(), 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        throw std::runtime_error("failed to record command buffer");
}

void Application::DrawFrame()
{
    _inFlightFence.WaitForFence(_device, UINT64_MAX);

    uint32 imageIndex;
    VkResult result = vkAcquireNextImageKHR(_device.GetDevice(), _swapChain.GetSwapChain(), UINT64_MAX, _imageAvailableSemaphore.GetSemaphore(), VK_NULL_HANDLE, &imageIndex);
    if(result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChainAndGraphicsPipeline();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("Unable to acquire swapchain image");

    _inFlightFence.ResetFence(_device);
    
    vkResetCommandBuffer(commandBuffer, 0);
    RecordCommandBuffer(commandBuffer, imageIndex);

    UpdateUniformBuffer();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { _imageAvailableSemaphore.GetSemaphore() };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore signalSemaphores[] = { _renderFinishedSemaphore.GetSemaphore() };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (_graphicsQueue.SubmitQueue(submitInfo, _inFlightFence) != VK_SUCCESS)
        throw std::runtime_error("failed to submit draw command buffer");

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { _swapChain.GetSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = nullptr;

    result = _presentQueue.PresentQueue(presentInfo);
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _frameBufferResized)
    {
        _frameBufferResized = false;
        RecreateSwapChainAndGraphicsPipeline();
    }
    else if (result != VK_SUCCESS)
        throw std::runtime_error("Unable to present swap chain image");
}

void Application::UpdateUniformBuffer()
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    auto extent = _swapChain.GetExtent();
    UniformBufferObject ubo{};
    ubo.proj = glm::ortho(0.0f, (float)cPS1WIDTH, 0.0f, (float)cPS1HEIGHT);

    std::memcpy(_uniformBufferMapped, &ubo, sizeof(UniformBufferObject));
}

void Application::CreateSyncObjects()
{
    if (_imageAvailableSemaphore.Create(_device) != VK_SUCCESS ||
        _renderFinishedSemaphore.Create(_device) != VK_SUCCESS ||
        _inFlightFence.Create(_device, VK_FENCE_CREATE_SIGNALED_BIT) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create semaphores!");
    }
}

void Application::CreateDescriptorPool()
{
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(_device.GetDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        throw std::runtime_error("failed to create descriptor pool!");
}

void Application::CreateDescriptorSets()
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    
    if (vkAllocateDescriptorSets(_device.GetDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate descriptor sets");

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = _uniformBuffer.GetBuffer();
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;

    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    
    descriptorWrite.pBufferInfo = &bufferInfo;
    descriptorWrite.pImageInfo = nullptr;
    descriptorWrite.pTexelBufferView = nullptr;
    
    vkUpdateDescriptorSets(_device.GetDevice(), 1, &descriptorWrite, 0, nullptr);
}
