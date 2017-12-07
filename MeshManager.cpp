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
	Mesh* newObj = Instantiate();
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

