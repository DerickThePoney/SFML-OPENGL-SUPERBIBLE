#include "stdafx.h"
#include "Scene.h"
#include "VulkanMesh.h"
#include "VulkanRenderer.h"
#include "VulkanGraphicsPipeline.h"

#include <assimp/Importer.hpp>		// C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "Triangle.h"

Scene::Scene()
{
}


Scene::~Scene()
{
}

void Scene::InitialiseFromFile(const std::string & filename)
{
	// Create an instance of the Importer class
	Assimp::Importer importer;
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile(filename,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	// If the import failed, report it
	if (!scene)
	{
		throw std::runtime_error("Unable to load scene " + filename);
	}
	// Now we can access the file's contents. 
	LoadMeshes(scene);
	LoadMaterials(scene);
	// We're done. Everything will be cleaned up by the importer destructor
	//return;
}

void Scene::Destroy()
{
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		meshes[i].Destroy(VulkanRenderer::GetDevice());
	}

	for (size_t i = 0; i < diffuseImages.size(); ++i)
	{
		diffuseImageViews[i].Destroy(VulkanRenderer::GetDevice());
		diffuseImages[i].Free(VulkanRenderer::GetDevice());
	}
}

void Scene::Draw(VulkanCommandBuffer & buffer, VulkanGraphicsPipeline& pipeline)
{
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		buffer.BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetLayout(), 0, 1, &descriptorSets[matID[i]], 0, nullptr);
		buffer.DrawMesh(meshes[i]);
	}
}

void Scene::LoadMeshes(const aiScene * scene)
{
	if (!scene->HasMeshes()) return;

	LoadNode(scene, scene->mRootNode);
}

void Scene::LoadNode(const aiScene * scene, const aiNode * node)
{

	if (node->mNumMeshes > 0)
	{
		LoadMeshNode(scene, node);
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		LoadNode(scene, node->mChildren[i]);
	}
}

void Scene::LoadMeshNode(const aiScene* scene, const aiNode * node)
{
	glm::mat4 tr = glm::mat4(
		node->mTransformation.a1, node->mTransformation.a2, node->mTransformation.a3, node->mTransformation.a4,
		node->mTransformation.b1, node->mTransformation.b2, node->mTransformation.b3, node->mTransformation.b4,
		node->mTransformation.c1, node->mTransformation.c2, node->mTransformation.c3, node->mTransformation.c4,
		node->mTransformation.d1, node->mTransformation.d2, node->mTransformation.d3, node->mTransformation.d4
	);
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		MeshData data;

		int meshNumber = node->mMeshes[i];

		aiMesh* mesh = scene->mMeshes[meshNumber];

		data.vertices.resize(mesh->mNumVertices);
				
		//copy the vertices
		for (unsigned int j = 0; j < mesh->mNumVertices; ++j)
		{
			data.vertices[j].pos = tr * glm::vec4(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z,1.0f);
			data.vertices[j].normal = tr * glm::vec4(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z, 0.0f);
			data.vertices[j].texCoord = glm::vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
			data.vertices[j].color = glm::vec3(0.9f, 0.9f, 0.9f);
		}

		//copy the indices
		for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
		{
			aiFace& face = mesh->mFaces[j];
			for (unsigned int k = 0; k < face.mNumIndices; ++k)
			{
				data.indices.push_back(face.mIndices[k]);
			}
		}
		VulkanMesh vMesh;
		vMesh.Initialise(VulkanRenderer::GetPhysicalDevice(), VulkanRenderer::GetDevice(), data, VulkanRenderer::GetTranferPool(), VulkanRenderer::GetTransferQueue());
		matID.push_back(mesh->mMaterialIndex);
		meshes.push_back(vMesh);
	}
}

void Scene::CreateDescriptorSets(int number)
{
	std::vector<VkDescriptorSetLayout> layouts(number, VulkanRenderer::GetDescriptorSetLayout());
	//VkDescriptorSetLayout layouts[] = { VulkanRenderer::GetDescriptorSetLayout() };
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = VulkanRenderer::GetDescriptorPool();
	allocInfo.descriptorSetCount = number;
	allocInfo.pSetLayouts = layouts.data();
	
	descriptorSets.resize(number);

	if (vkAllocateDescriptorSets(VulkanRenderer::GetDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor set!");
	}
}

void Scene::UpdateDescriptorSet(int id)
{
	std::array<VkWriteDescriptorSet, 3> desc = HelloTriangleApplication::GetDescriptorWrites();
	
	VkDescriptorImageInfo info;
	info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	info.sampler = desc[2].pImageInfo->sampler;
	info.imageView = diffuseImageViews[id];

	desc[0].dstSet = descriptorSets[id];
	desc[1].dstSet = descriptorSets[id];
	desc[2].dstSet = descriptorSets[id];
	desc[2].pImageInfo = &info;

	vkUpdateDescriptorSets(VulkanRenderer::GetDevice(), desc.size(), desc.data(), 0, nullptr);
}

void Scene::LoadMaterials(const aiScene * scene)
{
	CreateDescriptorSets(scene->mNumMaterials);

	for (int i = 0; i < scene->mNumMaterials; ++i)
	{
		LoadMaterial(scene, scene->mMaterials[i], i);
	}
}

void Scene::LoadMaterial(const aiScene * scene, const aiMaterial * material, int idx)
{
	aiString basePath("data/Models/Sponza/");
	aiString Path;
	aiReturn res = material->GetTexture(aiTextureType_DIFFUSE, 0, &Path);
	if (res == aiReturn_SUCCESS)
	{
		VulkanImage diffuse;
		basePath.Append(Path.C_Str());
		diffuse.Init(VulkanRenderer::GetPhysicalDevice(), VulkanRenderer::GetDevice(), VulkanRenderer::GetTranferPool(), VulkanRenderer::GetGraphicsQueue(), VulkanRenderer::GetTransferQueue(), basePath.C_Str(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		diffuseImages.push_back(diffuse);
		diffuseImageViews.push_back(diffuse.CreateImageView(VulkanRenderer::GetDevice(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT));

		UpdateDescriptorSet(diffuseImages.size() - 1);
	}
	else
	{
		aiString name;
		material->Get(AI_MATKEY_NAME, name);
		std::cout << "unable to find the material texture for mat " << name.C_Str() << std::endl;
	}
}
