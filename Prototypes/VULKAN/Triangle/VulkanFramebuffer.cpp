#include "stdafx.h"
#include "VulkanFramebuffer.h"
#include "VulkanDevice.h"
#include "VulkanRenderPass.h"
#include "VulkanImageView.h"



VulkanFramebuffer::VulkanFramebuffer()
{
}


VulkanFramebuffer::~VulkanFramebuffer()
{
}

void VulkanFramebuffer::Init(VulkanDevice & device, VulkanRenderPass& renderPass, uint32_t width, uint32_t height, uint32_t layer)
{
	std::vector<VkImageView> iv;
	for (size_t i = 0; i < m_akAttachements.size(); ++i)
	{
		iv.push_back(m_akAttachements[i]);
	}

	VkFramebufferCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.renderPass = renderPass;
	createInfo.attachmentCount = static_cast<uint32_t>(iv.size());
	createInfo.pAttachments = iv.data();
	createInfo.width = width;
	createInfo.height = height;
	createInfo.layers = layer;

	if (vkCreateFramebuffer(device, &createInfo, nullptr, &m_kFramebuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to build frame buffers");
	}
}

void VulkanFramebuffer::Destroy(VulkanDevice & device)
{
	vkDestroyFramebuffer(device, m_kFramebuffer, nullptr);
	m_akAttachements.clear();
}

void VulkanFramebuffer::AddAttachement(VulkanImageView & imageView)
{
	m_akAttachements.push_back(imageView);
}
