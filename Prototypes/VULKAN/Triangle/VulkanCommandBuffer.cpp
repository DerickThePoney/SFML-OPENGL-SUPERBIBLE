#include "stdafx.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanBuffer.h"
#include "VulkanMesh.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanImage.h"


VulkanCommandBuffer VulkanCommandBuffer::CreateCommandBuffer(VulkanDevice& device, VkCommandPool & pool, VkCommandBufferLevel level)
{
	//commandBuffers.resize(swapchainFrameBuffers.size());
	VkCommandBuffer buffer;
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = pool;
	allocInfo.level = level;
	allocInfo.commandBufferCount = (uint32_t)1;

	if (vkAllocateCommandBuffers(device, &allocInfo, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Command buffers allocation");
	}

	VulkanCommandBuffer ret;
	ret.m_kBufferObject = buffer;
	ret.level = level;

	return ret;
}

std::vector<VulkanCommandBuffer> VulkanCommandBuffer::CreateCommandBuffers(VulkanDevice& device, VkCommandPool & pool, VkCommandBufferLevel level, uint32_t number)
{
	std::vector<VkCommandBuffer> commandBuffers;
	commandBuffers.resize(number);
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = pool;
	allocInfo.level = level;
	allocInfo.commandBufferCount = number;

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Command buffers allocation");
	}

	std::vector<VulkanCommandBuffer> ret;
	ret.resize(number);
	for (size_t i = 0; i < ret.size(); ++i)
	{
		ret[i].m_kBufferObject = commandBuffers[i];
		ret[i].level = level;
	}

	return ret;
}

void VulkanCommandBuffer::Free(VulkanCommandBuffer & buffer, VulkanDevice & device, VkCommandPool & pool)
{
	vkFreeCommandBuffers(device, pool, 1, &buffer.m_kBufferObject);
}

void VulkanCommandBuffer::Free(std::vector<VulkanCommandBuffer>& buffers, VulkanDevice & device, VkCommandPool & pool)
{
	std::vector<VkCommandBuffer> commandBuffers;
	commandBuffers.resize(buffers.size());
	for (size_t i = 0; i < buffers.size(); ++i)
	{
		commandBuffers[i] = buffers[i].m_kBufferObject;
	}

	vkFreeCommandBuffers(device, pool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	buffers.clear();
}

VkResult VulkanCommandBuffer::Reset(std::vector<VulkanCommandBuffer>& buffers, VkCommandBufferResetFlagBits flags)
{
	for (size_t i = 0; i < buffers.size(); ++i)
	{
		VkResult res = buffers[i].Reset(flags);
		if (res != VK_SUCCESS)
		{
			std::cout << "resetting buffer did not work" << std::endl;
		}
	}
	return VK_SUCCESS;
}

VulkanCommandBuffer::VulkanCommandBuffer()
{
}


VulkanCommandBuffer::~VulkanCommandBuffer()
{
}

VkResult VulkanCommandBuffer::BeginCommandBuffer(VkCommandBufferUsageFlags flags, VkCommandBufferInheritanceInfo * inheritanceInfo)
{
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = flags;
	beginInfo.pInheritanceInfo = inheritanceInfo; // Optional

	return vkBeginCommandBuffer(m_kBufferObject, &beginInfo);
}

VkResult VulkanCommandBuffer::EndCommandBuffer()
{
	return vkEndCommandBuffer(m_kBufferObject);
}

VkResult VulkanCommandBuffer::Reset(VkCommandBufferResetFlagBits flags)
{
	return vkResetCommandBuffer(m_kBufferObject, flags);
}

void VulkanCommandBuffer::BeginRenderPass(VulkanRenderPass & pass, VulkanFramebuffer & frameBuffer, VkOffset2D & renderAreaOffset, VkExtent2D & renderAreaExtent, std::vector<VkClearValue>& clearValues, VkSubpassContents subpassContent)
{
	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = pass;
	renderPassInfo.framebuffer = frameBuffer;

	renderPassInfo.renderArea.offset = renderAreaOffset;
	renderPassInfo.renderArea.extent = renderAreaExtent;

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(m_kBufferObject, &renderPassInfo, subpassContent);
}

void VulkanCommandBuffer::EndRenderPass()
{
	vkCmdEndRenderPass(m_kBufferObject);
}

void VulkanCommandBuffer::ExecuteCommandBuffer(const VulkanCommandBuffer* secondaryBuffers, uint32_t numberOfBuffers)
{
	if (level != VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY)
	{
		throw std::runtime_error("Trying to execute a secondary buffer from a non primary buffer !");
	}

	std::vector <VkCommandBuffer> buffers;
	for (uint32_t i = 0; i < numberOfBuffers; ++i)
	{
		if (secondaryBuffers[i].level != VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_SECONDARY)
		{
			throw std::runtime_error("Trying to execute a secondary buffer from a non primary buffer !");
		}
		buffers.push_back(secondaryBuffers[i].m_kBufferObject);
	}

	vkCmdExecuteCommands(m_kBufferObject, numberOfBuffers, buffers.data());
}

void VulkanCommandBuffer::StartNextSubpass(VkSubpassContents subpassContent)
{
	vkCmdNextSubpass(m_kBufferObject, subpassContent);
}

void VulkanCommandBuffer::BindPipeline(VkPipelineBindPoint bindPoint, VulkanGraphicsPipeline & pipeline)
{
	vkCmdBindPipeline(m_kBufferObject, bindPoint, pipeline);
}

void VulkanCommandBuffer::BindDescriptorSets(VkPipelineBindPoint bindPoint, VkPipelineLayout & layout, uint32_t firstDescriptor, uint32_t descriptorCount, VkDescriptorSet * descriptors, uint32_t dynamicOffsetCount, const uint32_t * dynamicOffsets)
{
	vkCmdBindDescriptorSets(m_kBufferObject, bindPoint, layout, firstDescriptor, descriptorCount, descriptors, dynamicOffsetCount, dynamicOffsets);
}

void VulkanCommandBuffer::SetViewport(uint32_t firstViewport, uint32_t viewportCount, const VkViewport * viewports)
{
	vkCmdSetViewport(m_kBufferObject, firstViewport, viewportCount, viewports);
}

void VulkanCommandBuffer::SetScissors(uint32_t firstScissor, uint32_t scissorCount, const VkRect2D * scissors)
{
	vkCmdSetScissor(m_kBufferObject, firstScissor, scissorCount, scissors);
}

void VulkanCommandBuffer::SetDepthBias(float depthBiasConstant, float depthBiasClamp, float depthBiasSlope)
{
	vkCmdSetDepthBias(m_kBufferObject, depthBiasConstant, depthBiasClamp, depthBiasSlope);
}

void VulkanCommandBuffer::DrawMesh(VulkanMesh& mesh)
{
	mesh.Bind(m_kBufferObject);
	mesh.Draw(m_kBufferObject,1);
}

void VulkanCommandBuffer::DrawTriangles(const uint32_t number)
{
	vkCmdDraw(m_kBufferObject, number, 1, 0, 0);
}

void VulkanCommandBuffer::PipelineBarrier(VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, VkDependencyFlags dependencies, uint32_t memoryBarrierCount, VkMemoryBarrier * memoryBariers, uint32_t bufferMemoryBarrierCount, VkBufferMemoryBarrier * bufferMemoryBariers, uint32_t imageMemoryBarrierCount, VkImageMemoryBarrier * imageMemoryBariers)
{
	vkCmdPipelineBarrier(
		m_kBufferObject,
		srcStage, dstStage,
		dependencies,
		memoryBarrierCount, memoryBariers,
		bufferMemoryBarrierCount, bufferMemoryBariers,
		imageMemoryBarrierCount, imageMemoryBariers
	);
}

void VulkanCommandBuffer::CopyBufferToImage(VulkanBuffer & buffer, VkImage image, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy * regions)
{
	vkCmdCopyBufferToImage(
		m_kBufferObject,
		buffer,
		image,
		dstImageLayout,
		regionCount,
		regions
	);
}

void VulkanCommandBuffer::BlitImage(VulkanImage & imageSrc, VkImageLayout layoutSrc, VulkanImage & imageDst, VkImageLayout layoutDst, uint32_t regionCount, const VkImageBlit * pRegions, VkFilter filter)
{
	vkCmdBlitImage(
		m_kBufferObject,
		imageSrc, layoutSrc,
		imageDst, layoutDst,
		regionCount, pRegions,
		filter
	);
}

void VulkanCommandBuffer::BlitImage(VkImage & imageSrc, VkImageLayout layoutSrc, VkImage & imageDst, VkImageLayout layoutDst, uint32_t regionCount, const VkImageBlit * pRegions, VkFilter filter)
{
	vkCmdBlitImage(
		m_kBufferObject,
		imageSrc, layoutSrc,
		imageDst, layoutDst,
		regionCount, pRegions,
		filter
	);
}
