#pragma once

class VulkanInstance;
class VulkanSurface;

class VulkanPhysicalDevice
{
	NONCOPYABLE(VulkanPhysicalDevice);

public:
	VulkanPhysicalDevice();
	~VulkanPhysicalDevice();

	void PickDevice(VulkanInstance& instance, VulkanSurface& surface, const std::vector<const char*> deviceExtensions);

	operator VkPhysicalDevice() { return m_kPhysicalDevice; }

	
	const SwapChainSupportDetails& GetSwapchainSupportDetails() { return m_kSwapChainSupportDetails; }
	const QueueFamilyIndices& GetQueueFamilyIndices() { return m_kIndices; }

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	VkFormat FindSupportedFormats(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	
private:
	bool IsDeviceSuitable(VkPhysicalDevice device, VulkanSurface& surface, const std::vector<const char*> deviceExtensions);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VulkanSurface& surface);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*> deviceExtensions);
	SwapChainSupportDetails QuerySwapChainSupportDetails(VkPhysicalDevice device, VulkanSurface& surface);

private:
	VkPhysicalDevice m_kPhysicalDevice = VK_NULL_HANDLE;
	QueueFamilyIndices m_kIndices;
	SwapChainSupportDetails m_kSwapChainSupportDetails;
};