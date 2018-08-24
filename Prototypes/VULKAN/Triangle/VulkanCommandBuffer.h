#pragma once
class VulkanDevice;
class VulkanBuffer;
class VulkanGraphicsPipeline;
class VulkanMesh;
class VulkanCommandBuffer
{
public:
	static VulkanCommandBuffer CreateCommandBuffer(VulkanDevice& device, VkCommandPool& pool, VkCommandBufferLevel level);
	static std::vector<VulkanCommandBuffer> CreateCommandBuffers(VulkanDevice& device, VkCommandPool& pool, VkCommandBufferLevel level, uint32_t number);

	static void Free(VulkanCommandBuffer& buffer, VulkanDevice& device, VkCommandPool& pool);
	static void Free(std::vector<VulkanCommandBuffer>& buffers, VulkanDevice& device, VkCommandPool& pool);

public:
	VulkanCommandBuffer();
	~VulkanCommandBuffer();

public:
	//Commands
	VkResult BeginCommandBuffer(VkCommandBufferUsageFlags flags, VkCommandBufferInheritanceInfo* inheritanceInfo);
	VkResult EndCommandBuffer();
	VkResult Reset(VkCommandBufferResetFlagBits flags);

	//render pass stuff
	void BeginRenderPass(
		VkRenderPass& pass,
		VkFramebuffer& frameBuffer,
		VkOffset2D& renderAreaOffset,
		VkExtent2D& renderAreaExtent,
		std::vector<VkClearValue>& clearValues,
		VkSubpassContents subpassContent
	);
	void EndRenderPass();

	void ExecuteCommandBuffer(const VulkanCommandBuffer* secondaryBuffers, uint32_t numberOfBuffers);
	void StartNextSubpass(VkSubpassContents subpassContent);

	//Pipeline stuff
	void BindPipeline(VkPipelineBindPoint bindPoint, VulkanGraphicsPipeline& pipeline);
	void BindDescriptorSets(
		VkPipelineBindPoint bindPoint,
		VkPipelineLayout& layout,
		uint32_t firstDescriptor,
		uint32_t descriptorCount,
		VkDescriptorSet* descriptors,
		uint32_t dynamicOffsetCount,
		const uint32_t* dynamicOffsets
	);

	//dynamic states
	void SetViewport(uint32_t firstViewport, uint32_t viewportCount, const VkViewport* viewports);
	void SetScissors(uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* scissors);

	void SetDepthBias(float depthBiasConstant, float depthBiasClamp, float depthBiasSlope);

	//draw calls
	void DrawMesh(VulkanMesh& mesh);


	//Barriers
	void PipelineBarrier
	(
		VkPipelineStageFlags srcStage,
		VkPipelineStageFlags dstStage,
		VkDependencyFlags dependencies,
		uint32_t memoryBarrierCount,
		VkMemoryBarrier* memoryBariers,
		uint32_t bufferMemoryBarrierCount,
		VkBufferMemoryBarrier* bufferMemoryBariers,
		uint32_t imageMemoryBarrierCount,
		VkImageMemoryBarrier* imageMemoryBariers
	);


	//Copy commands
	void CopyBufferToImage
	(
		VulkanBuffer& buffer,
		VkImage image,
		VkImageLayout dstImageLayout,
		uint32_t regionCount,
		const VkBufferImageCopy* regions
	);



public:
	const VkCommandBuffer* GetCommandBuffer() { return &m_kBufferObject; }



private:
	VkCommandBuffer m_kBufferObject;
	VkCommandBufferLevel level;
};

