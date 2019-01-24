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

	void Init(uint32_t width, uint32_t height, void* pData, VkDeviceSize size, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlags, bool bGenerateMipMaps = false);
	void Init(const std::string& filename, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlags, bool bGenerateMipMaps = false);
	void Init(uint32_t width, uint32_t height, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlags, bool bGenerateMipMaps = false);
	void Init(VkImage& imageHandle, uint32_t width, uint32_t height, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags usage, bool bGenerateMipMaps = false);

	void Free();

	void CopyNewData(uint32_t width, uint32_t height, void* pData, VkDeviceSize size, bool bGenerateMipMaps = false);
	void CopyBufferToImage(VkCommandPool& pool, VkQueue& queue, VulkanBuffer& buffer);
	void TransitionImageLayout(VkCommandPool& pool, VkQueue& queue, VkImageLayout oldLayout, VkImageLayout newLayout);

	VulkanImageView CreateImageView(VkFormat format, VkImageAspectFlags aspectMask);

	operator VkImage() { return m_kImage; }
private:

	void GenerateMipMaps();

	VkImage m_kImage;
	VkDeviceMemory m_kImageMemory = VK_NULL_HANDLE;
	VkImageLayout currentLayout;

	VkExtent3D size;
	VkImageType type;
	VkFormat imageFormat;
	VkImageTiling imageTiling;
	VkImageUsageFlags usageFlags;
	VkMemoryPropertyFlags memoryFlags;
	uint32_t mipLevels;
};

