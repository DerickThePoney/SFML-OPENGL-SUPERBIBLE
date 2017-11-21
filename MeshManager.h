#pragma once
#include "Utility.h"
#include "Mesh.h"

class MeshManager
{
public:
	static MeshManager& Instance();

protected:
	static MeshManager s_kInstance;
	MeshManager();
	~MeshManager();

public:
	bool LoadMeshesFromFile(const std::vector<std::string> akFilenames);
	Mesh* GetMesh(std::string kMeshName);

private:
	std::unordered_map<std::string, Mesh*> m_akMeshes;
};

