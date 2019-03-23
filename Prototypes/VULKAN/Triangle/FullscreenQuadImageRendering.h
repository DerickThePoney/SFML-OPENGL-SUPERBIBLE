#pragma once
#include "VulkanRenderer.h"
#include "Material.h"

class VulkanGraphicsPipeline;
class Material;

class FullscreenQuadImageRendering
{
public:
	FullscreenQuadImageRendering();
	~FullscreenQuadImageRendering();

	void Initialise(VulkanImageView& view);
	void Destroy();

	void Draw(VulkanCommandBuffer& buffer);
private:
	VulkanGraphicsPipeline pipeline;
	Material m;
	VkDescriptorSet set;
};

