#pragma once
#include "GameObject.h"

class GameObjectManager
{
public:
	static GameObjectManager* Instance();
	static void Delete();

protected:
	static GameObjectManager* s_kInstance;
	GameObjectManager();
	~GameObjectManager();

public:
	GameObject* Instantiate();
	bool Destroy(GameObject* pkObj);



private:
	std::unordered_map<UI32, GameObject*> m_akGameObjectsMap;
};

