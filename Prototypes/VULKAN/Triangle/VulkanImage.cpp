#include "stdafx.h"
#include "VulkanImage.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanImageView.h"
#include "VulkanRenderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


bool VulkanImage::HasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D16_UNORM_S8_UINT;
}

VulkanImage::VulkanImage()
{
}


VulkanImage::~VulkanImage()
{
}

void VulkanImage::Init(VulkanPhysicalDevice & physicalDevice, VulkanDevice & device, VkCommandPool & pool, VkQueue & graphicsQueue, VkQueue & transferQueue, const std::string & filename, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlags)
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	VkDeviceSize imageSize = texWidth * texHeight * 4;
	if (!pixels)
	{
		throw std::runtime_error("Failed to load image texture !");
	}

	//put it all into a staging buffer
	VulkanBuffer stagingBuffer;
	stagingBuffer.Init(physicalDevice, device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	stagingBuffer.CopyDataToBuffer(physicalDevice, device, pool, transferQueue, 0, 0, imageSize, pixels);

	stbi_image_free(pixels);

	//create the image
	Init(physicalDevice, device, texWidth, texHeight, format, imageTiling, usage, memoryFlags);

	TransitionImageLayout(device, VulkanRenderer::GetGraphicsPool(), VulkanRenderer::GetGraphicsQueue(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(device, VulkanRenderer::GetTranferPool(), VulkanRenderer::GetTransferQueue(), stagingBuffer);
	TransitionImageLayout(device, VulkanRenderer::GetGraphicsPool(), VulkanRenderer::GetGraphicsQueue(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	stagingBuffer.Free(device);
}

void VulkanImage::Init(VulkanPhysicalDevice & physicalDevice, VulkanDevice & device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlags)
{
	size = VkExtent3D{ width, height, 1 };
	imageFormat = format;
	this->imageTiling = imageTiling;
	usageFlags = usage;
	this->memoryFlags = memoryFlags;


	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent = size;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = imageFormat;
	imageInfo.tiling = imageTiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usageFlags;

	const QueueFamilyIndices& indices = physicalDevice.GetQueueFamilyIndices();
	uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)indices.transferFamily };
	if (indices.graphicsFamily != indices.transferFamily)
	{
		imageInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
		imageInfo.queueFamilyIndexCount = 2;
		imageInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}


	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0;

	if (vkCreateImage(device, &imageInfo, nullptr, &m_kImage) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create image object!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, m_kImage, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = physicalDevice.FindMemoryType(memRequirements.memoryTypeBits, memoryFlags);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &m_kImageMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(device, m_kImage, m_kImageMemory, 0);
}

void VulkanImage::Init(VkImage & imageHandle, uint32_t width, uint32_t height, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags usage)
{
	size = VkExtent3D{ width, height, 1 };
	imageFormat = format;
	this->imageTiling = imageTiling;
	usageFlags = usage;
	this->memoryFlags = memoryFlags;
	m_kImage = imageHandle;
}

void VulkanImage::Free(VulkanDevice& device)
{
	if (m_kImageMemory != VK_NULL_HANDLE)
	{
		vkDestroyImage(device, m_kImage, nullptr);
		vkFreeMemory(device, m_kImageMemory, nullptr);
	}
	
	m_kImage = VK_NULL_HANDLE;
	m_kImageMemory = VK_NULL_HANDLE;
}

void VulkanImage::CopyBufferToImage(VulkanDevice& device, VkCommandPool& pool, VkQueue& queue, VulkanBuffer & buffer)
{
	VulkanCommandBuffer commandBuffer = VulkanCommandBuffer::CreateCommandBuffer(device, pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	commandBuffer.BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr);

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = size;

	commandBuffer.CopyBufferToImage(
		buffer,
		m_kImage,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	commandBuffer.EndCommandBuffer();

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffer.GetCommandBuffer();

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	VulkanCommandBuffer::Free(commandBuffer, device, pool);
}

void VulkanImage::TransitionImageLayout(VulkanDevice& device, VkCommandPool& pool, VkQueue& queue, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VulkanCommandBuffer commandBuffer = VulkanCommandBuffer::CreateCommandBuffer(device, pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	commandBuffer.BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr);

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = m_kImage;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (HasStencilComponent(imageFormat)) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;


	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	commandBuffer.PipelineBarrier
	(
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);


	commandBuffer.EndCommandBuffer();

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffer.GetCommandBuffer();

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	VulkanCommandBuffer::Free(commandBuffer, device, pool);
}

VulkanImageView VulkanImage::CreateImageView(VulkanDevice & device, VkFormat format, VkImageAspectFlags aspectMask)
{
	VulkanImageView view;
	view.Init(device, *this, format, aspectMask);

	return view;
}
