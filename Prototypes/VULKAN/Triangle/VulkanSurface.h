#pragma once

class VulkanInstance;

class VulkanSurface
{
	NONCOPYABLE(VulkanSurface);
public:
	VulkanSurface();
	~VulkanSurface();

	void Init(VulkanInstance& instance, GLFWwindow* window);

	void Destroy(VulkanInstance& instance);

	operator VkSurfaceKHR() { return m_kSurface; }

private:
	VkSurfaceKHR m_kSurface;
};