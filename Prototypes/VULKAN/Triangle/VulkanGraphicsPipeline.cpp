#include "stdafx.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanDevice.h"
#include "VulkanShader.h"
#include "VulkanRenderPass.h"
#include "VulkanRenderer.h"


VulkanGraphicsPipeline::VulkanGraphicsPipeline()
{
}


VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(const VulkanGraphicsPipeline & other)
{
	m_kData = other.m_kData;
	m_kData.shaders = nullptr;
}

void VulkanGraphicsPipeline::Initialise(VkExtent2D& extent)
{
	InitialiseDefaultValues(extent);
}

void VulkanGraphicsPipeline::SetShaders(uint32_t shaderFileTypeCount, ShadersFileType * akShaders)
{
	m_kData.shaders = new VulkanShader[shaderFileTypeCount];
	m_kData.shaderStages.clear();
	for (uint32_t i = 0; i < shaderFileTypeCount; ++i)
	{
		m_kData.shaders[i].Initialise(VulkanRenderer::GetDevice(), akShaders[i].type, akShaders[i].filepath.c_str());
		

		VkPipelineShaderStageCreateInfo shagerStageInfo = {};
		shagerStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		switch (akShaders[i].type)
		{
		case VERTEX:
			shagerStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			break;
		case TESSELATION_CONTROL:
			shagerStageInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			break;
		case TESSELATION_EVALUATION:
			shagerStageInfo.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			break;
		case GEOMETRY:
			shagerStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
			break;
		case FRAGMENT:
			shagerStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		case COMPUTE:
			throw std::runtime_error("Compute shader impossible in graphics pipeline");
			break;
		}
		shagerStageInfo.module = m_kData.shaders[i];
		shagerStageInfo.pName = "main";

		m_kData.shaderStages.push_back(shagerStageInfo);
	}
}

void VulkanGraphicsPipeline::SetSpecialisationForShader(uint32_t idx, VkSpecializationInfo info)
{
	if (idx >= m_kData.shaderStages.size())
	{
		throw std::runtime_error("Trying to specialisation for unspecified shader stage");
	}

	m_kData.specialisationInfos.push_back(info);
	m_kData.shaderStages[idx].pSpecializationInfo = &m_kData.specialisationInfos[m_kData.specialisationInfos.size() - 1];
}

void VulkanGraphicsPipeline::ResetSpecialisations()
{
	m_kData.specialisationInfos.clear();
	for (size_t i = 0; i < m_kData.shaderStages.size(); ++i)
	{
		m_kData.shaderStages[i].pSpecializationInfo = nullptr;
	}
}

void VulkanGraphicsPipeline::SetViewport(VkViewport & v)
{
	m_kData.viewport = v;
}

void VulkanGraphicsPipeline::SetScissors(VkRect2D & v)
{
	m_kData.scissor = v;
}

void VulkanGraphicsPipeline::SetTopology(VkPrimitiveTopology v)
{
	m_kData.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

void VulkanGraphicsPipeline::SetPrimitiveRestart(bool v)
{
	m_kData.inputAssembly.primitiveRestartEnable = (v)? VK_TRUE : VK_FALSE;
}

void VulkanGraphicsPipeline::SetDepthClampEnable(bool v)
{
	m_kData.rasterizer.depthClampEnable = (v) ? VK_TRUE : VK_FALSE;
}

void VulkanGraphicsPipeline::SetRasterizerDiscardEnable(bool v)
{
	m_kData.rasterizer.rasterizerDiscardEnable = (v) ? VK_TRUE : VK_FALSE;;
}

void VulkanGraphicsPipeline::SetPolygonMode(VkPolygonMode v, float lineWidth)
{
	m_kData.rasterizer.polygonMode = v;
	m_kData.rasterizer.lineWidth = lineWidth;
}

void VulkanGraphicsPipeline::SetCullMode(VkCullModeFlags cullMode)
{
	m_kData.rasterizer.cullMode = cullMode;
}

void VulkanGraphicsPipeline::SetFrontFace(VkFrontFace frontFace)
{
	m_kData.rasterizer.frontFace = frontFace;
}

void VulkanGraphicsPipeline::SetDepthBias(bool bEnable, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
	m_kData.rasterizer.depthBiasEnable = VK_FALSE;
	m_kData.rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	m_kData.rasterizer.depthBiasClamp = 0.0f; // Optional
	m_kData.rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
}

void VulkanGraphicsPipeline::SetMultisampling(bool bEnabled, VkSampleCountFlagBits sampleCount, float minSampleShading, const VkSampleMask * mask, bool bAlphaToCoverage, bool alphaToOne)
{
	m_kData.multisampling.sampleShadingEnable = bEnabled;
	m_kData.multisampling.rasterizationSamples = sampleCount;
	m_kData.multisampling.minSampleShading = minSampleShading; // Optional
	m_kData.multisampling.pSampleMask = mask; // Optional
	m_kData.multisampling.alphaToCoverageEnable = bAlphaToCoverage; // Optional
	m_kData.multisampling.alphaToOneEnable = alphaToOne; // Optional

}

void VulkanGraphicsPipeline::SetBlendAttachementCount(uint32_t count)
{
	m_kData.colorBlending.attachmentCount = count;

	m_kData.colorBlendAttachment.clear();
	for (uint32_t i = 0; i < count; ++i)
	{
		VkPipelineColorBlendAttachmentState color = {};
		color.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		color.blendEnable = VK_FALSE;
		color.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		color.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		color.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		color.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		color.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		color.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
		m_kData.colorBlendAttachment.push_back(color);
	}

	m_kData.colorBlending.pAttachments = m_kData.colorBlendAttachment.data();
}

void VulkanGraphicsPipeline::SetBlendingForAttachement(uint32_t idx, VkColorComponentFlags colorMask, bool bBlendEnable, 
	VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor, VkBlendOp colorBlendOp,
	VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor, VkBlendOp alphaBlendOp)
{
	if (idx >= m_kData.colorBlending.attachmentCount)
	{
		throw std::runtime_error("Trying to set blending for an unknown or unintialised color blend state object");
	}

	VkPipelineColorBlendAttachmentState& state = m_kData.colorBlendAttachment[idx];
	state.colorWriteMask = colorMask;
	state.blendEnable = (bBlendEnable)? VK_TRUE : VK_FALSE;
	state.srcColorBlendFactor = srcColorBlendFactor; // Optional
	state.dstColorBlendFactor = dstColorBlendFactor; // Optional
	state.colorBlendOp = colorBlendOp; // Optional
	state.srcAlphaBlendFactor = srcAlphaBlendFactor; // Optional
	state.dstAlphaBlendFactor = dstAlphaBlendFactor; // Optional
	state.alphaBlendOp = alphaBlendOp; // Optional
}

void VulkanGraphicsPipeline::SetDepthTest(bool bEnabledTest, bool bEnabledWrite)
{
	m_kData.depthStencil.depthTestEnable = bEnabledTest;
	m_kData.depthStencil.depthWriteEnable = bEnabledWrite;
}

void VulkanGraphicsPipeline::SetDynamicStates(const VkDynamicState * states, uint32_t dynamicStateCount)
{
	m_kData.dynamicState.dynamicStateCount = dynamicStateCount;
	m_kData.dynamicState.pDynamicStates = states;
}

void VulkanGraphicsPipeline::CreatePipeline(VkDescriptorSetLayout* descriptorSetLayout, VulkanRenderPass& renderPass)
{
	//create the meta struct for viewport and scissor
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &m_kData.viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &m_kData.scissor;

	//create the layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1; // Optional
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayout; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(VulkanRenderer::GetDevice(), &pipelineLayoutInfo, nullptr, &m_kPipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}


	//create graphics pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = static_cast<uint32_t>(m_kData.shaderStages.size());
	pipelineInfo.pStages = m_kData.shaderStages.data();

	pipelineInfo.pVertexInputState = &m_kData.vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &m_kData.inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &m_kData.rasterizer;
	pipelineInfo.pMultisampleState = &m_kData.multisampling;
	pipelineInfo.pDepthStencilState = &m_kData.depthStencil;
	pipelineInfo.pColorBlendState = &m_kData.colorBlending;
	pipelineInfo.pDynamicState = (m_kData.dynamicState.dynamicStateCount == 0) ? nullptr : &m_kData.dynamicState;

	pipelineInfo.layout = m_kPipelineLayout;

	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;

	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	if (vkCreateGraphicsPipelines(VulkanRenderer::GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_kGraphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to build graphics pipeline!");
	}
}

void VulkanGraphicsPipeline::Destroy()
{
	for (size_t i = 0; i < m_kData.shaderStages.size(); ++i)
	{
		m_kData.shaders[i].Destroy(VulkanRenderer::GetDevice());
	}
	delete[] m_kData.shaders;

	vkDestroyPipeline(VulkanRenderer::GetDevice(), m_kGraphicsPipeline, nullptr);
	m_kGraphicsPipeline = VK_NULL_HANDLE;
	vkDestroyPipelineLayout(VulkanRenderer::GetDevice(), m_kPipelineLayout, nullptr);
}

void VulkanGraphicsPipeline::InitialiseDefaultValues(VkExtent2D& swapChainExtent)
{
	//default vertexInput stage
	m_kData.vertexBinding = VertexData::getBindingDescription();
	m_kData.vertexAttributes = VertexData::GetAttributeDescriptions();
	m_kData.vertexInputInfo = {};
	m_kData.vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	m_kData.vertexInputInfo.vertexBindingDescriptionCount = 1;
	m_kData.vertexInputInfo.pVertexBindingDescriptions = &m_kData.vertexBinding; // Optional
	m_kData.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_kData.vertexAttributes.size());;
	m_kData.vertexInputInfo.pVertexAttributeDescriptions = m_kData.vertexAttributes.data(); // Optional

	//default viewport and scissors
	m_kData.viewport.x = 0.0f;
	m_kData.viewport.y = 0.0f;
	m_kData.viewport.width = (float)swapChainExtent.width;
	m_kData.viewport.height = (float)swapChainExtent.height;
	m_kData.viewport.minDepth = 0.0f;
	m_kData.viewport.maxDepth = 1.0f;

	m_kData.scissor.offset = { 0, 0 };
	m_kData.scissor.extent = swapChainExtent;

	//Default input assemply
	m_kData.inputAssembly = {};
	m_kData.inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	m_kData.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	m_kData.inputAssembly.primitiveRestartEnable = VK_FALSE;

	//default rasterizer
	m_kData.rasterizer = {};
	m_kData.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	m_kData.rasterizer.depthClampEnable = VK_FALSE;
	m_kData.rasterizer.rasterizerDiscardEnable = VK_FALSE;
	m_kData.rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	m_kData.rasterizer.lineWidth = 1.0f;
	m_kData.rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	m_kData.rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	m_kData.rasterizer.depthBiasEnable = VK_FALSE;
	m_kData.rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	m_kData.rasterizer.depthBiasClamp = 0.0f; // Optional
	m_kData.rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	//default multisampling
	m_kData.multisampling = {};
	m_kData.multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	m_kData.multisampling.sampleShadingEnable = VK_FALSE;
	m_kData.multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	m_kData.multisampling.minSampleShading = 1.0f; // Optional
	m_kData.multisampling.pSampleMask = nullptr; // Optional
	m_kData.multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	m_kData.multisampling.alphaToOneEnable = VK_FALSE; // Optional

	//default color blending
	m_kData.colorBlendAttachment = { {} };
	m_kData.colorBlendAttachment[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	m_kData.colorBlendAttachment[0].blendEnable = VK_FALSE;
	m_kData.colorBlendAttachment[0].srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	m_kData.colorBlendAttachment[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	m_kData.colorBlendAttachment[0].colorBlendOp = VK_BLEND_OP_ADD; // Optional
	m_kData.colorBlendAttachment[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	m_kData.colorBlendAttachment[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	m_kData.colorBlendAttachment[0].alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	m_kData.colorBlending = {};
	m_kData.colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	m_kData.colorBlending.logicOpEnable = VK_FALSE;
	m_kData.colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	m_kData.colorBlending.attachmentCount = m_kData.colorBlendAttachment.size();
	m_kData.colorBlending.pAttachments = m_kData.colorBlendAttachment.data();
	m_kData.colorBlending.blendConstants[0] = 0.0f; // Optional
	m_kData.colorBlending.blendConstants[1] = 0.0f; // Optional
	m_kData.colorBlending.blendConstants[2] = 0.0f; // Optional
	m_kData.colorBlending.blendConstants[3] = 0.0f; // Optional

	//default depth stencil
	m_kData.depthStencil = {};
	m_kData.depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	m_kData.depthStencil.depthTestEnable = VK_TRUE;
	m_kData.depthStencil.depthWriteEnable = VK_TRUE;
	m_kData.depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	m_kData.depthStencil.depthBoundsTestEnable = VK_FALSE;
	m_kData.depthStencil.minDepthBounds = 0.0f;
	m_kData.depthStencil.maxDepthBounds = 1.0f;

	m_kData.depthStencil.stencilTestEnable = VK_FALSE;
	m_kData.depthStencil.front = {};
	m_kData.depthStencil.back = {};

	//should do dynamic stuff
	m_kData.dynamicState = {};
	m_kData.dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	m_kData.dynamicState.pDynamicStates = nullptr;
	m_kData.dynamicState.dynamicStateCount = 0;
	m_kData.dynamicState.flags = 0;

}
