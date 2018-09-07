#pragma once
class VulkanDevice;
class VulkanPhysicalDevice;
class VulkanBuffer;
class VulkanImageView;
class VulkanImage
{
public:
	static bool HasStencilComponent(VkFormat format);

public:
	VulkanImage();
	~VulkanImage();

	void Init(VulkanPhysicalDevice& physicalDevice, VulkanDevice& device, VkCommandPool& pool, VkQueue & graphicsQueue, VkQueue & transferQueue, const std::string& filename, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlags);
	void Init(VulkanPhysicalDevice& physicalDevice, VulkanDevice& device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlags);
	void Init(VkImage& imageHandle, uint32_t width, uint32_t height, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags usage);

	void Free(VulkanDevice& device);

	void CopyBufferToImage(VulkanDevice& device, VkCommandPool& pool, VkQueue& queue, VulkanBuffer& buffer);
	void TransitionImageLayout(VulkanDevice& device, VkCommandPool& pool, VkQueue& queue, VkImageLayout oldLayout, VkImageLayout newLayout);

	VulkanImageView CreateImageView(VulkanDevice& device, VkFormat format, VkImageAspectFlags aspectMask);

	operator VkImage() { return m_kImage; }
private:

	VkImage m_kImage;
	VkDeviceMemory m_kImageMemory = VK_NULL_HANDLE;

	VkExtent3D size;
	VkImageType type;
	VkFormat imageFormat;
	VkImageTiling imageTiling;
	VkImageUsageFlags usageFlags;
	VkMemoryPropertyFlags memoryFlags;
};

