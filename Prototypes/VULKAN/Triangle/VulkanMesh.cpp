#include "stdafx.h"
#include "VulkanMesh.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "ObjectCreator.h"
#include "MeshData.h"



VulkanMesh::VulkanMesh()
{
}


VulkanMesh::~VulkanMesh()
{
}

void VulkanMesh::Initialise(MeshData & data, const VkCommandPool& commandPool, const VkQueue& queue)
{
	VkDeviceSize size = sizeof(data.vertices[0]) * data.vertices.size() + sizeof(data.indices[0]) * data.indices.size();

	IndexPositions = sizeof(data.vertices[0]) * data.vertices.size();
	IndexSize = data.indices.size();

	//create the staging buffer
	VulkanBuffer stagingBuffer;
	stagingBuffer.Init(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);


	//mem map
	stagingBuffer.CopyDataToBuffer(commandPool, queue, 0, 0, sizeof(data.vertices[0]) * data.vertices.size(), data.vertices.data());

	stagingBuffer.CopyDataToBuffer(commandPool, queue, IndexPositions, 0, sizeof(data.indices[0]) * data.indices.size(), data.indices.data());

	//actual buffer creation
	m_kData.Init(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	//copy the buffer
	stagingBuffer.CopyBufferTo(m_kData, commandPool, queue);

	stagingBuffer.Free();
}

void VulkanMesh::Destroy()
{
	m_kData.Free();
}

void VulkanMesh::Bind(VkCommandBuffer & buffer)
{
	VkBuffer vertexBuffers[] = { m_kData };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(buffer, m_kData, IndexPositions, VK_INDEX_TYPE_UINT32);
}

void VulkanMesh::Draw(VkCommandBuffer & buffer, uint32_t instanceCount)
{
	vkCmdDrawIndexed(buffer, IndexSize, instanceCount, 0, 0, 0);
}
