#include "stdafx.h"
#include "VulkanBuffer.h"
#include "VulkanRenderer.h"


VulkanBuffer::VulkanBuffer()
{
}


VulkanBuffer::~VulkanBuffer()
{
}

void VulkanBuffer::Init(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties)
{
	const QueueFamilyIndices& indices = VulkanRenderer::GetPhysicalDevice().GetQueueFamilyIndices();
	//buffer creation
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;

	uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)indices.transferFamily };
	if (indices.graphicsFamily != indices.transferFamily)
	{
		bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
		bufferInfo.queueFamilyIndexCount = 2;
		bufferInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	if (vkCreateBuffer(VulkanRenderer::GetDevice(), &bufferInfo, nullptr, &m_kBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create the buffer");
	}

	//memory allocation 

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(VulkanRenderer::GetDevice(), m_kBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = VulkanRenderer::GetPhysicalDevice().FindMemoryType(memRequirements.memoryTypeBits, memoryProperties);

	if (vkAllocateMemory(VulkanRenderer::GetDevice(), &allocInfo, nullptr, &m_kBufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(VulkanRenderer::GetDevice(), m_kBuffer, m_kBufferMemory, 0);
	m_uiSize = size;
	m_eUsage = usage;
	m_eMemoryProperties = memoryProperties;
}

VkResult VulkanBuffer::MapBuffer(VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void ** ppData)
{
	return vkMapMemory(VulkanRenderer::GetDevice(), m_kBufferMemory, offset, size, flags, ppData);
}

VkResult VulkanBuffer::FlushMappedMemory()
{
	VkMappedMemoryRange range = {};
	range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	range.memory = m_kBufferMemory;
	range.size = m_uiSize;
	range.offset = 0;
	return vkFlushMappedMemoryRanges(VulkanRenderer::GetDevice(), 1, &range);
}

void VulkanBuffer::UnMapBuffer()
{
	vkUnmapMemory(VulkanRenderer::GetDevice(), m_kBufferMemory);
}

void VulkanBuffer::CopyDataToBuffer(const VkCommandPool& transferPool, const VkQueue& queue, VkDeviceSize offset, VkMemoryMapFlags flags, VkDeviceSize iDataSize, const void * pData)
{
	if (IsMappable())
	{
		void* data;
		MapBuffer(offset, iDataSize, flags, &data);
		memcpy(data, pData, (size_t)iDataSize);
		if (ShouldFlush()) FlushMappedMemory();
		UnMapBuffer();
	}
	else if(IsTransferDst())
	{
		VulkanBuffer staging;
		staging.Init(iDataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		void* data;
		staging.MapBuffer(0, iDataSize, 0, &data);
		memcpy(data, pData, (size_t)iDataSize);
		if (ShouldFlush()) FlushMappedMemory();
		staging.UnMapBuffer();

		staging.CopyBufferTo(*this, transferPool, queue);
	}
	else
	{
		//TODO: ERRORS
	}
	
}

void VulkanBuffer::CopyBufferTo(const VulkanBuffer & other, const VkCommandPool & transferPool, const VkQueue& queue)
{
	//start
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = transferPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(VulkanRenderer::GetDevice(), &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	//copy command
	VkBufferCopy bufferCopy = {};
	bufferCopy.srcOffset = 0;
	bufferCopy.dstOffset = 0;
	bufferCopy.size = m_uiSize;
	vkCmdCopyBuffer(commandBuffer, m_kBuffer, other.m_kBuffer, 1, &bufferCopy);


	//end
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(VulkanRenderer::GetDevice(), transferPool, 1, &commandBuffer);


}

void VulkanBuffer::Free()
{
	vkFreeMemory(VulkanRenderer::GetDevice(), m_kBufferMemory, nullptr);
	vkDestroyBuffer(VulkanRenderer::GetDevice(), m_kBuffer, nullptr);
	m_kBufferMemory = 0;
	m_kBuffer = 0;
}

bool VulkanBuffer::IsMappable()
{
	return m_eMemoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
}

bool VulkanBuffer::ShouldFlush()
{
	return !(m_eMemoryProperties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

bool VulkanBuffer::IsTransferSrc()
{
	return m_eUsage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
}

bool VulkanBuffer::IsTransferDst()
{
	return m_eUsage & VK_BUFFER_USAGE_TRANSFER_DST_BIT;
}
