#include "stdafx.h"
#include "MeshManager.h"

MeshManager MeshManager::s_kInstance = MeshManager();

MeshManager & MeshManager::Instance()
{
	return s_kInstance;
}

MeshManager::MeshManager()
{
}


MeshManager::~MeshManager()
{
}
