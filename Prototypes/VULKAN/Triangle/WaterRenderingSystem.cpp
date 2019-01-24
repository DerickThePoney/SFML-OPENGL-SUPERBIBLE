#include "stdafx.h"
#include "WaterRenderingSystem.h"

#include "MeshData.h"
#include "Intersections.h"

#include "Triangle.h"

/*WaterRenderingSystem::WaterRenderingSystem()
{
}


WaterRenderingSystem::~WaterRenderingSystem()
{
}*/

void WaterRenderingSystem::Initialise(float fSize, int32_t iResolution)
{
	m_fSize = fSize;
	m_iResolution = iResolution;
	val = (int32_t)log2(iResolution);

	CreateWaterMesh();
	CreateWaterMaterial();

	CreateTextureViewer();

	AmplitudeSimulationSpace::Settings s;
	s.spectrumType = AmplitudeSimulationSpace::Settings::SpectrumType::PiersonMoskowitz;
	s.n_x = 10;
	s.n_theta = 4;
	s.size = fSize;
	//s.windSpeed = 20;
	space = new AmplitudeSimulationSpace(s);

	waterheigthpos.resize(m_iResolution * m_iResolution);
	waterheigthnormals.resize(m_iResolution * m_iResolution);

	//waterNormals.Init(m_iResolution, m_iResolution, VK_FORMAT_R32G32B32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	//waterNormalsView = waterNormals.CreateImageView(VK_FORMAT_R32G32B32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT);


}

void WaterRenderingSystem::Destroy()
{
	DestroyTextureViewer();
	//waterNormalsView.Destroy(VulkanRenderer::GetDevice());
	//waterNormals.Free();
	waterPipeline.Destroy();
	waterPipelineWireframe.Destroy();
	waterMesh.Destroy(VulkanRenderer::GetDevice());
	

	delete space;
}

void WaterRenderingSystem::Update(float deltaTime)
{
	/*space->timeStep(space->cflTimeStep(), true);
	

	for (int i = 0; i < m_iResolution; i++)
	{
		for (int j = 0; j < m_iResolution; j++)
		{
			glm::vec2 pos = glm::vec2(-m_fSize/2 + (float)i / (float)(m_iResolution - 1), -m_fSize/2 + (float)j / (float)(m_iResolution - 1));
			std::pair<glm::vec3, glm::vec3> val = space->waterSurface(pos);
			waterheigthpos[i * m_iResolution + j] = val.first;
			waterheigthnormals[i * m_iResolution + j] = 0.5f*(val.second + 1.0f);
		}
	}

	//waterNormals.CopyNewData(m_iResolution, m_iResolution, waterheigthnormals.data(), m_iResolution * m_iResolution * sizeof(glm::vec3));*/
}

void WaterRenderingSystem::Draw(VulkanCommandBuffer & buffer, const Frustum & frustum)
{
	if (!Intersect(bounds, frustum)) return;
	buffer.BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, waterPipeline);
	buffer.BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, waterPipeline.GetLayout(), 0, 1, &materialData, 0, nullptr);
	buffer.DrawMesh(waterMesh);

	if (!showWireFrame) return;
	buffer.BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, waterPipelineWireframe);
	buffer.BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, waterPipelineWireframe.GetLayout(), 0, 1, &materialData, 0, nullptr);
	buffer.DrawMesh(waterMesh);
}

void WaterRenderingSystem::DrawOverlays()
{

	if (ImGui::CollapsingHeader("Water system"))
	{
		//int val = log2(m_iResolution);
		int oldVal = val;
		
		ImGui::SliderInt("Resolution", &val, 1, 8);
		ImGui::SameLine();
		m_iResolution = (int)pow(2, val);
		ImGui::Text("%d", m_iResolution);

		float oldSize = m_fSize;
		ImGui::SliderFloat("Size", &m_fSize, 1.0f, 500.0f);


		if (oldSize != m_fSize || val != oldVal)
		{
			waterMesh.Destroy(VulkanRenderer::GetDevice());
			CreateWaterMesh();
		}

		ImGui::Checkbox("Show wireframe", &showWireFrame);


		//ImGui::Image((void *)(unsigned long long)(VkImageView)waterNormalsView, ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));

	}
}

void WaterRenderingSystem::CreateWaterMesh()
{
	MeshData data;

	int nbVerts = m_iResolution + 1;
	float scale = m_fSize / m_iResolution;
	data.vertices.resize(nbVerts * nbVerts);

	for (int i = 0; i < nbVerts; ++i)
	{
		for (int j = 0; j < nbVerts; ++j)
		{
			data.vertices[i*nbVerts + j].pos = glm::vec4(scale * (i - nbVerts / 2), 0.0f, scale * (j - nbVerts / 2), 1.0f);
			data.vertices[i*nbVerts + j].normal = glm::vec4(0, 1, 0, 0.0f);
			data.vertices[i*nbVerts + j].tangent = glm::vec4(1, 0, 0, 0.0f);
			data.vertices[i*nbVerts + j].texCoord = glm::vec2(00, 00);
			data.vertices[i*nbVerts + j].color = glm::vec3(0.9f, 0.9f, 0.9f);
		}
	}

	for (int i = 0; i < nbVerts - 1; ++i)
	{
		for (int j = 0; j < nbVerts - 1; ++j)
		{
			data.indices.push_back((i)*nbVerts + j); data.indices.push_back((i)*nbVerts + j + 1);
			data.indices.push_back((i + 1)*nbVerts + j + 1); data.indices.push_back((i + 1)*nbVerts + j);
		}
	}

	bounds.Set(data, glm::mat4(1));
	waterMesh.Initialise(VulkanRenderer::GetPhysicalDevice(), VulkanRenderer::GetDevice(), data, VulkanRenderer::GetTranferPool(), VulkanRenderer::GetTransferQueue());

}

void WaterRenderingSystem::CreateWaterMaterial()
{
	std::vector<VkDynamicState> dynamicStateEnables = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
	};

	/** Water pipeline **/
	//build aplha blended
	std::vector<ShadersFileType> shadersWater;
	shadersWater.resize(4);
	shadersWater[0].filepath = "data/shaders/UberWater.vert.spv";
	shadersWater[0].type = SHADER_TYPE::VERTEX;
	shadersWater[1].filepath = "data/shaders/UberWater.tesc.spv";
	shadersWater[1].type = SHADER_TYPE::TESSELATION_CONTROL;
	shadersWater[2].filepath = "data/shaders/UberWater.tese.spv";
	shadersWater[2].type = SHADER_TYPE::TESSELATION_EVALUATION;
	shadersWater[3].filepath = "data/shaders/UberWater.frag.spv";
	shadersWater[3].type = SHADER_TYPE::FRAGMENT;

	waterPipeline.Initialise(VulkanRenderer::GetSurfaceExtent());

	//Set the shaders
	waterPipeline.SetShaders(static_cast<uint32_t>(shadersWater.size()), shadersWater.data());
	waterPipeline.SetTopology(VK_PRIMITIVE_TOPOLOGY_PATCH_LIST);
	waterPipeline.SetDynamicStates(dynamicStateEnables.data(), static_cast<uint32_t>(dynamicStateEnables.size()));
	//waterPipeline.SetPolygonMode(VK_POLYGON_MODE_LINE, 1.0f);

	//set blend mode
	waterPipeline.SetDepthTest(true, true);
	/*m_kPipelines[1].SetBlendAttachementCount(1);
	m_kPipelines[1].SetBlendingForAttachement(0, VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
		true, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD);*/

	//Create the pipline
	waterPipeline.CreatePipeline(&VulkanRenderer::GetDescriptorSetLayout(), VulkanRenderer::GetRenderPass(0));



	waterPipelineWireframe.Initialise(VulkanRenderer::GetSurfaceExtent());
	shadersWater[3].filepath = "data/shaders/WireframeFragment.frag.spv";

	//Set the shaders
	waterPipelineWireframe.SetShaders(static_cast<uint32_t>(shadersWater.size()), shadersWater.data());
	waterPipelineWireframe.SetTopology(VK_PRIMITIVE_TOPOLOGY_PATCH_LIST);
	waterPipelineWireframe.SetDynamicStates(dynamicStateEnables.data(), static_cast<uint32_t>(dynamicStateEnables.size()));
	waterPipelineWireframe.SetPolygonMode(VK_POLYGON_MODE_LINE, 1.0f);
	waterPipelineWireframe.SetDepthTest(false, false);
	waterPipelineWireframe.CreatePipeline(&VulkanRenderer::GetDescriptorSetLayout(), VulkanRenderer::GetRenderPass(0));


	VkDescriptorSetLayout& layouts = VulkanRenderer::GetDescriptorSetLayout();
	//VkDescriptorSetLayout layouts[] = { VulkanRenderer::GetDescriptorSetLayout() };
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = VulkanRenderer::GetDescriptorPool();
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &layouts;


	if (vkAllocateDescriptorSets(VulkanRenderer::GetDevice(), &allocInfo, &materialData) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor set!");
	}

	SetMaterialData();
}

void WaterRenderingSystem::SetMaterialData()
{
	std::string waterColor = "data/Models/BlenderScene/Textures/WaterColor.png";
	std::cout << "\tLoading texture " << waterColor << std::endl;
	diffuseColor.Init(waterColor, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true);
	diffuseColorView = diffuseColor.CreateImageView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);


	std::vector<VkWriteDescriptorSet> desc = HelloTriangleApplication::GetDescriptorWrites();

	VkDescriptorImageInfo info = {};
	info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	info.sampler = HelloTriangleApplication::GetNoRepeatSampler();
	info.imageView = diffuseColorView;

	VkDescriptorImageInfo infoambient = {};
	infoambient.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	infoambient.sampler = HelloTriangleApplication::GetNoRepeatSampler();
	infoambient.imageView = diffuseColorView;

	VkDescriptorImageInfo infoopacity = {};
	infoopacity.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	infoopacity.sampler = HelloTriangleApplication::GetNoRepeatSampler();
	infoopacity.imageView = diffuseColorView;

	VkDescriptorImageInfo infoNormalMap = {};
	infoNormalMap.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	infoNormalMap.sampler = HelloTriangleApplication::GetNoRepeatSampler();
	infoNormalMap.imageView = diffuseColorView;

	desc[0].dstSet = materialData;
	desc[1].dstSet = materialData;
	desc[2].dstSet = materialData;
	desc[3].dstSet = materialData;
	desc[4].dstSet = materialData;
	desc[5].dstSet = materialData;
	desc[2].pImageInfo = &info;
	desc[3].pImageInfo = &infoambient;
	desc[4].pImageInfo = &infoopacity;
	desc[5].pImageInfo = &infoNormalMap;

	vkUpdateDescriptorSets(VulkanRenderer::GetDevice(), static_cast<uint32_t>(desc.size()), desc.data(), 0, nullptr);
}

void WaterRenderingSystem::DestroyTextureViewer()
{
	textureQuad.Destroy(VulkanRenderer::GetDevice());
	vkDestroyDescriptorSetLayout(VulkanRenderer::GetDevice(), textureViewLayout, nullptr);
	textureViewPipeline.Destroy();
}

void WaterRenderingSystem::CreateTextureViewer()
{
	//Make the mesh
	MeshData data;
	data.vertices.resize(4);
	data.indices.resize(6);
	
	VertexData v0; v0.pos = glm::vec3(-1, -1, 0);
	data.vertices[0] = v0;
	v0.pos = glm::vec3(1, -1, 0);
	data.vertices[1] = v0;
	v0.pos = glm::vec3(1, 1, 0);
	data.vertices[2] = v0;
	v0.pos = glm::vec3(-1, 1, 0);
	data.vertices[3] = v0;

	data.indices[0] = 0; data.indices[1] = 1; data.indices[2] = 2;
	data.indices[3] = 0; data.indices[4] = 2; data.indices[5] = 3;
	
	textureQuad.Initialise(VulkanRenderer::GetPhysicalDevice(), VulkanRenderer::GetDevice(), data, VulkanRenderer::GetTranferPool(), VulkanRenderer::GetTransferQueue());

	//create the descriptorSet layout
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::vector<VkDescriptorSetLayoutBinding> bindings =
	{
		uboLayoutBinding,
		samplerLayoutBinding
	};

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(VulkanRenderer::GetDevice(), &layoutInfo, nullptr, &textureViewLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}

	

	//create the pipeline
	std::vector<VkDynamicState> dynamicStateEnables = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
	};

	std::vector<ShadersFileType> shaders;
	shaders.resize(2);
	shaders[0].filepath = "data/shaders/UI.vert.spv";
	shaders[0].type = VERTEX;
	shaders[1].filepath = "data/shaders/UI.frag.spv";
	shaders[1].type = FRAGMENT;

	textureViewPipeline.Initialise(VulkanRenderer::GetSurfaceExtent());
	textureViewPipeline.SetShaders(static_cast<uint32_t>(shaders.size()), shaders.data());
	textureViewPipeline.SetDynamicStates(dynamicStateEnables.data(), static_cast<uint32_t>(dynamicStateEnables.size()));
	//waterPipelineWireframe.SetPolygonMode(VK_POLYGON_MODE_LINE, 1.0f);
	textureViewPipeline.SetDepthTest(false, false);
	textureViewPipeline.CreatePipeline(&textureViewLayout, VulkanRenderer::GetRenderPass(0));


	// uniform buffer
	//descritor set
	//update
	//late write

}
