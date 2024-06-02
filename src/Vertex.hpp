#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

struct Vertex {
    glm::ivec2 pos;
    glm::ivec3 color;

    static VkVertexInputBindingDescription GetBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
};

