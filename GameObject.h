#pragma once
#include "Transform.h"
#include "MeshRendererComponent.h"
#include "IComponent.h"


class GameObject
{

	friend class GameObjectManager;
protected:
	GameObject();
	GameObject(GameObject* pkParent);
	GameObject(GameObject* pkParent, const std::string& rkName);
	GameObject(const std::string& rkName);
	~GameObject();

	GameObject(const GameObject& other);
	GameObject(GameObject&& other) = delete;

	GameObject& operator=(const GameObject& other) = delete;
	GameObject& operator=(GameObject&& other) = delete;

public:
	//hierarchy functions
	bool AddChild(GameObject* pkObject);
	bool RemoveChild(const UI32 iIndex);
	bool RemoveChild(const std::string& rkName);
	void ClearChildren();
	std::size_t GetNumberOfChildren();

	void FindChildrenInHierarchy(const std::string& rkName, std::vector<GameObject*> apkReturnVector);
	GameObject* GetChild(const UI32 iIndex);
	GameObject* GetChild(const std::string& rkName);


	//runtime methods
	void Initialise();
	void Update(double fElapsedTime);
	virtual void UpdateAllTransformsInHierarchy();
	void OnPreRender();

	void ImGUIHierarchy(GameObject*& node_clicked);
	virtual void Inspector();

	void SetName(const std::string& rkName) { m_kName = rkName; }

	UI32 GetID() const { return m_uiID; }

	template<typename T>
	std::shared_ptr<T> FindComponent()
	{
		for (UI32 i = 0; i < m_kComponent.size(); ++i)
		{
			const RTTI thisType = m_kComponent[i]->GetType();
			if (thisType == T::TYPE)
				return std::dynamic_pointer_cast<T>(m_kComponent[i]);
		}
		return nullptr;
	}

	template<typename T>
	std::shared_ptr<T> AddComponent()
	{
		IComponentPtr pkComponent = CREATE_NEW_COMPONENT(T::TYPE, this);

		if (pkComponent != nullptr)
		{
			m_kComponent.push_back(pkComponent);
			pkComponent->Init();
		}

		return std::dynamic_pointer_cast<T>(pkComponent);
	}


	void AddComponent(const RTTI* pkComponentType);

	template<class Archive>
	void save(Archive & archive) const
	{

		archive(CEREAL_NVP(m_kName), m_kTransform);// , CEREAL_NVP(akComponentsNames));
		archive(CEREAL_NVP(m_kComponent));
		
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
		//std::vector<std::string> akComponentsNames;
		archive(CEREAL_NVP(m_kName), m_kTransform);
		DEARCHIVE_WITH_DEFAULT(m_kComponent, std::vector<std::shared_ptr<IComponent>>());

		for (UI32 i = 0; i < m_kComponent.size(); ++i)
		{
			m_kComponent[i]->m_pkParent = this;
		}

		std::size_t uiNbChildren = 0;
		DEARCHIVE_WITH_DEFAULT(uiNbChildren, 0);

		for (size_t i = 0; i < uiNbChildren; ++i)
		{
			GameObject * obj = GameObjectManager::Instance()->Instantiate();
			DEARCHIVE_NO_DEFAULT_FROM_NVP(*obj, "GameObject", obj);
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

	//IComponents
	std::vector<std::shared_ptr<IComponent>> m_kComponent;
};

