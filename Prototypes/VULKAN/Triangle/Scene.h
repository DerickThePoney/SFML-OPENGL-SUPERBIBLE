#pragma once

class VulkanMesh;
class VulkanCommandBuffer;
struct aiScene;
struct aiNode;
class Scene
{
public:
	Scene();
	~Scene();

	void InitialiseFromFile(const std::string& filename);
	void Destroy();

	void Draw(VulkanCommandBuffer& buffer);

private:
	void LoadMeshes(const aiScene* scene);
	void LoadNode(const aiScene* scene, const aiNode* node);
	void LoadMeshNode(const aiScene* scene, const aiNode* node);



	std::vector<VulkanMesh> meshes;
};

