#include "stdafx.h"
#include "FullscreenQuadImageRendering.h"
#include "Triangle.h"



FullscreenQuadImageRendering::FullscreenQuadImageRendering()
{
}


FullscreenQuadImageRendering::~FullscreenQuadImageRendering()
{
}

void FullscreenQuadImageRendering::Initialise(VulkanImageView& view)
{
	std::vector<ShadersFileType> shaders;
	std::vector<ShadersFileType> shadersWater;
	ShadersFileType vert;
	vert.filepath = "data/shaders/FullscreenTriangle.vert.spv";
	vert.type = VERTEX;
	ShadersFileType frag;
	frag.filepath = "data/shaders/FullscreenTriangle.frag.spv";
	frag.type = FRAGMENT;
	shaders.push_back(vert);
	shaders.push_back(frag);

	pipeline.Initialise(VulkanRenderer::GetSurfaceExtent());
	pipeline.SetShaders(shaders.size(), shaders.data());
	pipeline.SetDepthTest(false, false);
	pipeline.CreatePipeline(&VulkanRenderer::GetDescriptorSetLayout(), VulkanRenderer::GetRenderPass(0));

	m.SetShaders(shaders, 1);
	set = m.GetDescriptorSet();

	
	VkDescriptorImageInfo imageinfo = {};
	imageinfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	imageinfo.sampler = HelloTriangleApplication::GetNoRepeatSampler();
	imageinfo.imageView = view;

	VkWriteDescriptorSet image = {};
	image.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	image.dstSet = set;
	image.dstArrayElement = 0;
	image.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	image.descriptorCount = 1;
	image.dstBinding = 1;
	image.pImageInfo = &imageinfo;
	vkUpdateDescriptorSets(VulkanRenderer::GetDevice(), 1, &image, 0, nullptr);

}

void FullscreenQuadImageRendering::Destroy()
{
	m.Destroy();
	pipeline.Destroy();
}

void FullscreenQuadImageRendering::Draw(VulkanCommandBuffer& buffer)
{
	buffer.BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	buffer.BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetLayout(), 0, 1, &set, 0, nullptr);
	buffer.DrawTriangles(3);
}
