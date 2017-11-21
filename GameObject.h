#pragma once
#include "Transform.h"

class GameObject
{
public:
	GameObject();
	GameObject(GameObject* pkParent);
	GameObject(GameObject* pkParent, const std::string& rkName);
	GameObject(const std::string& rkName);
	~GameObject();

	GameObject(const GameObject& other) = delete;
	GameObject(GameObject&& other) = delete;

	GameObject& operator=(const GameObject& other) = delete;
	GameObject& operator=(GameObject&& other) = delete;

	//hierarchy functions
	bool AddChild(GameObject* pkObject);
	bool RemoveChild(const UI32 iIndex);
	bool RemoveChild(const std::string& rkName);
	std::size_t GetNumberOfChildren();

	void FindChildrenInHierarchy(const std::string& rkName, std::vector<GameObject*> apkReturnVector);
	GameObject* GetChild(const UI32 iIndex);
	GameObject* GetChild(const std::string& rkName);


	//runtime methods
	void Update(double fElapsedTime);
	virtual void UpdateAllTransformsInHierarchy();

	void ImGUIHierarchy(GameObject*& node_clicked);
	virtual void Inspector();

	void SetName(const std::string& rkName) { m_kName = rkName; }

private:
	bool CheckPresenceInChildren(GameObject* pkGameObject);

protected:
	//tag
	std::string m_kName;

	//hierachy stuff
	std::vector<GameObject*> m_apkChildren;
	GameObject* m_pkParent;
	
public:
	//positional stuff
	Transform m_kTransform;
};

