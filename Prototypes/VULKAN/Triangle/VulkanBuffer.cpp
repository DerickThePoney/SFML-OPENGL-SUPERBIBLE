#include "stdafx.h"
#include "VulkanBuffer.h"
#include "VulkanPhysicalDevice.h"


VulkanBuffer::VulkanBuffer()
{
}


VulkanBuffer::~VulkanBuffer()
{
}

void VulkanBuffer::Init(VulkanPhysicalDevice& physicalDevice, const VkDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties)
{
	const QueueFamilyIndices& indices = physicalDevice.GetQueueFamilyIndices();
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

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_kBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create the buffer");
	}

	//memory allocation 

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, m_kBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = physicalDevice.FindMemoryType(memRequirements.memoryTypeBits, memoryProperties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &m_kBufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(device, m_kBuffer, m_kBufferMemory, 0);
	m_uiSize = size;
}

void VulkanBuffer::MapBuffer(const VkDevice & device, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void ** ppData)
{
	vkMapMemory(device, m_kBufferMemory, offset, size, flags, ppData);
}

void VulkanBuffer::UnMapBuffer(const VkDevice & device)
{
	vkUnmapMemory(device, m_kBufferMemory);
}

void VulkanBuffer::CopyBufferTo(const VulkanBuffer & other, const VkDevice & device, const VkCommandPool & transferPool, const VkQueue& queue)
{
	//start
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = transferPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

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

	vkFreeCommandBuffers(device, transferPool, 1, &commandBuffer);


}

void VulkanBuffer::Free(const VkDevice& device)
{
	vkFreeMemory(device, m_kBufferMemory, nullptr);
	vkDestroyBuffer(device, m_kBuffer, nullptr);
}
