#pragma once

class VulkanPhysicalDevice;
class VulkanDevice;
class VulkanBuffer
{
public:
	VulkanBuffer();
	~VulkanBuffer();

	void Init(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties);

	VkResult MapBuffer(VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData);
	VkResult FlushMappedMemory();
	void UnMapBuffer();
	void CopyDataToBuffer(const VkCommandPool& transferPool, const VkQueue& queue, VkDeviceSize offset, VkMemoryMapFlags flags, VkDeviceSize iDataSize, const void *pData);


	void CopyBufferTo(const VulkanBuffer& other, const VkCommandPool& transferPool, const VkQueue& queue);
	
	void Free();

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

