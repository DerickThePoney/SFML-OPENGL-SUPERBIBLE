#pragma once
#include "Utility.h"
#include "Mesh.h"

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
	Mesh* Instantiate();
	bool Destroy(Mesh* pkObj);

	Mesh* FindFromID(UI32 uiID);

private:
	std::unordered_map<UI32, Mesh*> m_akMeshes;
};

