#pragma once
#include <vulkan/vulkan.h>
#include <optional>
#include "Types.hpp"
#include "Surface.hpp"
#include "PhysicalDevice.hpp"

struct QueueFamilyIndices
{
public:
    std::optional<uint32> graphicsFamily;
    std::optional<uint32> presentFamily;

    bool IsComplete();

    static QueueFamilyIndices FindQueueFamilies(PhysicalDevice physicalDevice, Surface surface);
};

