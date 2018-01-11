#include "stdafx.h"
#include "MaterialManager.h"

MaterialManager* Singleton<MaterialManager>::s_kInstance = nullptr;

MaterialManager::MaterialManager()
{
}


MaterialManager::~MaterialManager()
{
	auto it = m_akMaterials.begin();
	while (it != m_akMaterials.end())
	{
		if (it->second)
		{
			SAFE_DELETE(it->second);
		}
		it = m_akMaterials.erase(it);
	}
}

Material * MaterialManager::Instantiate()
{
	Material* newObj = new Material();
	m_akMaterials[newObj->m_uiMaterialID] = newObj;
	return newObj;
}

Material * MaterialManager::Instantiate(Material * pkMaterial)
{
	Material* newObj = new Material(*pkMaterial);
	m_akMaterials[newObj->m_uiMaterialID] = newObj;
	return newObj;
}

Material * MaterialManager::InstantiateFromFile(const std::string & kFilename)
{
	//check if is it not already loaded
	auto itFind = m_akRessourceToID.find(kFilename);
	Material* newObj;

	if (itFind == m_akRessourceToID.end())
	{
		//instantiate it
		newObj = Instantiate();
		if (!newObj->InitMaterialFromRessource(kFilename))
		{
			Destroy(newObj);
			return nullptr;
		}

		m_akRessourceToID[kFilename].push_back(newObj->m_uiMaterialID);
	}
	else
	{
		newObj = m_akMaterials[*itFind->second.begin()];// Instantiate(m_akMaterials[*itFind->second.begin()]);
	}

	return newObj;
}

bool MaterialManager::Destroy(Material * pkObj)
{
	if (pkObj == nullptr)
	{
		return false;
	}

	auto itFind = m_akMaterials.find(pkObj->m_uiMaterialID);
	if (itFind != m_akMaterials.end())
	{
		ReleaseRessourceFromID(itFind->second->m_uiMaterialID);

		SAFE_DELETE(itFind->second);
		m_akMaterials.erase(itFind);
	}

	return true;
}

Material * MaterialManager::FindFromID(UI32 uiID)
{
	auto itFind = m_akMaterials.find(uiID);
	return (itFind == m_akMaterials.end()) ? nullptr : itFind->second;
}

void MaterialManager::ReleaseRessourceFromID(const UI32 & id)
{
	auto it = m_akRessourceToID.begin();
	for (; it != m_akRessourceToID.end(); ++it)
	{
		for (auto itL = it->second.begin(); itL != it->second.end(); ++itL)
		{
			if (*itL == id)
			{
				it->second.erase(itL);
				if (it->second.empty()) m_akRessourceToID.erase(it);
				return;
			}
		}
	}
	return;
}
