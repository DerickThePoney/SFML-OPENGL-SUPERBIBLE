#include "stdafx.h"
#include "Scene.h"
#include "VulkanMesh.h"
#include "VulkanRenderer.h"

#include <assimp/Importer.hpp>		// C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags


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
	// We're done. Everything will be cleaned up by the importer destructor
	//return;
}

void Scene::Destroy()
{
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		meshes[i].Destroy(VulkanRenderer::GetDevice());
	}
}

void Scene::Draw(VulkanCommandBuffer & buffer)
{
	for (size_t i = 0; i < meshes.size(); ++i)
	{
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
		meshes.push_back(vMesh);
	}
}
