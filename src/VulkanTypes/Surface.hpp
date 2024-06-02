#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "Instance.hpp"

class Surface
{
private:
	VkSurfaceKHR _surface;

public:
	Surface();

	VkResult Create(Instance instance, GLFWwindow* pWindow);
	void Destroy(Instance instance);

	const VkSurfaceKHR& GetSurface() const;
};

