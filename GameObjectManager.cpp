#include "stdafx.h"
#include "GameObjectManager.h"


GameObjectManager* GameObjectManager::s_kInstance = nullptr;

GameObjectManager * GameObjectManager::Instance()
{
	if (s_kInstance == nullptr)
	{
		s_kInstance = new GameObjectManager();
	}

	return s_kInstance;
}

void GameObjectManager::Delete()
{
	SAFE_DELETE(s_kInstance);
}

GameObjectManager::GameObjectManager()
{
}


GameObjectManager::~GameObjectManager()
{
	auto it = m_akGameObjectsMap.begin();
	while (it != m_akGameObjectsMap.end())
	{
		if (it->second)
		{
			SAFE_DELETE(it->second);
		}
		it = m_akGameObjectsMap.erase(it);
	}
}

GameObject * GameObjectManager::Instantiate(GameObject* prefab)
{
	GameObject* newObj;
	if (prefab == nullptr)
	{
		newObj = new GameObject();
	}
	else
	{
		newObj = new GameObject(*prefab);
	}
	
	m_akGameObjectsMap[newObj->GetID()] = newObj;

	return newObj;
}

bool GameObjectManager::Destroy(GameObject * pkObj)
{
	if (pkObj == nullptr)
	{
		return false;
	}

	auto itFind = m_akGameObjectsMap.find(pkObj->GetID());
	if (itFind != m_akGameObjectsMap.end())
	{
		while (itFind->second->GetNumberOfChildren() > 0)
		{
			GameObject * pkChild = itFind->second->GetChild(0);
			itFind->second->RemoveChild(0);
			Destroy(pkChild);
		}

		SAFE_DELETE(itFind->second);
		m_akGameObjectsMap.erase(itFind);
	}

	return true;
}
