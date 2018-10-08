#pragma once

class VulkanShader;
class VulkanDevice;
class VulkanRenderPass;
enum SHADER_TYPE;

struct ShadersFileType
{
	std::string filepath;
	SHADER_TYPE type;
};



class VulkanGraphicsPipeline
{
public:
	VulkanGraphicsPipeline();
	~VulkanGraphicsPipeline();

	VulkanGraphicsPipeline(const VulkanGraphicsPipeline& other);

	void Initialise(VkExtent2D& extent);

	//Shaders stuff
	void SetShaders(uint32_t shaderFileTypeCount, ShadersFileType* akShaders);
	void SetSpecialisationForShader(uint32_t idx, VkSpecializationInfo info);
	void ResetSpecialisations();

	void SetViewport(VkViewport& v);
	void SetScissors(VkRect2D& v);

	//Input Assembly
	void SetTopology(VkPrimitiveTopology v = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	void SetPrimitiveRestart(bool v = false);

	//Rasteriser state
	void SetDepthClampEnable(bool v = false);
	void SetRasterizerDiscardEnable(bool v = false);
	void SetPolygonMode(VkPolygonMode v = VK_POLYGON_MODE_FILL,  float lineWidth = 1.0f);
	void SetCullMode(VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT);
	void SetFrontFace(VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE);
	void SetDepthBias(bool bEnable = false, float depthBiasConstantFactor = 0.0f, float depthBiasClamp = 0.0f, float depthBiasSlopeFactor = 0.0f);

	// multisampling
	void SetMultisampling(bool bEnabled = false, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT, float minSampleShading = 1.0f, const VkSampleMask* mask = nullptr, bool bAlphaToCoverage = false, bool alphaToOne = false);
	
	//blending
	void SetBlendAttachementCount(uint32_t count);
	void SetBlendingForAttachement(uint32_t idx, VkColorComponentFlags colorMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, bool bBlendEnable = false,
		VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_ONE, VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, VkBlendOp colorBlendOp = VK_BLEND_OP_ADD,
		VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE, VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD
	);

	//Depth test
	void SetDepthTest(bool bEnabledTest, bool bEnabledWrite);

	//dynamic state
	void SetDynamicStates(const VkDynamicState* states, uint32_t dynamicStateCount);

	void CreatePipeline(VkDescriptorSetLayout* descriptorSetLayout, VulkanRenderPass& renderPass);
	void Destroy();

	operator VkPipeline() { return m_kGraphicsPipeline; }

	VkPipelineLayout& GetLayout() { return m_kPipelineLayout; }

private:
	void InitialiseDefaultValues(VkExtent2D& swapChainExtent);

private:
	VkPipelineLayout m_kPipelineLayout;
	VkPipeline m_kGraphicsPipeline;

	struct DataCache
	{
		//temp data
		VulkanShader* shaders;
		VkVertexInputBindingDescription vertexBinding;
		std::vector<VkVertexInputAttributeDescription> vertexAttributes;
		VkViewport viewport;
		VkRect2D scissor;
		std::vector<VkSpecializationInfo> specialisationInfos;

		//pipeline info
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		VkPipelineVertexInputStateCreateInfo vertexInputInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssembly;
		//no need for VkPipelineViewportStateCreateInfo -->create at runtime
		VkPipelineRasterizationStateCreateInfo rasterizer;
		VkPipelineMultisampleStateCreateInfo multisampling;
		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlending;
		VkPipelineDepthStencilStateCreateInfo depthStencil;
		VkPipelineDynamicStateCreateInfo dynamicState;
	};

	DataCache m_kData;

};

