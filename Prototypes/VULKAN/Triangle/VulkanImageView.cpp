#include "stdafx.h"
#include "VulkanImageView.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"

VulkanImageView::VulkanImageView()
{
}


VulkanImageView::~VulkanImageView()
{
}

void VulkanImageView::Init(VulkanDevice & device, VulkanImage& image, VkFormat format, VkImageAspectFlags aspectMask, uint32_t mipLevels)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectMask;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(device, &viewInfo, nullptr, &m_kImageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}
}

void VulkanImageView::Destroy(VulkanDevice & device)
{
	vkDestroyImageView(device, m_kImageView, nullptr);
}
