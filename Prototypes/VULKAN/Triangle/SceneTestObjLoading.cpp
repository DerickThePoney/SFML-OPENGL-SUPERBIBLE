#include "stdafx.h"
#include "SceneTestObjLoading.h"
#include "MeshData.h"
#include "AABB.h"
#include "VulkanMesh.h"
#include "VulkanRenderer.h"
#include "VulkanGraphicsPipeline.h"
#include "SceneObject.h"
#include "Frustum.h"
#include "Intersections.h"
#include "WaterRenderingSystem.h"

//#include "CameraProgram.h"

#include <assimp/Importer.hpp>		// C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "Material.h"
#include "Triangle.h"


SceneTestObjLoading::SceneTestObjLoading()
{
}


SceneTestObjLoading::~SceneTestObjLoading()
{
}

void SceneTestObjLoading::InitialiseFromFile(const std::string & filename)
{
	BuildPipelines();
	// Create an instance of the Importer class
	Assimp::Importer importer;
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.

	const aiScene* scene = importer.ReadFile("data/Models/BlenderScene/BlenderScene_subdiv.obj",
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	/*const aiScene* scene2 = importer.ReadFile("data/Models/Sponza/Animation2.fbx",
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	// If the import failed, report it
	if (!scene2)
	{
		throw std::runtime_error("Unable to load scene " + filename);
	}

	LoadAnimations(scene2);

	const aiScene* scene = importer.ReadFile(filename,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);*/

	// If the import failed, report it
	if (!scene)
	{
		throw std::runtime_error("Unable to load scene " + filename);
	}
	// Now we can access the file's contents. 
	LoadNodes(scene);
	LoadMeshes(scene);
	LoadMaterials(scene);
	// We're done. Everything will be cleaned up by the importer destructor
	//return;

	GroupByPipeline();

	water.Initialise(500.0f, 16);
	
}

void SceneTestObjLoading::Destroy()
{
	water.Destroy();
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		meshes[i].Destroy(VulkanRenderer::GetDevice());
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

	for (size_t i = 0; i < opacityMaps.size(); ++i)
	{
		opacityMaps[i].Free();
	}

	for (size_t i = 0; i < opacityMapViews.size(); ++i)
	{
		opacityMapViews[i].Destroy(VulkanRenderer::GetDevice());
	}

	for (size_t i = 0; i < m_kPipelines.size(); ++i)
	{
		m_kPipelines[i].Destroy();
	}
}

glm::mat4 SceneTestObjLoading::GetCameraPosition(float dt)
{
	return glm::translate(glm::vec3(13.0f, 10.0f, 0.0f));// cameraAnimation.Update(dt);
}

void SceneTestObjLoading::Draw(VulkanCommandBuffer & buffer, const Frustum& frustum)
{
	//VkEngine::Timer<std::chrono::microseconds::period> timer("SCENE_DRAW_PROFILING");
	int objectsDrawn = 0;
	water.Draw(buffer, frustum);

	for (size_t i = 0; i < pipelineMeshIDs.size(); ++i)
	{
		//if (i ==  0|| i == 1)continue;
		buffer.BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_kPipelines[i]);
		for (auto itMatID = pipelineMeshIDs[i].begin(); itMatID != pipelineMeshIDs[i].end(); ++itMatID)
		{
			buffer.BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, m_kPipelines[i].GetLayout(), 0, 1, &descriptorSets[itMatID->first], 0, nullptr);
			for (auto it = itMatID->second.begin(); it != itMatID->second.end(); ++it)
			{
				//frustum culling
				if (!Intersect(boundingSpheres[objects[*it].meshIDX], frustum)) continue;
				++objectsDrawn;

				buffer.DrawMesh(meshes[objects[*it].meshIDX]);
			}
		}
	}

	
}

void SceneTestObjLoading::Update(float deltaTime)
{
	water.Update(deltaTime);
}

void SceneTestObjLoading::DrawOverlays()
{
	ImGui::Begin("Scene parameters");
	water.DrawOverlays();
	ImGui::End();
}


void SceneTestObjLoading::BuildPipelines()
{
	//read the shaders
	std::vector<ShadersFileType> shaders;
	std::vector<ShadersFileType> shadersWater;
	ShadersFileType vert;
	vert.filepath = "data/shaders/UberNoOpacity.vert.spv";
	vert.type = VERTEX;
	ShadersFileType frag;
	frag.filepath = "data/shaders/UberNoOpacity.frag.spv";
	frag.type = FRAGMENT;
	shaders.push_back(vert);
	shaders.push_back(frag);

	Material m;
	m.SetShaders(shaders, 10);
	m.Destroy();

	std::vector<VkDynamicState> dynamicStateEnables = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
	};

	//Init the pipeline
	m_kPipelines.push_back({});
	//m_kPipelines.push_back({});
	m_kPipelines.push_back({});
	m_kPipelines[0].Initialise(VulkanRenderer::GetSurfaceExtent());

	//Set the shaders
	m_kPipelines[0].SetShaders(shaders.size(), shaders.data());
	m_kPipelines[0].SetDynamicStates(dynamicStateEnables.data(), dynamicStateEnables.size());
	//m_kPipeline.SetPolygonMode(VK_POLYGON_MODE_LINE, .2f);

	//Create the pipline
	m_kPipelines[0].CreatePipeline(&VulkanRenderer::GetDescriptorSetLayout(), VulkanRenderer::GetRenderPass(0));



	/** Aplha blended pipeline **/
	//build aplha blended
	shaders[0].filepath = "data/shaders/UberNoOpacity.vert.spv";
	shaders[1].filepath = "data/shaders/UberOpacity.frag.spv";

	m_kPipelines[1].Initialise(VulkanRenderer::GetSurfaceExtent());

	//Set the shaders
	m_kPipelines[1].SetShaders(shaders.size(), shaders.data());
	m_kPipelines[1].SetDynamicStates(dynamicStateEnables.data(), dynamicStateEnables.size());
	//m_kPipeline.SetPolygonMode(VK_POLYGON_MODE_LINE, .2f);

	//set blend mode
	//m_kPipelines[2].SetPolygonMode(VK_POLYGON_MODE_LINE);
	m_kPipelines[1].SetCullMode(VK_CULL_MODE_NONE);
	m_kPipelines[1].SetDepthTest(true, false);
	m_kPipelines[1].SetBlendAttachementCount(1);
	m_kPipelines[1].SetBlendingForAttachement(0, VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
		true, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD);

	//Create the pipline
	m_kPipelines[1].CreatePipeline(&VulkanRenderer::GetDescriptorSetLayout(), VulkanRenderer::GetRenderPass(0));

	pipelineMeshIDs = { {},{} };

	//pipeline cache test
	/*{
		VulkanShader* pshaders = new VulkanShader[2];
		pshaders[0].Initialise(VulkanRenderer::GetDevice(), shaders[0].type, shaders[0].filepath.c_str());
		pshaders[1].Initialise(VulkanRenderer::GetDevice(), shaders[1].type, shaders[1].filepath.c_str());

		VkPipelineCache cache;
		VkPipelineCacheCreateInfo info;
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		info.initialDataSize = 0;
		info.pInitialData = nullptr;
		info.pNext = nullptr;
		info.flags = 0;
		if (vkCreatePipelineCache(VulkanRenderer::GetDevice(), &info, nullptr, &cache) != VK_SUCCESS)
		{
			throw std::runtime_error("Unable to build pipeline cache");
		}

		{
			std::vector<VulkanGraphicsPipeline> pipelines; pipelines.resize(1000);
			{

				VkEngine::Timer<std::chrono::milliseconds::period> t("Pipeline creation with cache");

				for (int i = 0; i < 1000; ++i)
				{
					pipelines[i].Initialise(VulkanRenderer::GetSurfaceExtent());
					pipelines[i].SetShaders(pshaders, shaders.size(), shaders.data());
					pipelines[i].CreatePipeline(&VulkanRenderer::GetDescriptorSetLayout(), VulkanRenderer::GetRenderPass(0), cache);
				}

			}

			for (int i = 0; i < 1000; ++i)
			{
				pipelines[i].Destroy();
			}
		}

		{
			std::vector<VulkanGraphicsPipeline> pipelines; pipelines.resize(1000);
			{

				VkEngine::Timer<std::chrono::milliseconds::period> t("Pipeline creation without cache");

				for (int i = 0; i < 1000; ++i)
				{
					pipelines[i].Initialise(VulkanRenderer::GetSurfaceExtent());
					pipelines[i].SetShaders(pshaders, shaders.size(), shaders.data());
					pipelines[i].CreatePipeline(&VulkanRenderer::GetDescriptorSetLayout(), VulkanRenderer::GetRenderPass(0));
				}

			}

			for (int i = 0; i < 1000; ++i)
			{
				pipelines[i].Destroy();
			}
		}

		pshaders[0].Destroy(VulkanRenderer::GetDevice());
		pshaders[1].Destroy(VulkanRenderer::GetDevice());
		delete[] pshaders;

		size_t dataSize;
		vkGetPipelineCacheData(VulkanRenderer::GetDevice(), cache, &dataSize, nullptr);

		std::vector<char> pData; pData.resize(dataSize);
		vkGetPipelineCacheData(VulkanRenderer::GetDevice(), cache, &dataSize, pData.data());

		vkDestroyPipelineCache(VulkanRenderer::GetDevice(), cache, nullptr);
	}*/
	
}

void SceneTestObjLoading::LoadNodes(const aiScene * scene)
{
	if (!scene->HasMeshes()) return;

	LoadNode(scene, scene->mRootNode);	
}

void SceneTestObjLoading::LoadNode(const aiScene * scene, const aiNode * node)
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

void SceneTestObjLoading::LoadMeshNode(const aiScene* scene, const aiNode * node)
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

		int meshNumber = node->mMeshes[i];
		object.meshIDX = meshNumber;
		object.materialIDX = scene->mMeshes[meshNumber]->mMaterialIndex;
		
		matIDSet.insert(object.materialIDX);

		objects.push_back(object);
	}
}

void SceneTestObjLoading::CreateDescriptorSets(int number)
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

void SceneTestObjLoading::UpdateDescriptorSet(int idImage, int idOpacityMap, int idDescriptor, VkSampler sampler)
{
	std::vector<VkWriteDescriptorSet> desc = HelloTriangleApplication::GetDescriptorWrites();
	
	VkDescriptorImageInfo info = {};
	info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	info.sampler = sampler;
	info.imageView = diffuseImageViews[idImage];

	VkDescriptorImageInfo infoambient = {};
	infoambient.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	infoambient.sampler = sampler;
	infoambient.imageView = diffuseImageViews[idImage];

	VkDescriptorImageInfo infoopacity = {};
	infoopacity.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	infoopacity.sampler = sampler;
	infoopacity.imageView = (idOpacityMap != -1) ? opacityMapViews[idOpacityMap] : diffuseImageViews[idImage];

	VkDescriptorImageInfo infoNormalMap = {};
	infoNormalMap.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	infoNormalMap.sampler = sampler;
	infoNormalMap.imageView = (idOpacityMap != -1) ? opacityMapViews[idOpacityMap] : diffuseImageViews[idImage];

	desc[0].dstSet = descriptorSets[idDescriptor];
	desc[1].dstSet = descriptorSets[idDescriptor];
	desc[2].dstSet = descriptorSets[idDescriptor];
	desc[3].dstSet = descriptorSets[idDescriptor];
	desc[4].dstSet = descriptorSets[idDescriptor];
	desc[5].dstSet = descriptorSets[idDescriptor];
	desc[2].pImageInfo = &info;
	desc[3].pImageInfo = &infoambient;
	desc[4].pImageInfo = &infoopacity;
	desc[5].pImageInfo = &infoNormalMap;

	vkUpdateDescriptorSets(VulkanRenderer::GetDevice(), desc.size(), desc.data(), 0, nullptr);
}

void SceneTestObjLoading::LoadMeshes(const aiScene * scene)
{
	if (!scene->HasMeshes()) return;
	meshes.resize(scene->mNumMeshes);
	boundingSpheres.resize(scene->mNumMeshes);
	boundingBoxes.resize(scene->mNumMeshes);

	for (size_t i = 0; i < scene->mNumMeshes; ++i)
	{
		LoadMesh(scene, scene->mMeshes[i], i);
	}	
}

void SceneTestObjLoading::LoadMesh(const aiScene * scene, const aiMesh * mesh, int IDX)
{
	MeshData data;
	data.vertices.resize(mesh->mNumVertices);

	//copy the vertices
	for (unsigned int j = 0; j < mesh->mNumVertices; ++j)
	{
		data.vertices[j].pos = glm::vec4(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z, 1.0f);
		data.vertices[j].normal = glm::vec4(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z, 0.0f);
		data.vertices[j].tangent = glm::vec4(mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z, 0.0f);
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

	boundingBoxes[IDX].Set(data, glm::mat4(1));
	boundingSpheres[IDX].Set(data, glm::mat4(1));
	meshes[IDX].Initialise(VulkanRenderer::GetPhysicalDevice(), VulkanRenderer::GetDevice(), data, VulkanRenderer::GetTranferPool(), VulkanRenderer::GetTransferQueue());
}

void SceneTestObjLoading::LoadMaterials(const aiScene * scene)
{
	CreateDescriptorSets(scene->mNumMaterials);

	for (auto it = matIDSet.begin(); it != matIDSet.end(); ++it)
	{
		LoadMaterial(scene, scene->mMaterials[*it], *it);
	}
}

void SceneTestObjLoading::LoadMaterial(const aiScene * scene, const aiMaterial * material, int idx)
{
	aiString name;
	material->Get(AI_MATKEY_NAME, name);
	std::cout << "Loading Material " << name.C_Str() << " with id " << idx << std::endl;

	bool isWater = false;
	if (!strcmp(name.C_Str(), "Water.001")) isWater = true;

	//DisplayMaterialProperties(material);
	//DiffuseTexture
	aiString basePath("data/Models/BlenderScene/");
	aiString PathD;
	aiReturn res = material->GetTexture(aiTextureType_DIFFUSE, 0, &PathD);
	

	
	if (res == aiReturn_SUCCESS)
	{
		std::cout << "\tLoading texture " << PathD.C_Str() << std::endl;
		VulkanImage diffuse;
		basePath.Append(PathD.C_Str());
		diffuse.Init(basePath.C_Str(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true);
		diffuseImages.push_back(diffuse);
		diffuseImageViews.push_back(diffuse.CreateImageView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT));
	}
	else
	{
		
		std::cout << "\tunable to find the material texture for mat " << name.C_Str() << std::endl;
	}

	//Ambient
	basePath = "data/Models/BlenderScene/";
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
			ambient.Init(basePath.C_Str(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true);
			ambientImages.push_back(ambient);
			ambientImageViews.push_back(ambient.CreateImageView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT));
		}
	}
	else
	{

		std::cout << "\tunable to find the ambient material texture for mat " << name.C_Str() << std::endl;
	}

	//opacity value
	float opacity;
	res = material->Get(AI_MATKEY_OPACITY, opacity);

	if (res == aiReturn_SUCCESS)
	{
		
	}

	/*//opacity map
	bool bHasOpacityMap = false;
	basePath = "data/Models/BlenderScene/";
	aiString PathO;
	res = material->GetTexture(aiTextureType_OPACITY, 0, &PathO);

	if (res == aiReturn_SUCCESS)
	{
		bHasOpacityMap = true;
		std::cout << "\tLoading texture " << PathO.C_Str() << std::endl;
		VulkanImage opacity;
		basePath.Append(PathO.C_Str());
		opacity.Init(basePath.C_Str(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		opacityMaps.push_back(opacity);
		opacityMapViews.push_back(opacity.CreateImageView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT));
		
	}
	else
	{

		std::cout << "\tNo opacity map found for material " << name.C_Str() << std::endl;
	}*/

	//pipelineID[idx]  = (bHasOpacityMap) ? 1 : 0;
	if (isWater)
	{
		//pipelineID[idx] = 1;
	}
	else if (opacity == 1.0f)
	{
		pipelineID[idx] = 0;
	}
	else
	{
		pipelineID[idx] = 1;
	}

	if (diffuseImages.size() > 0)
	{
		UpdateDescriptorSet(diffuseImages.size() - 1, -1, idx, (opacity == 1.0f)? HelloTriangleApplication::GetRepeatSampler() : HelloTriangleApplication::GetNoRepeatSampler());
	}
}

void SceneTestObjLoading::LoadAnimations(const aiScene * scene)
{
	for (uint32_t i = 0; i < scene->mNumAnimations; ++i)
	{
		LoadAnimation(scene, scene->mAnimations[i]);
	}
}

void SceneTestObjLoading::LoadAnimation(const aiScene * scene, const aiAnimation * animation)
{
	cameraAnimation.SetTicksPerSecond(1);
	for (unsigned int i = 0; i < animation->mChannels[0]->mNumPositionKeys; ++i)
	{
		glm::mat4 TRS = glm::mat4(1);// glm::scale(glm::vec3(animation->mChannels[0]->mScalingKeys[i].mValue.x, animation->mChannels[0]->mScalingKeys[i].mValue.y, animation->mChannels[0]->mScalingKeys[i].mValue.z));
		TRS = (glm::mat4)glm::quat(animation->mChannels[0]->mRotationKeys[i].mValue.w, glm::vec3(animation->mChannels[0]->mRotationKeys->mValue.x, animation->mChannels[0]->mRotationKeys[i].mValue.y, animation->mChannels[0]->mRotationKeys[i].mValue.z)) * TRS;
		TRS = glm::rotate(glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::translate(glm::vec3(animation->mChannels[0]->mPositionKeys[i].mValue.x / animation->mChannels[0]->mScalingKeys[i].mValue.x, -animation->mChannels[0]->mPositionKeys[i].mValue.y / animation->mChannels[0]->mScalingKeys[i].mValue.y, animation->mChannels[0]->mPositionKeys[i].mValue.z/ animation->mChannels[0]->mScalingKeys[i].mValue.z)) * TRS;
		cameraAnimation.AddKeyframe((float)animation->mChannels[0]->mScalingKeys[i].mTime, TRS);
	}
	
}

void SceneTestObjLoading::DisplayMaterialProperties(const aiMaterial * material)
{
	for (size_t i = 0; i < material->mNumProperties; ++i)
	{
		aiMaterialProperty* property = material->mProperties[i];
		std::cout << "\tPropertyName: " << property->mKey.C_Str() << std::endl;
		
		if (property->mSemantic != aiTextureType_NONE)
		{
			std::cout << "\t\tSemantic: ";
			switch (property->mSemantic)
			{				
			case aiTextureType_DIFFUSE:
				std::cout << "aiTextureType_DIFFUSE" << std::endl;
				break;					
			case aiTextureType_SPECULAR:
				std::cout << "aiTextureType_SPECULAR" << std::endl;
				break;
			case aiTextureType_AMBIENT:
				std::cout << "aiTextureType_AMBIENT" << std::endl;
				break;
			case aiTextureType_EMISSIVE:
				std::cout << "aiTextureType_EMISSIVE" << std::endl;
				break;
			case aiTextureType_HEIGHT:
				std::cout << "aiTextureType_HEIGHT" << std::endl;
				break;
			case aiTextureType_NORMALS:
				std::cout << "aiTextureType_NORMALS" << std::endl;
				break;
			case aiTextureType_SHININESS:
				std::cout << "aiTextureType_SHININESS" << std::endl;
				break;
			case aiTextureType_OPACITY:
				std::cout << "aiTextureType_OPACITY" << std::endl;
				break;
			case aiTextureType_DISPLACEMENT:
				std::cout << "aiTextureType_DISPLACEMENT" << std::endl;
				break;
			case aiTextureType_LIGHTMAP:
				std::cout << "aiTextureType_LIGHTMAP" << std::endl;
				break;
			case aiTextureType_REFLECTION:
				std::cout << "aiTextureType_REFLECTION" << std::endl;
				break;
			case aiTextureType_UNKNOWN:
				std::cout << "aiTextureType_UNKNOWN" << std::endl;
				break;
			}
		}

	}
}




void SceneTestObjLoading::GroupByPipeline()
{
	for (size_t i = 0; i < objects.size(); ++i)
	{
		std::map<int, std::list<int>>& map = pipelineMeshIDs[pipelineID[objects[i].materialIDX]];
		map[objects[i].materialIDX].push_back(i);
	}
}