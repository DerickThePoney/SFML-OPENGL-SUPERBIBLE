#pragma once
#include "IScene.h"
#include "CameraProgram.h"
#include "WaterRenderingSystem.h"

class VulkanMesh;
class VulkanCommandBuffer;
class VulkanImage;
class VulkanImageView;
struct aiScene;
struct aiNode;
struct aiMaterial;
struct aiMesh;
struct aiAnimation;
class VulkanGraphicsPipeline;
class AABB;
class BoundingSphere;
class Frustum;

class SceneObject;

class ObjectPositionAnimation;
class WaterRenderingSystem;

class SceneTestObjLoading : public IScene
{
public:
	SceneTestObjLoading();
	~SceneTestObjLoading();

	virtual void InitialiseFromFile(const std::string& filename);
	virtual void Destroy();

	glm::mat4 GetCameraPosition(float dt);

	virtual void Draw(VulkanCommandBuffer& buffer, const Frustum& frustum);


	// Hérité via IScene
	virtual void Update(float deltaTime) override;
	virtual void DrawOverlays() override;


private:
	void BuildPipelines();
	void LoadNodes(const aiScene* scene);
	void LoadNode(const aiScene* scene, const aiNode* node);
	void LoadMeshNode(const aiScene* scene, const aiNode* node);
	void CreateDescriptorSets(int number);
	void UpdateDescriptorSet(int idImage, int idOpacityMap, int idDescriptor, VkSampler sampler);
	void LoadMeshes(const aiScene* scene);
	void LoadMesh(const aiScene* scene, const aiMesh* mesh, int IDX);
	void LoadMaterials(const aiScene* scene);
	void LoadMaterial(const aiScene* scene, const aiMaterial* material,int idx);
	void LoadAnimations(const aiScene* scene);
	void LoadAnimation(const aiScene* scene, const aiAnimation* animation);
	void DisplayMaterialProperties(const aiMaterial* material);

	void GroupByPipeline();

	std::vector<SceneObject> objects;

	std::vector<std::map<int, std::list<int>>> pipelineMeshIDs;

	//mesh data
	std::vector<VulkanMesh> meshes;
	std::vector<BoundingSphere> boundingSpheres;
	std::vector<AABB> boundingBoxes;
	std::vector<VkDescriptorSet> descriptorSets;
	std::set<int> matIDSet;
	std::vector<int> pipelineID;

	/*
	
	
	std::vector<std::map<int,std::list<int>>> pipelineMeshIDs;*/

	std::vector<VulkanImage> diffuseImages;
	std::vector<VulkanImageView> diffuseImageViews;

	std::vector<VulkanImage> ambientImages;
	std::vector<VulkanImageView> ambientImageViews;

	std::vector<VulkanImage> opacityMaps;
	std::vector<VulkanImageView> opacityMapViews;

	std::vector<VulkanGraphicsPipeline> m_kPipelines;


	ObjectPositionAnimation cameraAnimation;

	WaterRenderingSystem water;

};

