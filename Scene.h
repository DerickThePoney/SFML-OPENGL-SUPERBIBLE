#pragma once
#include "GameObjectManager.h"
#include "GameObject.h"
#include "Camera.h"

class Scene
{
public:
	Scene();
	~Scene();

	void Initialise(UI32 width, UI32 height);
	void Terminate();

	void Update(double deltaTime);

	void OnPreRender();

	void OnResize(unsigned int width, unsigned int height);

	//Scene API
	Camera& GetCamera() { return m_kCamera; }
	void AddRootObject(GameObject* kObject) { m_akRootObjects.push_back(kObject); }

	void ExtractVisibleObjectList(std::vector<MeshRendererComponent *>& akObjects);

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(CEREAL_NVP(m_kFilename), CEREAL_NVP(m_kCamera));

		std::size_t uiNbChildren = m_akRootObjects.size();
		archive(CEREAL_NVP(uiNbChildren));

		for (size_t i = 0; i < uiNbChildren; ++i)
		{
			archive(cereal::make_nvp("GameObject", (*m_akRootObjects[i])));
		}
	}

	template<class Archive>
	void load(Archive& archive)
	{
		archive(CEREAL_NVP(m_kFilename), CEREAL_NVP(m_kCamera));

		std::size_t uiNbChildren = m_akRootObjects.size();
		archive(CEREAL_NVP(uiNbChildren));

		for (size_t i = 0; i < uiNbChildren; ++i)
		{
			GameObject * obj = GameObjectManager::Instance()->Instantiate();
			archive(cereal::make_nvp("GameObject", (*obj)));
			m_akRootObjects.push_back(obj);
		}
	}


private:
	void ExtractVisibleObjectsInHierarchy(GameObject* base, std::vector<MeshRendererComponent *>& akObjects);

	void TransformsUpdate();
	void IMGuiStuffs();

private:

	std::string m_kFilename;
	Camera m_kCamera;
	std::vector<GameObject*> m_akRootObjects;

	OGLTexture2D m_kTexture;
};

