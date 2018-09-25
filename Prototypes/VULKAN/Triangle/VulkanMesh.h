#pragma once

#include "VulkanBuffer.h"
class VulkanPhysicalDevice;
class VulkanDevice;

struct MeshData
{
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
};


class VulkanMesh
{
public:
	VulkanMesh();
	~VulkanMesh();

	void Initialise(VulkanPhysicalDevice& physicalDevice, VulkanDevice& device, MeshData& data, const VkCommandPool& commandPool, const VkQueue& queue);
	void Destroy(VulkanDevice& device);

	void Bind(VkCommandBuffer& buffer);
	void Draw(VkCommandBuffer& buffer, uint32_t instanceCount);

private:
	VulkanBuffer m_kData;
	uint32_t IndexPositions;
	uint32_t IndexSize;
};

