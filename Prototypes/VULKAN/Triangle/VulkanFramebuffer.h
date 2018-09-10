#pragma once
class VulkanDevice;
class VulkanRenderPass;
class VulkanImageView;
class VulkanFramebuffer
{
public:
	VulkanFramebuffer();
	~VulkanFramebuffer();

	void Init(VulkanDevice& device, VulkanRenderPass& renderPass, uint32_t width, uint32_t height, uint32_t layer);
	void Destroy(VulkanDevice& device);

	void AddAttachement(VulkanImageView& imageView);

	operator VkFramebuffer() { return m_kFramebuffer; }

private:
	VkFramebuffer m_kFramebuffer;
	std::vector<VulkanImageView> m_akAttachements;
};

