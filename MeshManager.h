#pragma once
#include "Utility.h"
#include "BMesh.h"
#include "Mesh.h"
#include "ObjLoader.h"

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
	Mesh* InstantiateFromFile(const std::string& kFilename);
	bool Destroy(Mesh* pkObj);

	Mesh* FindFromID(UI32 uiID);

private:
	void ReleaseRessourceFromID(const UI32& id);

private:
	std::unordered_map<UI32, Mesh*> m_akMeshes;
	std::map<std::string, UI32> m_akRessourceToID;
};

