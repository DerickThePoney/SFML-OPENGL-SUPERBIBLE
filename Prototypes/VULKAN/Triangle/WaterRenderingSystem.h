#pragma once

#include "VulkanRenderer.h"
#include "AABB.h"

#include "AmplitudeSimulationSpace.h"

class WaterRenderingSystem
{
public:
	//WaterRenderingSystem();
	//~WaterRenderingSystem();

	void Initialise(float fSize, int32_t iResolution);
	void Destroy();
	
	void Update(float deltaTime);
	
	void Draw(VulkanCommandBuffer& buffer, const Frustum& frustum);
	void LateDraw(VulkanCommandBuffer& buffer, const Frustum& frustum);
	void DrawOverlays();

private:
	void CreateWaterMesh();
	void CreateWaterMaterial();
	void SetMaterialData();

	void CreateTextureViewer();
	void DestroyTextureViewer();
	void UpdateDebugUniformBuffer();

	//Mesh data
	float m_fSize;
	int32_t m_iResolution;
	int32_t val;

	//Rendering data
	VulkanMesh waterMesh;
	VulkanGraphicsPipeline waterPipeline;
	VulkanGraphicsPipeline waterPipelineWireframe;
	VulkanImage waterNormals;
	VulkanImageView waterNormalsView;
	VkDescriptorSet materialData;
	AABB bounds;
	VulkanImage diffuseColor;
	VulkanImageView diffuseColorView;

	//Debug view
	VulkanMesh textureQuad;
	VulkanGraphicsPipeline textureViewPipeline;
	VkDescriptorSetLayout textureViewLayout;
	VulkanBuffer screenSizeBuffer;
	VkDescriptorPool textureViewDescPool;
	VkDescriptorSet textureViewDescSet;
	VkSampler textureViewSampler;
	struct ScreenSize
	{
		glm::vec2 screenSize;
	} m_screenSize;


	bool showWireFrame;
	bool showWaterNormalsTexture;

	AmplitudeSimulationSpace* space;
	//std::vector<std::pair<glm::vec3, glm::vec3>> waterheigth;
	std::vector<glm::vec3> waterheigthpos;
	std::vector<glm::vec3> waterheigthnormals;
};

