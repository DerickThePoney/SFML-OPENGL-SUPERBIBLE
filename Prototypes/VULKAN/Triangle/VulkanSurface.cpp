#include "stdafx.h"
#include "VulkanSurface.h"

#include "VulkanInstance.h"

VulkanSurface::VulkanSurface()
{
}

VulkanSurface::~VulkanSurface()
{
}

void VulkanSurface::Init(VulkanInstance & instance, GLFWwindow * window)
{
	if (glfwCreateWindowSurface(instance, window, nullptr, &m_kSurface) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to retrieve surface for the window");
	}
}
void VulkanSurface::Destroy(VulkanInstance & instance)
{
	vkDestroySurfaceKHR(instance, m_kSurface, nullptr);
}
