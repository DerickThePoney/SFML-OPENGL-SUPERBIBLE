#include "stdafx.h"
#include "Scene.h"
#include "MeshData.h"
#include "AABB.h"
#include "VulkanMesh.h"
#include "VulkanRenderer.h"
#include "VulkanGraphicsPipeline.h"
#include "SceneObject.h"
#include "Frustum.h"
#include "Intersections.h"

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
	BuildPipelines();
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

	GroupByPipeline();
}

void Scene::Destroy()
{
	for (size_t i = 0; i < objects.size(); ++i)
	{
		objects[i].mesh.Destroy(VulkanRenderer::GetDevice());
	}

	for (size_t i = 0; i < diffuseImages.size(); ++i)
	{
		diffuseImageViews[i].Destroy(VulkanRenderer::GetDevice());
		diffuseImages[i].Free();
	}

	for (size_t i = 0; i < ambientImages.size(); ++i)
	{
		ambientImages[i].Free();
	}

	for (size_t i = 0; i < ambientImageViews.size(); ++i)
	{
		ambientImageViews[i].Destroy(VulkanRenderer::GetDevice());
	}

	for (size_t i = 0; i < m_kPipelines.size(); ++i)
	{
		m_kPipelines[i].Destroy();
	}
}

void Scene::Draw(VulkanCommandBuffer & buffer, const Frustum& frustum)
{
	int objectsDrawn = 0;

	for (size_t i = 0; i < pipelineMeshIDs.size(); ++i)
	{
		buffer.BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_kPipelines[i]);
		for (auto itMatID = pipelineMeshIDs[i].begin(); itMatID != pipelineMeshIDs[i].end(); ++itMatID)
		{
			buffer.BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, m_kPipelines[i].GetLayout(), 0, 1, &descriptorSets[itMatID->first], 0, nullptr);
			for (auto it = itMatID->second.begin(); it != itMatID->second.end(); ++it)
			{
				//frustum culling
				if (!Intersect(objects[*it].boundingSphere, frustum)) continue;
				++objectsDrawn;

				buffer.DrawMesh(objects[*it].mesh);
			}
		}
	}
}

void Scene::BuildPipelines()
{
	//read the shaders
	std::vector<ShadersFileType> shaders;
	ShadersFileType vert;
	vert.filepath = "data/shaders/UberNoOpacity.vert.spv";
	vert.type = VERTEX;
	ShadersFileType frag;
	frag.filepath = "data/shaders/UberNoOpacity.frag.spv";
	frag.type = FRAGMENT;
	shaders.push_back(vert);
	shaders.push_back(frag);

	std::vector<VkDynamicState> dynamicStateEnables = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
	};

	//Init the pipeline
	m_kPipelines.push_back({});
	m_kPipelines.push_back({});
	m_kPipelines[0].Initialise(VulkanRenderer::GetSurfaceExtent());

	//Set the shaders
	m_kPipelines[0].SetShaders(shaders.size(), shaders.data());
	m_kPipelines[0].SetDynamicStates(dynamicStateEnables.data(), dynamicStateEnables.size());
	//m_kPipeline.SetPolygonMode(VK_POLYGON_MODE_LINE, .2f);

	//Create the pipline
	m_kPipelines[0].CreatePipeline(&VulkanRenderer::GetDescriptorSetLayout(), VulkanRenderer::GetRenderPass(0));

	//build aplha blended
	shaders[1].filepath = "data/shaders/UberOpacity.frag.spv";

	m_kPipelines[1].Initialise(VulkanRenderer::GetSurfaceExtent());

	//Set the shaders
	m_kPipelines[1].SetShaders(shaders.size(), shaders.data());
	m_kPipelines[1].SetDynamicStates(dynamicStateEnables.data(), dynamicStateEnables.size());
	//m_kPipeline.SetPolygonMode(VK_POLYGON_MODE_LINE, .2f);

	//set blend mode
	m_kPipelines[1].SetDepthTest(true, false);
	m_kPipelines[1].SetBlendAttachementCount(1);
	m_kPipelines[1].SetBlendingForAttachement(0, VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
		true, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD);

	//Create the pipline
	m_kPipelines[1].CreatePipeline(&VulkanRenderer::GetDescriptorSetLayout(), VulkanRenderer::GetRenderPass(0));

	pipelineMeshIDs = { {},{} };
	
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
		SceneObject object;
		object.transform = tr;

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
				
		object.boundingBox.Set(data, glm::mat4(1));
		object.boundingSphere.Set(data, glm::mat4(1));

		object.mesh.Initialise(VulkanRenderer::GetPhysicalDevice(), VulkanRenderer::GetDevice(), data, VulkanRenderer::GetTranferPool(), VulkanRenderer::GetTransferQueue());
		object.matID = mesh->mMaterialIndex;
		matIDSet.insert(mesh->mMaterialIndex);

		objects.push_back(object);
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
	pipelineID.resize(number);

	if (vkAllocateDescriptorSets(VulkanRenderer::GetDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor set!");
	}
}

void Scene::UpdateDescriptorSet(int idImage, int idOpacityMap, int idDescriptor)
{
	std::vector<VkWriteDescriptorSet> desc = HelloTriangleApplication::GetDescriptorWrites();
	
	VkDescriptorImageInfo info = {};
	info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	info.sampler = desc[2].pImageInfo->sampler;
	info.imageView = diffuseImageViews[idImage];

	VkDescriptorImageInfo infoambient = {};
	infoambient.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	infoambient.sampler = desc[2].pImageInfo->sampler;
	infoambient.imageView = ambientImageViews[idImage];

	VkDescriptorImageInfo infoopacity = {};
	infoopacity.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	infoopacity.sampler = desc[2].pImageInfo->sampler;
	infoopacity.imageView = (idOpacityMap != -1) ? opacityMapViews[idOpacityMap] : diffuseImageViews[idImage];

	desc[0].dstSet = descriptorSets[idDescriptor];
	desc[1].dstSet = descriptorSets[idDescriptor];
	desc[2].dstSet = descriptorSets[idDescriptor];
	desc[3].dstSet = descriptorSets[idDescriptor];
	desc[4].dstSet = descriptorSets[idDescriptor];
	desc[2].pImageInfo = &info;
	desc[3].pImageInfo = &infoambient;
	desc[4].pImageInfo = &infoopacity;

	vkUpdateDescriptorSets(VulkanRenderer::GetDevice(), desc.size(), desc.data(), 0, nullptr);
}

void Scene::LoadMaterials(const aiScene * scene)
{
	CreateDescriptorSets(scene->mNumMaterials);

	for (auto it = matIDSet.begin(); it != matIDSet.end(); ++it)
	{
		LoadMaterial(scene, scene->mMaterials[*it], *it);
	}
}

void Scene::LoadMaterial(const aiScene * scene, const aiMaterial * material, int idx)
{
	aiString name;
	material->Get(AI_MATKEY_NAME, name);
	std::cout << "Loading Material " << name.C_Str() << " with id " << idx << std::endl;

	//DiffuseTexture
	aiString basePath("data/Models/Sponza/");
	aiString PathD;
	aiReturn res = material->GetTexture(aiTextureType_DIFFUSE, 0, &PathD);
	

	
	if (res == aiReturn_SUCCESS)
	{
		std::cout << "\tLoading texture " << PathD.C_Str() << std::endl;
		VulkanImage diffuse;
		basePath.Append(PathD.C_Str());
		diffuse.Init(basePath.C_Str(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		diffuseImages.push_back(diffuse);
		diffuseImageViews.push_back(diffuse.CreateImageView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT));
	}
	else
	{
		
		std::cout << "unable to find the material texture for mat " << name.C_Str() << std::endl;
	}

	//Ambient
	basePath = "data/Models/Sponza/";
	aiString PathA;
	res = material->GetTexture(aiTextureType_AMBIENT, 0, &PathA);

	if (res == aiReturn_SUCCESS)
	{
		if (PathA == PathD)
		{
			ambientImageViews.push_back(diffuseImages[diffuseImageViews.size() - 1].CreateImageView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT));
		}
		else
		{
			std::cout << "\tLoading texture " << PathA.C_Str() << std::endl;
			VulkanImage ambient;
			basePath.Append(PathA.C_Str());
			ambient.Init(basePath.C_Str(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			ambientImages.push_back(ambient);
			ambientImageViews.push_back(ambient.CreateImageView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT));
		}
	}
	else
	{

		std::cout << "unable to find the ambient material texture for mat " << name.C_Str() << std::endl;
	}

	//opacity
	bool bHasOpacity = false;
	basePath = "data/Models/Sponza/";
	aiString PathO;
	res = material->GetTexture(aiTextureType_OPACITY, 0, &PathO);

	if (res == aiReturn_SUCCESS)
	{
		bHasOpacity = true;
		std::cout << "\tLoading texture " << PathO.C_Str() << std::endl;
		VulkanImage opacity;
		basePath.Append(PathO.C_Str());
		opacity.Init(basePath.C_Str(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		opacityMaps.push_back(opacity);
		opacityMapViews.push_back(opacity.CreateImageView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT));
		
	}
	else
	{

		std::cout << "No opacity map found for material " << name.C_Str() << std::endl;
	}

	pipelineID[idx]  = (bHasOpacity) ? 1 : 0;

	if (diffuseImages.size() > 0)
	{
		UpdateDescriptorSet(diffuseImages.size() - 1, (bHasOpacity) ? opacityMapViews.size()-1 :-1, idx);
	}
}

void Scene::GroupByPipeline()
{
	for (size_t i = 0; i < objects.size(); ++i)
	{
		std::map<int, std::list<int>>& map = pipelineMeshIDs[pipelineID[objects[i].matID]];
		map[objects[i].matID].push_back(i);
	}
}
