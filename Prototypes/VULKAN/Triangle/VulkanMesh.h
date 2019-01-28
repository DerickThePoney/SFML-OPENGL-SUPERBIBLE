#pragma once

#include "VulkanBuffer.h"
class VulkanPhysicalDevice;
class VulkanDevice;
struct MeshData;

class VulkanMesh
{
public:
	VulkanMesh();
	~VulkanMesh();

	void Initialise(MeshData& data, const VkCommandPool& commandPool, const VkQueue& queue);
	void Destroy();

	void Bind(VkCommandBuffer& buffer);
	void Draw(VkCommandBuffer& buffer, uint32_t instanceCount);

private:
	VulkanBuffer m_kData;
	uint32_t IndexPositions;
	uint32_t IndexSize;
};

