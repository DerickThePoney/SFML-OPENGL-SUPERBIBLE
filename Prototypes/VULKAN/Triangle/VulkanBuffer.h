#pragma once

class VulkanPhysicalDevice;
class VulkanBuffer
{
public:
	VulkanBuffer();
	~VulkanBuffer();

	void Init(VulkanPhysicalDevice& physicalDevice, const VkDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties);

	VkResult MapBuffer(const VkDevice& device, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData);
	VkResult FlushMappedMemory(const VkDevice& device);
	void UnMapBuffer(const VkDevice& device);
	void CopyDataToBuffer(VulkanPhysicalDevice& physicalDevice, const VkDevice& device, const VkCommandPool& transferPool, const VkQueue& queue, VkDeviceSize offset, VkMemoryMapFlags flags, VkDeviceSize iDataSize, const void *pData);


	void CopyBufferTo(const VulkanBuffer& other, const VkDevice& device, const VkCommandPool& transferPool, const VkQueue& queue);
	
	void Free(const VkDevice& device);

	operator VkBuffer() { return m_kBuffer; }

	bool IsMappable();
	bool ShouldFlush();
	bool IsTransferSrc();
	bool IsTransferDst();

private:
	VkBuffer m_kBuffer;
	VkDeviceMemory m_kBufferMemory;

	VkDeviceSize m_uiSize;
	VkBufferUsageFlags m_eUsage;
	VkMemoryPropertyFlags m_eMemoryProperties;
};

