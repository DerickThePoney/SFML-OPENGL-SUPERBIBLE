#include "stdafx.h"
#include "MeshManager.h"

MeshManager* MeshManager::s_kInstance = nullptr;

MeshManager * MeshManager::Instance()
{
	if (s_kInstance == nullptr)
	{
		s_kInstance = new MeshManager();
	}

	return s_kInstance;
}

void MeshManager::Delete()
{
	SAFE_DELETE(s_kInstance);
}

MeshManager::MeshManager()
{
}


MeshManager::~MeshManager()
{
	auto it = m_akMeshes.begin();
	while (it != m_akMeshes.end())
	{
		if (it->second)
		{
			SAFE_DELETE(it->second);
		}
		it = m_akMeshes.erase(it);
	}
}

Mesh * MeshManager::Instantiate()
{
	Mesh* newObj = new Mesh();
	m_akMeshes[newObj->m_uiMeshID] = newObj;
	return newObj;
}

Mesh * MeshManager::InstantiateFromFile(const std::string & kFilename)
{
	//check if is it not already loaded
	auto itFind = m_akRessourceToID.find(kFilename);
	Mesh* newObj;
	
	if (itFind == m_akRessourceToID.end())
	{
		//instantiate it
		newObj = Instantiate();
		std::ifstream ifstr(kFilename);

		if (kFilename.find(".bin") != kFilename.npos)
		{
			cereal::BinaryInputArchive output(ifstr);
			output(*newObj);
		}
		else if (kFilename.find(".xml") != kFilename.npos)
		{
			cereal::XMLInputArchive output(ifstr);
			output(*newObj);

			if (newObj->m_bIsProcedural)
			{
				static std::default_random_engine skRNG(0);
				
				BMesh bmesh;
				MakeSpaceShip kSpaceship(skRNG());
				kSpaceship.Apply(bmesh);
				bmesh.BuildMesh(*newObj);
			}
		}
		else if (kFilename.find(".obj") != kFilename.npos)
		{
			bool res = ObjLoader::LoadFromObjFile(newObj, kFilename);
		}
		else if (kFilename.find(".ply") != kFilename.npos)
		{
			bool res = PLYLoader::LoadFromPLYFile(newObj, kFilename);
		}

		newObj->LoadBuffersOnGraphicsCard();

		m_akRessourceToID[kFilename] = newObj->m_uiMeshID;
	}
	else
	{
		newObj = m_akMeshes[itFind->second];
	}

	return newObj;
}

bool MeshManager::Destroy(Mesh * pkObj)
{
	if (pkObj == nullptr)
	{
		return false;
	}

	auto itFind = m_akMeshes.find(pkObj->m_uiMeshID);
	if (itFind != m_akMeshes.end())
	{
		ReleaseRessourceFromID(itFind->second->m_uiMeshID);
		
		SAFE_DELETE(itFind->second);
		m_akMeshes.erase(itFind);
	}

	return true;
}

Mesh * MeshManager::FindFromID(UI32 uiID)
{
	auto itFind = m_akMeshes.find(uiID);
	return (itFind == m_akMeshes.end()) ? nullptr : itFind->second;
}

void MeshManager::ReleaseRessourceFromID(const UI32 & id)
{
	auto it = m_akRessourceToID.begin();
	for (; it != m_akRessourceToID.end(); ++it)
	{
		if (it->second == id)
		{
			m_akRessourceToID.erase(it);
			return;
		}
	}
	return;
}

