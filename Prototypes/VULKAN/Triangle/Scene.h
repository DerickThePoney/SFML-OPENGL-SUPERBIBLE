#pragma once

class VulkanMesh;
class VulkanCommandBuffer;
class VulkanImage;
class VulkanImageView;
struct aiScene;
struct aiNode;
struct aiMaterial;
class VulkanGraphicsPipeline;
class AABB;
class Frustum;

class SceneObject;

class Scene
{
public:
	Scene();
	~Scene();

	void InitialiseFromFile(const std::string& filename);
	void Destroy();

	void Draw(VulkanCommandBuffer& buffer, const Frustum& frustum);

private:
	void BuildPipelines();
	void LoadMeshes(const aiScene* scene);
	void LoadNode(const aiScene* scene, const aiNode* node);
	void LoadMeshNode(const aiScene* scene, const aiNode* node);
	void CreateDescriptorSets(int number);
	void UpdateDescriptorSet(int idImage, int idOpacityMap, int idDescriptor);
	void LoadMaterials(const aiScene* scene);
	void LoadMaterial(const aiScene* scene, const aiMaterial* material,int idx);

	void GroupByPipeline();

	std::vector<SceneObject> objects;

	std::set<int> matIDSet;
	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<int> pipelineID;
	std::vector<std::map<int,std::list<int>>> pipelineMeshIDs;

	std::vector<VulkanImage> diffuseImages;
	std::vector<VulkanImageView> diffuseImageViews;

	std::vector<VulkanImage> ambientImages;
	std::vector<VulkanImageView> ambientImageViews;

	std::vector<VulkanImage> opacityMaps;
	std::vector<VulkanImageView> opacityMapViews;

	std::vector<VulkanGraphicsPipeline> m_kPipelines;
};

