#pragma once

class VulkanDevice;
class VulkanImage;
class VulkanImageView
{
public:
	VulkanImageView();
	~VulkanImageView();

	void Init(VulkanDevice & device, VulkanImage& image, VkFormat format, VkImageAspectFlags aspectMask);
	void Destroy(VulkanDevice & device);

	operator VkImageView() { return m_kImageView; }

private:
	VkImageView m_kImageView;
};

