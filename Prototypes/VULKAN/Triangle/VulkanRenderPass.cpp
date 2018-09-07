#include "stdafx.h"
#include "VulkanRenderPass.h"
#include "VulkanDevice.h"


VulkanRenderPass::VulkanRenderPass()
{
}


VulkanRenderPass::~VulkanRenderPass()
{
}

void VulkanRenderPass::Init(VulkanDevice& device)
{
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(m_akImageAttachments.size());
	renderPassInfo.pAttachments = m_akImageAttachments.data();
	renderPassInfo.subpassCount = m_akSubpassDescription.size();
	renderPassInfo.pSubpasses = m_akSubpassDescription.data();
	renderPassInfo.dependencyCount = m_akSubpassDependencies.size();
	renderPassInfo.pDependencies = m_akSubpassDependencies.data();

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_kRenderPassObject) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create the render pass!");
	}
}

void VulkanRenderPass::Destroy(VulkanDevice& device)
{
	vkDestroyRenderPass(device, m_kRenderPassObject, nullptr);
	m_akImageAttachments.clear();
	m_akColorImageAttachementRefs.clear();
	m_akDepthImageAttachementRefs.clear();
	m_akResolveImageAttachementRefs.clear();
	m_akSubpassDescription.clear();
	m_akSubpassData.clear();
	m_akSubpassDependencies.clear();
}

void VulkanRenderPass::AddAttachment(ATTACHEMENT_TYPE type, VkFormat format, VkSampleCountFlagBits sampleCount,
	VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, 
	VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp,
	VkImageLayout initialLayout, VkImageLayout finalLayout, VkImageLayout refLayout)
{
	VkAttachmentDescription attachment = {};
	attachment.format = format;
	attachment.samples = sampleCount;
	attachment.loadOp = loadOp;
	attachment.storeOp = storeOp;

	attachment.stencilLoadOp = stencilLoadOp;
	attachment.stencilStoreOp = stencilStoreOp;

	attachment.initialLayout = initialLayout;
	attachment.finalLayout = finalLayout;
	m_akImageAttachments.push_back(attachment);
	
	VkAttachmentReference attachmentRef = {};
	attachmentRef.attachment = m_akImageAttachments.size()-1;
	attachmentRef.layout = refLayout;
	switch (type)
	{
	case ATTACHEMENT_TYPE::ATTACHMENT_COLOR:
		m_akColorImageAttachementRefs.push_back(attachmentRef);
		break;
	case ATTACHEMENT_TYPE::ATTACHMENT_DEPTH:
		m_akDepthImageAttachementRefs.push_back(attachmentRef);
		break;
	case ATTACHEMENT_TYPE::ATTACHMENT_RESOLVE:
		m_akResolveImageAttachementRefs.push_back(attachmentRef);
		break;
	}
	
}

void VulkanRenderPass::AddSubpass(VkPipelineBindPoint bindPoint, std::vector<uint32_t> colorRef, int32_t depthRef, std::vector<uint32_t> resolveRef)
{
	SubpassData data = {};

	for (size_t i = 0; i < colorRef.size(); ++i)
	{
		data.colors.push_back(m_akColorImageAttachementRefs[colorRef[i]]);
	}
	for (size_t i = 0; i < resolveRef.size(); ++i)
	{
		data.resolve.push_back(m_akResolveImageAttachementRefs[resolveRef[i]]);
	}

	data.depth = depthRef;
	m_akSubpassData.push_back(data);

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = bindPoint;
	subpass.colorAttachmentCount = m_akSubpassData[m_akSubpassData.size()-1].colors.size();
	subpass.pColorAttachments = m_akSubpassData[m_akSubpassData.size() - 1].colors.data();
	subpass.pDepthStencilAttachment = (m_akSubpassData[m_akSubpassData.size() - 1].depth == -1) ? nullptr : &m_akDepthImageAttachementRefs[depthRef]; //TODO: MAKE SURE THERE IS ONLY ONE
	subpass.pResolveAttachments = m_akSubpassData[m_akSubpassData.size() - 1].resolve.data();

	m_akSubpassDescription.push_back(subpass);
	
}

void VulkanRenderPass::AddSubpassDependency(
	uint32_t srcSubpass, uint32_t dstSupass,
	VkPipelineStageFlags srcStageMask, VkAccessFlags srcAccessMask,
	VkPipelineStageFlags dstStageMask, VkAccessFlags dstAccessMask,
	VkDependencyFlags dependencyFlags
)
{
	if (srcSubpass != VK_SUBPASS_EXTERNAL && srcSubpass >= m_akSubpassDescription.size())
	{
		throw std::runtime_error("Unknown subpass for subpass dependency");
	}
	if (dstSupass != VK_SUBPASS_EXTERNAL && dstSupass >= m_akSubpassDescription.size())
	{
		throw std::runtime_error("Unknown subpass for subpass dependency");
	}

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = srcSubpass;
	dependency.dstSubpass = dstSupass;
	dependency.srcStageMask = srcStageMask;
	dependency.srcAccessMask = srcAccessMask;
	dependency.dstStageMask = dstStageMask;
	dependency.dstAccessMask = dstAccessMask;
	dependency.dependencyFlags = dependencyFlags;
	m_akSubpassDependencies.push_back(dependency);
}
