#include "stdafx.h"
#include "MeshManager.h"

MeshManager* MeshManager::s_kInstance = nullptr;

MeshManager * MeshManager::Instance()
{
	if (s_kInstance == nullptr)
		s_kInstance = new MeshManager();
	return s_kInstance;
}

void MeshManager::Delete()
{
	delete s_kInstance;
}

MeshManager::MeshManager()
{
}


MeshManager::~MeshManager()
{
}

void MeshManager::AddMesh(const std::string kMeshName, Mesh * pkMesh)
{
}
