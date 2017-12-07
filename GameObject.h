#pragma once
#include "Transform.h"
#include "MeshRendererComponent.h"


class GameObject
{

	friend class GameObjectManager;
protected:
	GameObject();
	GameObject(GameObject* pkParent);
	GameObject(GameObject* pkParent, const std::string& rkName);
	GameObject(const std::string& rkName);
	~GameObject();

	GameObject(const GameObject& other) = delete;
	GameObject(GameObject&& other) = delete;

	GameObject& operator=(const GameObject& other) = delete;
	GameObject& operator=(GameObject&& other) = delete;

public:
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

	UI32 GetID() const { return m_uiID; }

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(CEREAL_NVP(m_kName), m_kTransform, m_kMeshRenderer);
		
		//dump children
		std::size_t uiNbChildren = m_apkChildren.size();
		archive(CEREAL_NVP(uiNbChildren));

		for (size_t i = 0; i < uiNbChildren; ++i)
		{
			archive(cereal::make_nvp("GameObject", (*m_apkChildren[i])));
		}
	}

	template<class Archive>
	void load(Archive& archive)
	{
		archive(CEREAL_NVP(m_kName), m_kTransform, m_kMeshRenderer);

		std::size_t uiNbChildren = m_apkChildren.size();
		archive(CEREAL_NVP(uiNbChildren));

		for (size_t i = 0; i < uiNbChildren; ++i)
		{
			GameObject * obj = GameObjectManager::Instance()->Instantiate();
			archive(cereal::make_nvp("GameObject", (*obj)));
			AddChild(obj);
		}
	}

private:
	bool CheckPresenceInChildren(GameObject* pkGameObject);

protected:
	//ID stuff
	static UI32 s_uiIDCounter;
	UI32 m_uiID;

	//tag
	std::string m_kName;

	//hierachy stuff
	std::vector<GameObject*> m_apkChildren;
	GameObject* m_pkParent;
	
public:
	//positional stuff
	Transform m_kTransform;

	//Visuals
	MeshRendererComponent m_kMeshRenderer;
};

