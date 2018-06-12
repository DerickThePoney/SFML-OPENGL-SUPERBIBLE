#pragma once

class VulkanPhysicalDevice;

class VulkanDevice
{

	NONCOPYABLE(VulkanDevice);

public:
	VulkanDevice();
	~VulkanDevice();

	void Initialise(VulkanPhysicalDevice& physicalDevice, 
					bool enableValidationLayers, 
					const std::vector<const char*> validationLayers, 
					const std::vector<const char*> deviceExtensions);

	operator VkDevice() { return m_kDevice; }

	void GetQueue(uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue& queue);

private:
	VkDevice m_kDevice = VK_NULL_HANDLE;
};

