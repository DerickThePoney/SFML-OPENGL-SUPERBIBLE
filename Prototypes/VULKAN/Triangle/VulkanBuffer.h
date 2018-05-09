#pragma once

class VulkanPhysicalDevice;
class VulkanBuffer
{
public:
	VulkanBuffer();
	~VulkanBuffer();

	void Init(VulkanPhysicalDevice& physicalDevice, const VkDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties);

	void MapBuffer(const VkDevice& device, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData);
	void UnMapBuffer(const VkDevice& device);

	void CopyBufferTo(const VulkanBuffer& other, const VkDevice& device, const VkCommandPool& transferPool, const VkQueue& queue);
	
	void Free(const VkDevice& device);

	operator VkBuffer() { return m_kBuffer; }

private:
	VkBuffer m_kBuffer;
	VkDeviceMemory m_kBufferMemory;

	VkDeviceSize m_uiSize;
};

