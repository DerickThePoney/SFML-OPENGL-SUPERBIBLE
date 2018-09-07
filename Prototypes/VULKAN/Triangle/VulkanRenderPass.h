#pragma once

enum ATTACHEMENT_TYPE
{
	ATTACHMENT_COLOR,
	ATTACHMENT_DEPTH,
	ATTACHMENT_RESOLVE
};

class VulkanDevice;
class VulkanRenderPass
{
public:
	VulkanRenderPass();
	~VulkanRenderPass();

	void Init(VulkanDevice& device);
	void Destroy(VulkanDevice& device);

	void AddAttachment(ATTACHEMENT_TYPE type, VkFormat format, VkSampleCountFlagBits sampleCount,
		VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, 
		VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp,
		VkImageLayout initialLayout, VkImageLayout finalLayout, VkImageLayout refLayout);
	void AddSubpass(VkPipelineBindPoint bindPoint, std::vector<uint32_t> colorRef, int32_t depthRef, std::vector<uint32_t> resolveRef);
	void AddSubpassDependency(uint32_t srcSubpass, uint32_t dstSupass,
		VkPipelineStageFlags srcStageMask, VkAccessFlags srcAccessMask,
		VkPipelineStageFlags dstStageMask, VkAccessFlags dstAccessMask,
		VkDependencyFlags dependencyFlags);

	size_t GetNumberOfAttachment() { return m_akImageAttachments.size(); }
	size_t GetSubpassNumber() { return m_akSubpassDescription.size(); }

	operator VkRenderPass() { return m_kRenderPassObject; }

private:
	VkRenderPass m_kRenderPassObject = VK_NULL_HANDLE;

	
	struct SubpassData
	{
		std::vector<VkAttachmentReference> colors = {};
		std::vector<VkAttachmentReference> resolve = {};
		int32_t depth;
	};
	std::vector<VkAttachmentDescription> m_akImageAttachments;
	std::vector<VkAttachmentReference> m_akColorImageAttachementRefs;
	std::vector<VkAttachmentReference> m_akDepthImageAttachementRefs;
	std::vector<VkAttachmentReference> m_akResolveImageAttachementRefs;
	std::vector<VkSubpassDescription> m_akSubpassDescription;
	std::vector<SubpassData> m_akSubpassData;
	std::vector<VkSubpassDependency> m_akSubpassDependencies;
};

