#pragma once

#include "VulkanRenderer.h"
#include "AABB.h"

#include "AmplitudeSimulationSpace.h"

class WaterRenderingSystem
{
public:
	//WaterRenderingSystem();
	//~WaterRenderingSystem();

	void Initialise(float fSize, int32_t iMeshResolution, int32_t iTextureResolution);
	void Destroy();
	
	void Update(float deltaTime);
	
	void Draw(VulkanCommandBuffer& buffer, const Frustum& frustum);
	void LateDraw(VulkanCommandBuffer& buffer, const Frustum& frustum);
	void DrawOverlays();

private:
	void CreateWaterMesh();
	void CreateDebugNormalsLayout();
	void CreateWaterDescriptorSetLayout();
	void CreateWaterMaterial();
	void SetMaterialData();
	void SetWaterMaterialData();

	void CreateTextureViewer();
	void DestroyTextureViewer();
	

	//Mesh data
	float m_fSize;
	int32_t m_iMeshResolution;
	int32_t m_iTextureResolution;
	int32_t val;

	//Rendering data
	VulkanMesh waterMesh;
	VkDescriptorSetLayout waterLayout;
	VkDescriptorPool waterDescPool;
	VkDescriptorSet waterDescSet;

	VkDescriptorSetLayout waterNormalsLayout;
	VkDescriptorPool waterNormalsDescPool;
	VkDescriptorSet waterNormalsDescSet;

	VkSampler waterSampler;
	VulkanGraphicsPipeline waterPipeline;
	VulkanGraphicsPipeline waterPipelineWireframe;
	VulkanGraphicsPipeline waterPipelineNormals;

	VulkanImage waterDisplacement;
	VulkanImageView waterDisplacementView;
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
	bool showNormals;

	AmplitudeSimulationSpace* space;
	//std::vector<std::pair<glm::vec3, glm::vec3>> waterheigth;
	std::vector<glm::vec3> waterheigthpos;
	std::vector<glm::vec3> waterheigthnormals;
};

