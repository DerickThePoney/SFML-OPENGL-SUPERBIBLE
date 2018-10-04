#pragma once

class VulkanMesh;
class VulkanCommandBuffer;
class VulkanImage;
class VulkanImageView;
struct aiScene;
struct aiNode;
struct aiMaterial;
class VulkanGraphicsPipeline;

class Scene
{
public:
	Scene();
	~Scene();

	void InitialiseFromFile(const std::string& filename);
	void Destroy();

	void Draw(VulkanCommandBuffer& buffer, VulkanGraphicsPipeline& pipeline);

private:
	void LoadMeshes(const aiScene* scene);
	void LoadNode(const aiScene* scene, const aiNode* node);
	void LoadMeshNode(const aiScene* scene, const aiNode* node);
	void CreateDescriptorSets(int number);
	void UpdateDescriptorSet(int id);
	void LoadMaterials(const aiScene* scene);
	void LoadMaterial(const aiScene* scene, const aiMaterial* material,int idx);



	std::vector<VulkanMesh> meshes;
	std::vector<int> matID;
	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VulkanImage> diffuseImages;
	std::vector<VulkanImageView> diffuseImageViews;
};

