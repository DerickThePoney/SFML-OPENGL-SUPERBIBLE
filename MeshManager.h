#pragma once
#include "Utility.h"

class Mesh;
class MeshManager
{
public:
	static MeshManager* Instance();
	static void Delete();

protected:
	static MeshManager* s_kInstance;
	MeshManager();
	~MeshManager();

public:
	
	void AddMesh(const std::string kMeshName, Mesh* pkMesh);

	Mesh* GetMesh(const std::string& kMeshName);
	Mesh* GetMesh(UI32 uiMeshID);
private:
	std::unordered_map<UI32, Mesh*> m_akMeshes;
	std::unordered_map<std::string, UI32> m_akMeshesNames;
};

