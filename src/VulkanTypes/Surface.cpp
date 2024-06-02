#include "Surface.hpp"

Surface::Surface():
	_surface(VK_NULL_HANDLE)
{
}

VkResult Surface::Create(Instance instance, GLFWwindow* pWindow)
{
	return glfwCreateWindowSurface(instance.GetInstance(), pWindow, nullptr, &_surface);
}

void Surface::Destroy(Instance instance)
{
	vkDestroySurfaceKHR(instance.GetInstance(), _surface, nullptr);
}

const VkSurfaceKHR& Surface::GetSurface() const
{
	return _surface;
}
